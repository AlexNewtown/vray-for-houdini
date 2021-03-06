//
// Copyright (c) 2015-2018, Chaos Software Ltd
//
// V-Ray For Houdini
//
// ACCESSIBLE SOURCE CODE WITHOUT DISTRIBUTION OF MODIFICATION LICENSE
//
// Full license text:
//  https://github.com/ChaosGroup/vray-for-houdini/blob/master/LICENSE
//

#include "vfh_vray_cloud.h"
#include "vfh_log.h"
#include "vfh_defines.h"
#include "vfh_hou_utils.h"

#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QRegExp>
#include <QMainWindow>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QTextBrowser>
#include <QProgressBar>
#include <QPushButton>
#include <QQueue>
#include <QCloseEvent>

#include <RE/RE_Window.h>

using namespace VRayForHoudini;
using namespace Cloud;

/// JSON config key for the executable file path.
static const QString keyExecutable("executable");

/// Resolved V-Ray Cloud Client binary file path.
static QString vrayCloudClient;

/// Flag indicatig that we've already tried to resolved
/// V-Ray Cloud Client binary file path.
static int vrayCloudClientChecked(false);

/// Not allowed characters regular expression.
static QRegExp cloudNameFilter("[^a-zA-Z\\d\\s\\_\\-.,\\(\\)\\[\\]]");

/// HTML log block format.
static const QString preTag("<pre>%1</pre>");

/// URL match to replace with link.
static const QRegExp urlMatch("((?:https?|ftp)://\\S+)");
static const QString urlMatchReplace("<a href=\"\\1\">\\1</a>");

/// Frame range argument format.
static const QString frameRangeFmt("%1-%2");

/// Only letters, digits, spaces and _-.,()[] allowed.
static QString filterName(const QString &value)
{
	QString filteredValue(value);
	return filteredValue.remove(cloudNameFilter);
}

/// Parses vcloud.json and extracts executable file path.
static void findVRayCloudClient()
{
	if (vrayCloudClientChecked)
		return;

	QString cgrHome; 
#ifdef _WIN32
	const QProcessEnvironment &env = QProcessEnvironment::systemEnvironment();
	cgrHome = env.value("APPDATA");
#else
	cgrHome = QDir::homePath();
#endif

	QStringList vcloudDirList;
	vcloudDirList << cgrHome;
#ifdef _WIN32
	vcloudDirList << "Chaos Group";
#else
	vcloudDirList << ".ChaosGroup";
#endif
	vcloudDirList << "vcloud";
	vcloudDirList << "client";
	vcloudDirList << "vcloud.json";

	const QString vcloudFilePath = vcloudDirList.join(QDir::separator());

	QFileInfo vcloudFileInfo(vcloudFilePath);
	if (vcloudFileInfo.exists()) {
		QFile vcloudFile(vcloudFileInfo.absoluteFilePath());
		if (vcloudFile.open(QIODevice::ReadOnly|QIODevice::Text)) {
			QJsonDocument vcloudJson = QJsonDocument::fromJson(vcloudFile.readAll());
			vcloudFile.close();

			if (vcloudJson.isObject()) {
				const QJsonObject vcloudObject = vcloudJson.object();
				if (vcloudObject.contains(keyExecutable)) {
					vrayCloudClient = vcloudObject[keyExecutable].toString();
				}
			}
		}
	}

	if (vrayCloudClient.isEmpty()) {
		Log::getLog().error("V-Ray Cloud Client is not found! Please, visit https://www.chaosgroup.com/cloud");
	}

	vrayCloudClientChecked = true;
}

JobFilePath::JobFilePath(const QString &filePath)
	: filePath(filePath)
{}

JobFilePath::~JobFilePath()
{
	removeFilePath(filePath);
}

QString JobFilePath::createFilePath()
{
	QString tmpFilePath;

	QFileInfo tempDir(QDir::tempPath());
	if (!tempDir.isDir() || !tempDir.isWritable()) {
		Log::getLog().error("Temporary location \"%s\" is not writable!",
							qPrintable(tempDir.absolutePath()));
	}
	else {
		QFileInfo tempFile(tempDir.absoluteFilePath(), "vfhVRayCloud.vrscene");
		if (tempFile.exists()) {
			removeFilePath(tempFile.absoluteFilePath());
		}

		tmpFilePath = tempFile.absoluteFilePath();
	}

	return tmpFilePath;
}

void JobFilePath::removeFilePath(const QString &filePath)
{
	if (!QFile::exists(filePath))
		return;

	const int removeRes = QFile::remove(filePath);
	if (!removeRes) {
		Log::getLog().error("Failed to remove \"%s\"!", qPrintable(filePath));
	}
}

Job::Job(const QString &sceneFile)
	: sceneFile(sceneFile)
{}

void Job::setProject(const QString &value)
{
	project = filterName(value);
}

void Job::setName(const QString &value)
{
	name = filterName(value);
}

void Job::toArguments(const Job &job, QStringList &arguments)
{
	arguments << "--project" << job.getProject();
	arguments << "--name" << job.getName();
	arguments << "--sceneFile" << job.sceneFile;

	arguments << "--renderMode" << job.renderMode;

	arguments << "--width" << QString::number(job.width);
	arguments << "--height" << QString::number(job.height);

	if (job.animation) {
		arguments << "--animation";
		arguments << "--frameRange" << frameRangeFmt.arg(job.frameRange.first).arg(job.frameRange.second);
		arguments << "--frameStep" << QString::number(job.frameStep);
	}

	if (job.ignoreWarnings) {
		arguments << "--ignoreWarnings";
	}
	if (job.noProgress) {
		arguments << "--no-progress";
	}
	if (job.vr) {
		arguments << "--vr";
	}
	if (!job.colorCorrectionsFile.isEmpty()) {
		arguments << "--colorCorrectionsFile" << job.colorCorrectionsFile;
	}
}

/// Cloud command holder.
struct CloudCommand {
	/// Executable.
	QString command;

	/// Arguments.
	QStringList arguments;
};

/// A type for cloud commands queue.
typedef QQueue<CloudCommand> CloudCommands;

/// Upload progress window.
class CloudWindow
	: public QMainWindow
{
	Q_OBJECT

public:
	CloudWindow(CloudWindow* &cloudWindowInstance, const QString &jobFile, QWidget *parent)
		: QMainWindow(parent)
		, jobFile(jobFile)
		, cloudWindowInstance(cloudWindowInstance)
	{
		setWindowTitle(tr("V-Ray Cloud Client: Scene upload"));
		setAttribute(Qt::WA_DeleteOnClose);
		setWindowFlags(Qt::Window|Qt::Tool);

		setupUI();

		connectSlotsUI();
		connectSlotsProcess();

		resize(1024, 768);
	}

	~CloudWindow() VRAY_DTOR_OVERRIDE {
		terminateProcess();

		cloudWindowInstance = nullptr;
	}

	void setupUI() {
		editor = new QTextBrowser(this);
		editor->setReadOnly(true);
		editor->setOpenLinks(true);
		editor->setOpenExternalLinks(true);

		progress = new QProgressBar(this);

		stopButton = new QPushButton("Abort", this);

		QHBoxLayout *progressLayout = new QHBoxLayout;
		progressLayout->setMargin(0);
		progressLayout->setSpacing(10);
		progressLayout->addWidget(progress);
		progressLayout->addWidget(stopButton);

		QVBoxLayout *mainLayout = new QVBoxLayout;
		mainLayout->setMargin(10);
		mainLayout->setSpacing(10);
		mainLayout->addWidget(editor);
		mainLayout->addLayout(progressLayout);

		QWidget *centralWidget = new QWidget(this);
		centralWidget->setLayout(mainLayout);

		setCentralWidget(centralWidget);
	}

	void connectSlotsUI() const {
		connect(stopButton, SIGNAL(clicked()),
		        this, SLOT(onPressAbort()));
	}

	void connectSlotsProcess() const {
		connect(&proc, SIGNAL(readyReadStandardOutput()),
		        this, SLOT(onProcStdOutput()));

		connect(&proc, SIGNAL(readyReadStandardError()),
		        this, SLOT(onProcStdError()));

		connect(&proc, SIGNAL(error(QProcess::ProcessError)),
		        this, SLOT(onProcError()));

		connect(&proc, SIGNAL(finished(int)),
		        this, SLOT(onProcFinished()));
	}

	void uploadScene(const CloudCommands &job) {
		commands = job;

		// Busy progress.
		progress->setMinimum(0);
		progress->setMaximum(0);

		executeVRayCloudClient();
	}

	void executeVRayCloudClient() {
		if (commands.empty())
			return;

		const CloudCommand cmd = commands.dequeue();

		Log::getLog().info("Calling V-Ray Cloud Client: %s", qPrintable(cmd.arguments.join(" ")));

		proc.start(cmd.command, cmd.arguments, QIODevice::ReadOnly);
	}

	void uploadCompleted() const {
		progress->hide();
		stopButton->hide();
	}

	void appendText(const QByteArray &data) const {
		QString text(QString(data).trimmed());
		if (text.isEmpty())
			return;

		text = text.replace(urlMatch, urlMatchReplace);

		editor->insertHtml(preTag.arg(text));
		editor->insertPlainText("\n");
	}

private Q_SLOTS:
	void onPressAbort() {
		close();
	}

	void onProcError() {
		commands.clear();

		uploadCompleted();
	}

	void onProcFinished() {
		if (commands.isEmpty()) {
			uploadCompleted();
			return;
		}

		executeVRayCloudClient();
	}

	void onProcStdError() {
		appendText(proc.readAllStandardError());
	}

	void onProcStdOutput() {
		appendText(proc.readAllStandardOutput());
	}

protected:
	void closeEvent(QCloseEvent *ev) VRAY_OVERRIDE {
		QMessageBox::StandardButton res(QMessageBox::Yes);

		if (proc.state() != QProcess::NotRunning ||
		    !commands.isEmpty())
		{
			res = QMessageBox::warning(this, tr("V-Ray Cloud Client: Abort upload"),
			                           tr("This will abort scene upload. Are you sure?"),
			                           QMessageBox::Cancel | QMessageBox::Yes,
			                           QMessageBox::Yes);
		}

		if (res != QMessageBox::Yes) {
			ev->ignore();
		}
		else {
			terminateProcess();
			ev->accept();
		}
	}

	void terminateProcess() {
		if (proc.state() == QProcess::NotRunning)
			return;

		progress->setFormat("Aborting upload...");
		progress->setMinimum(0);
		progress->setMaximum(1);
		progress->setValue(1);

		stopButton->setEnabled(false);

		commands.clear();

		proc.terminate();
		proc.waitForFinished(2000);
	}

	QTextBrowser *editor = nullptr;
	QProgressBar *progress = nullptr;
	QPushButton *stopButton = nullptr;

	/// Process.
	QProcess proc;

	/// Upload command queue.
	CloudCommands commands;

private:
	/// Job file to delete temp export file.
	JobFilePath jobFile;

	/// Window instance.
	CloudWindow* &cloudWindowInstance;
};

/// We allow only one window instance.
static CloudWindow *cloudWindowInstance = nullptr;

/// A blocking call to the V-Ray Cloud Client. Used in batch version.
static void executeVRayCloudClient(const CloudCommand &cmd)
{
	Log::getLog().info("Calling V-Ray Cloud Client: %s", qPrintable(cmd.arguments.join(" ")));

	QProcess vrayCloudClientProc;
	vrayCloudClientProc.setProcessChannelMode(QProcess::ForwardedChannels);
	vrayCloudClientProc.start(cmd.command, cmd.arguments, QIODevice::ReadOnly);
	vrayCloudClientProc.waitForFinished(-1);
}

int VRayForHoudini::Cloud::submitJob(const Job &job)
{
	findVRayCloudClient();

	if (vrayCloudClient.isEmpty())
		return false;

	Log::getLog().info("Using V-Ray Cloud Client: \"%s\"", qPrintable(vrayCloudClient));

	CloudCommands commands;

	CloudCommand createCmd;
	createCmd.command = vrayCloudClient;
	createCmd.arguments << "project" << "create" << "--name" << job.getProject();
	commands.enqueue(createCmd);

	CloudCommand submitCmd;
	submitCmd.command = vrayCloudClient;
	submitCmd.arguments << "job" << "submit";
	Job::toArguments(job, submitCmd.arguments);
	commands.enqueue(submitCmd);

	if (!HOU::isUIAvailable()) {
		JobFilePath jobFile(job.sceneFile);
		for (const CloudCommand &cmd : commands) {
			executeVRayCloudClient(cmd);
		}
	}
	else if (!cloudWindowInstance) {
		cloudWindowInstance = new CloudWindow(cloudWindowInstance, job.sceneFile, RE_Window::mainQtWindow());
		cloudWindowInstance->show();
		cloudWindowInstance->uploadScene(commands);
	}

	return true;
}

int VRayForHoudini::Cloud::isClientAvailable()
{
	findVRayCloudClient();

	if (!vrayCloudClient.isEmpty())
		return true;

	// findVRayCloudClient() will print text error if client is not found.
	if (HOU::isUIAvailable()) {
		if (!cloudWindowInstance) {
			cloudWindowInstance = new CloudWindow(cloudWindowInstance, QString(), RE_Window::mainQtWindow());
			cloudWindowInstance->appendText("V-Ray Cloud Client is not found!");
			cloudWindowInstance->appendText("Please, visit https://www.chaosgroup.com/cloud");
			cloudWindowInstance->uploadCompleted();
			cloudWindowInstance->show();
		}
	}

	return false;
}

#ifndef Q_MOC_RUN
#include <vfh_vray_cloud.cpp.moc>
#endif

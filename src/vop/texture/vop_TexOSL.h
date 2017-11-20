//
// Copyright (c) 2015-2017, Chaos Software Ltd
//
// V-Ray For Houdini
//
// ACCESSIBLE SOURCE CODE WITHOUT DISTRIBUTION OF MODIFICATION LICENSE
//
// Full license text: https://github.com/ChaosGroup/vray-for-houdini/blob/master/LICENSE
//

#ifndef VRAY_FOR_HOUDINI_VOP_NODE_TEX_OSL_H
#define VRAY_FOR_HOUDINI_VOP_NODE_TEX_OSL_H

#include "vop_node_base.h"
#include "vfh_prm_templates.h"

namespace VRayForHoudini {
namespace VOP {

class TexOSL
	: public VOP::NodeBase {
public:
	/// Description of a single OSL parameter
	struct ParamInfo
	{
		std::string name; ///< The name of the parameter
		VOP_Type type; ///< VOP_Type of the parameter (color, vector, point, normal, float, int or string)
		bool validDefault; ///< True if the default value is taken from code
		double numberDefault[3]; ///< stores (r,g,b) or (x,y,z) or (float) or (int)
		std::string stringDefault; ///< stores the string default value
	};

	typedef std::vector<ParamInfo> OSLParamList;

	TexOSL(OP_Network *parent, const char *name, OP_Operator *entry)
		: NodeBase(parent, name, entry), m_codeHash(0) {}

	PluginResult asPluginDesc(Attrs::PluginDesc &pluginDesc,
		VRayExporter &exporter,
		ExportContext *parentContext = nullptr) VRAY_OVERRIDE;

	/// Map index to input label
	virtual const char * inputLabel(unsigned idx) const VRAY_OVERRIDE;
	/// Map index to output label
	virtual const char * outputLabel(unsigned idx) const VRAY_OVERRIDE;

	virtual unsigned getNumVisibleInputs() const VRAY_OVERRIDE;
	/// Get number of outputs (same as maxOutputs)
	virtual unsigned getNumVisibleOutputs() const VRAY_OVERRIDE;
	/// Get number of possible inputs (OSL code inputs + base)
	virtual unsigned orderedInputs() const VRAY_OVERRIDE;

	/// Get number of output 1 + number of base inputs
	virtual unsigned maxOutputs() const VRAY_OVERRIDE;
protected:

	void setPluginType() VRAY_OVERRIDE;

	/// Map input name to index
	virtual int getInputFromName(const UT_String &in) const VRAY_OVERRIDE;
	/// Map output name to index
	virtual int	getOutputFromName(const UT_String &out) const VRAY_OVERRIDE;

	/// Map input name to index
	virtual int getInputFromNameSubclass(const UT_String &in) const VRAY_OVERRIDE;

	/// Map input index to name
	virtual void getInputNameSubclass(UT_String &in, int idx) const VRAY_OVERRIDE;
	/// Map output index to name
	virtual void getOutputNameSubclass(UT_String &out, int idx) const VRAY_OVERRIDE;

	/// Input types always textures so VOP_TYPE_COLOR
	virtual void getInputTypeInfoSubclass(VOP_TypeInfo &type_info, int idx) VRAY_OVERRIDE;
	/// The output type (VOP_TYPE_COLOR)
	virtual void getOutputTypeInfoSubclass(VOP_TypeInfo &type_info, int idx)VRAY_OVERRIDE;

	/// This returns only VOP_TYPE_COLOR, because this TexOSL only supports texture interface
	virtual void getAllowedInputTypeInfosSubclass(unsigned idx, VOP_VopTypeInfoArray &type_infos) VRAY_OVERRIDE;
private:

	/// Get count of inputs generated by OSL
	int customInputsCount() const;

	/// Check if the OSL code hash changed and if needed, re-parse it
	void updateParamsIfNeeded() const;

	/// Get the name of the output color param
	/// NOTE: Use this when the result pointer will be passed to houdini!!
	///       This will copy the data in a buffer that will not move (reallocate)
	const char * getOutputName() const;

	// TODO: Add support for .osl/.oso file path

	Hash::MHash m_codeHash; ///< Hash of the OSL code
	std::vector<std::string> m_inputList; ///< Names of all inputs (string params)
	OSLParamList m_paramList; ///< All params from OSL code (inputs and params)

	std::string m_outputName; ///< The output color parameter name
	mutable char m_outputNameBuff[1024]; ///< Buffer containing the output parameter name, used so we have pointer that will not move
};

} // namespace VOP
} // namespace VRayForHoudini

#endif // #define VRAY_FOR_HOUDINI_VOP_NODE_TEX_OSL_H


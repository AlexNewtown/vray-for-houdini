//
// Copyright (c) 2015-2018, Chaos Software Ltd
//
// V-Ray For Houdini
//
// ACCESSIBLE SOURCE CODE WITHOUT DISTRIBUTION OF MODIFICATION LICENSE
//
// Full license text: https://github.com/ChaosGroup/vray-for-houdini/blob/master/LICENSE
//

#include "vop_MtlMulti.h"

using namespace VRayForHoudini;

// NOTE: Sockets order:
//
//   - Auto. sockets from description
//   - material_1
//   - ...
//   - material_<mtl_count>
//

void VOP::MtlMulti::setPluginType()
{
	pluginType = VRayPluginType::MATERIAL;
	pluginID   = "MtlMulti";
}


const char* VOP::MtlMulti::inputLabel(unsigned idx) const
{
	const int numBaseInputs = VOP::NodeBase::orderedInputs();

	if (idx < numBaseInputs) {
		return VOP::NodeBase::inputLabel(idx);
	}

	const int socketIndex = idx - numBaseInputs + 1;

	return getCreateSocketLabel(socketIndex, "Material %i", socketIndex);
}


int VOP::MtlMulti::getInputFromName(const UT_String &in) const
{
	return getInputFromNameSubclass(in);
}


void VOP::MtlMulti::getInputNameSubclass(UT_String &in, int idx) const
{
	const int numBaseInputs = VOP::NodeBase::orderedInputs();

	if (idx < numBaseInputs) {
		VOP::NodeBase::getInputNameSubclass(in, idx);
	}
	else {
		const int socketIndex = idx - numBaseInputs + 1;
		in = getCreateSocketToken(socketIndex, "mtl_%i", socketIndex);
	}
}


int VOP::MtlMulti::getInputFromNameSubclass(const UT_String &in) const
{
	int inIdx;
	if (in.startsWith("mtl_")) {
		const int numBaseInputs = VOP::NodeBase::orderedInputs();

		int idx = -1;
		sscanf(in.buffer(), "mtl_%i", &idx);

		inIdx = numBaseInputs + idx - 1;
	}
	else {
		inIdx = VOP::NodeBase::getInputFromNameSubclass(in);
	}

	return inIdx;
}


void VOP::MtlMulti::getInputTypeInfoSubclass(VOP_TypeInfo &type_info, int idx)
{
	const int numBaseInputs = VOP::NodeBase::orderedInputs();

	if (idx < numBaseInputs) {
		VOP::NodeBase::getInputTypeInfoSubclass(type_info, idx);
	}
	else {
		type_info.setType(VOP_SURFACE_SHADER);
	}
}

void VOP::MtlMulti::getAllowedInputTypeInfosSubclass(unsigned idx, VOP_VopTypeInfoArray &type_infos)
{
	const int numBaseInputs = VOP::NodeBase::orderedInputs();

	if (idx < numBaseInputs) {
		VOP::NodeBase::getAllowedInputTypeInfosSubclass(idx, type_infos);
	}
	else {
		VOP_TypeInfo vopTypeInfo;
		getInputTypeInfoSubclass(vopTypeInfo, idx);
		if (vopTypeInfo == VOP_TypeInfo(VOP_SURFACE_SHADER)) {
			type_infos.append(VOP_TypeInfo(VOP_TYPE_BSDF));
		}
	}
}

void VOP::MtlMulti::getAllowedInputTypesSubclass(unsigned idx, VOP_VopTypeArray &voptypes)
{
	const int numBaseInputs = VOP::NodeBase::orderedInputs();

	if (idx < numBaseInputs) {
		VOP::NodeBase::getAllowedInputTypesSubclass(idx, voptypes);
	}
	else {
		VOP_TypeInfo vopTypeInfo;
		getInputTypeInfoSubclass(vopTypeInfo, idx);
		if (vopTypeInfo == VOP_TypeInfo(VOP_SURFACE_SHADER)) {
			voptypes.append(VOP_TYPE_BSDF);
		}
	}
}

bool VOP::MtlMulti::willAutoconvertInputType(int input_idx)
{
	const int numBaseInputs = VOP::NodeBase::orderedInputs();

	if (input_idx < numBaseInputs) {
		return VOP::NodeBase::willAutoconvertInputType(input_idx);
	}

	VOP_VopTypeInfoArray typesInfo;
	getAllowedInputTypeInfosSubclass(input_idx, typesInfo);
	if (typesInfo.find(VOP_TypeInfo(VOP_SURFACE_SHADER))) {
		return true;
	}

	return false;
}

int VOP::MtlMulti::customInputsCount() const
{
	// One socket per texture
	const int numCustomInputs = evalInt("mtl_count", 0, 0.0);
	return numCustomInputs;
}


unsigned VOP::MtlMulti::getNumVisibleInputs() const
{
	return orderedInputs();
}


unsigned VOP::MtlMulti::orderedInputs() const
{
	int orderedInputs = VOP::NodeBase::orderedInputs();
	orderedInputs += customInputsCount();

	return orderedInputs;
}


void VOP::MtlMulti::getCode(UT_String &codestr, const VOP_CodeGenContext &)
{
}


OP::VRayNode::PluginResult VOP::MtlMulti::asPluginDesc(Attrs::PluginDesc &pluginDesc, VRayExporter &exporter, ExportContext *parentContext)
{
	const int mtls_count = evalInt("mtl_count", 0, 0.0);

	VRay::ValueList mtls_list;
	VRay::IntList   ids_list;

	for (int i = 1; i <= mtls_count; ++i) {
		const QString mtlSockName = SL("mtl_%i").arg(i);

		OP_Node *mtl_node = VRayExporter::getConnectedNode(this, mtlSockName);
		if (!mtl_node) {
			Log::getLog().warning("Node \"%s\": Material node is not connected to \"%s\", ignoring...",
			                      getName().buffer(), _toChar(mtlSockName));
		}
		else {
			VRay::Plugin mtl_plugin = exporter.exportVop(mtl_node, parentContext);

			if (mtl_plugin.isEmpty()) {
				Log::getLog().error("Node \"%s\": Failed to export material node connected to \"%s\", ignoring...",
				                    getName().buffer(), _toChar(mtlSockName));
			}
			else {
				mtls_list.push_back(VRay::Value(mtl_plugin));
				ids_list.push_back(i - 1);
			}
		}
	}

	if (mtls_list.empty())
		return PluginResultError;

	pluginDesc.add(Attrs::PluginAttr(SL("mtls_list"), mtls_list));
	pluginDesc.add(Attrs::PluginAttr(SL("ids_list"), ids_list));

	OP_Node *mtlid_gen       = VRayExporter::getConnectedNode(this, SL("mtlid_gen"));
	OP_Node *mtlid_gen_float = VRayExporter::getConnectedNode(this, SL("mtlid_gen_float"));

	if (mtlid_gen && !mtlid_gen_float) {
		pluginDesc.add(Attrs::PluginAttr(SL("mtlid_gen_float"), Attrs::PluginAttr::AttrTypeIgnore));
	}

	if (mtlid_gen_float && !mtlid_gen) {
		pluginDesc.add(Attrs::PluginAttr(SL("mtlid_gen"), Attrs::PluginAttr::AttrTypeIgnore));
	}

	return PluginResultContinue;
}

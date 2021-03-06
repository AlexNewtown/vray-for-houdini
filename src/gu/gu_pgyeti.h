//
// Copyright (c) 2015-2018, Chaos Software Ltd
//
// V-Ray For Houdini
//
// ACCESSIBLE SOURCE CODE WITHOUT DISTRIBUTION OF MODIFICATION LICENSE
//
// Full license text: https://github.com/ChaosGroup/vray-for-houdini/blob/master/LICENSE
//

#ifndef VRAY_FOR_HOUDINI_GU_PGYETI_H
#define VRAY_FOR_HOUDINI_GU_PGYETI_H

#include "vfh_includes.h"
#include "vfh_VRayPgYetiRefBase.h"

namespace VRayForHoudini {

/// Yeti hair preview implemented as a packed primitive.
class VRayPgYetiRef
	: public VRayPgYetiRefBase
{
public:
	static GA_PrimitiveTypeId typeId();
	static void install(GA_PrimitiveFactory *gafactory);

	VRayPgYetiRef();
	VRayPgYetiRef(const VRayPgYetiRef &src);
	virtual ~VRayPgYetiRef();

	GU_PackedFactory *getFactory() const VRAY_OVERRIDE;
	GU_PackedImpl *copy() const VRAY_OVERRIDE;
	bool unpack(GU_Detail &destgdp) const VRAY_OVERRIDE;

private:
	int detailRebuild(GU_PrimPacked *prim) VRAY_OVERRIDE;
};

} // namespace VRayForHoudini

#endif // VRAY_FOR_HOUDINI_GU_PGYETI_H

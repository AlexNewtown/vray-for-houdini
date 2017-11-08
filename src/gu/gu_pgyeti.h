//
// Copyright (c) 2015-2017, Chaos Software Ltd
//
// V-Ray For Houdini
//
// ACCESSIBLE SOURCE CODE WITHOUT DISTRIBUTION OF MODIFICATION LICENSE
//
// Full license text: https://github.com/ChaosGroup/vray-for-houdini/blob/master/LICENSE
//

#ifndef VRAY_FOR_HOUDINI_GU_PGYETI_H
#define VRAY_FOR_HOUDINI_GU_PGYETI_H

#include "vfh_primitives.h"

#include <GU/GU_PackedImpl.h>

namespace VRayForHoudini {

#define VFH_VRAY_YETI_PARAMS_COUNT 7
#define VFH_VRAY_YETI_PARAMS (\
(const char *, file, ""),\
(const char *, imageSearchPath, ""),\
(fpreal, density, 1.0),\
(fpreal, length, 1.0),\
(fpreal, width, 1.0),\
(bool, dynamicHairTesselation, true),\
(fpreal, segmentLength, 4.0)\
)

/// Yeti hair preview implemented as a packed primitive.
class VRayPgYetiRef
	: public GU_PackedImpl
{
public:
	static GA_PrimitiveTypeId typeId();
	static void install(GA_PrimitiveFactory *gafactory);

	VRayPgYetiRef();
	VRayPgYetiRef(const VRayPgYetiRef &src);
	~VRayPgYetiRef();

	VFH_MAKE_ACCESSORS(VFH_VRAY_YETI_PARAMS, VFH_VRAY_YETI_PARAMS_COUNT)

	GU_PackedFactory *getFactory() const VRAY_OVERRIDE;
	GU_PackedImpl *copy() const VRAY_OVERRIDE;
	bool isValid() const VRAY_OVERRIDE;
	void clearData() VRAY_OVERRIDE;
	bool save(UT_Options &options, const GA_SaveMap &map) const VRAY_OVERRIDE;
	bool load(const UT_Options &options, const GA_LoadMap &map) VRAY_OVERRIDE;
	void update(const UT_Options &options) VRAY_OVERRIDE;
	bool getBounds(UT_BoundingBox &box) const VRAY_OVERRIDE;
	bool getRenderingBounds(UT_BoundingBox &box) const VRAY_OVERRIDE;
	void getVelocityRange(UT_Vector3 &min, UT_Vector3 &max) const VRAY_OVERRIDE;
	void getWidthRange(fpreal &wmin, fpreal &wmax) const VRAY_OVERRIDE;
	bool unpack(GU_Detail &destgdp) const VRAY_OVERRIDE;
	GU_ConstDetailHandle getPackedDetail(GU_PackedContext *context = 0) const VRAY_OVERRIDE;
	int64 getMemoryUsage(bool inclusive) const VRAY_OVERRIDE;
	void countMemory(UT_MemoryCounter &counter, bool inclusive) const VRAY_OVERRIDE;

	/// Returns current options.
	const UT_Options &getOptions() const { return m_options; }

private:
	/// Build packed detail.
	void detailBuild();

	/// Clear detail.
	void detailClear();

	/// Update detail from options.
	/// @param options New options set.
	/// @returns True if detail was changed, false - otherwise.
	int updateFrom(const UT_Options &options);

	/// Bbox.
	UT_BoundingBox m_bbox;

	/// Geometry detail.
	GU_ConstDetailHandle m_detail;

	/// Current options set.
	UT_Options m_options;
};

} // namespace VRayForHoudini

#endif // VRAY_FOR_HOUDINI_GU_PGYETI_H

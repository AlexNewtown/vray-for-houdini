//
// Copyright (c) 2015-2016, Chaos Software Ltd
//
// V-Ray For Houdini
//
// ACCESSIBLE SOURCE CODE WITHOUT DISTRIBUTION OF MODIFICATION LICENSE
//
// Full license text: https://github.com/ChaosGroup/vray-for-houdini/blob/master/LICENSE
//

#ifndef VRAY_FOR_HOUDINI_VOP_NODE_TEX_DEF_H
#define VRAY_FOR_HOUDINI_VOP_NODE_TEX_DEF_H

#include "vop_texture_output.h"
#include "vop_node_base.h"
#include "vop_TexFalloff.h"
#include "vop_TexRemap.h"
#include "vop_TexGradRamp.h"
#include "vop_TexLayeredMax.h"

namespace VRayForHoudini {
namespace VOP {

#define TEX_DEF(PluginID) NODE_BASE_DEF(TEXTURE, PluginID)

TEX_DEF(BitmapBuffer)

TEX_DEF(ColorTextureToMono)
TEX_DEF(FloatToTex)
TEX_DEF(TexAColor)
TEX_DEF(TexAColorChannel)
TEX_DEF(TexAColorOp)
TEX_DEF(TexBezierCurve)
TEX_DEF(TexBillboardParticle)
TEX_DEF(TexBitmap)
TEX_DEF(TexBlend)
TEX_DEF(TexBlendBumpNormal)
TEX_DEF(TexBulge)
TEX_DEF(TexCellular)
TEX_DEF(TexChecker)
TEX_DEF(TexClamp)
TEX_DEF(TexColor2Scalar)
TEX_DEF(TexColor8Mix)
TEX_DEF(TexColorAndAlpha)
TEX_DEF(TexColorAverage)
TEX_DEF(TexColorCurve)
TEX_DEF(TexColorExponential)
TEX_DEF(TexColorMathBasic)
TEX_DEF(TexColorSwitch)
TEX_DEF(TexColorToFloat)
TEX_DEF(TexCombineColor)
TEX_DEF(TexCombineColorLightMtl)
TEX_DEF(TexCombineFloat)
TEX_DEF(TexCompMax)
TEX_DEF(TexCondition)
TEX_DEF(TexCondition2)
TEX_DEF(TexCurvature)
TEX_DEF(TexDirt)
TEX_DEF(TexDistance)
TEX_DEF(TexDistanceBetween)
TEX_DEF(TexEdges)
TEX_DEF(TexFloat)
TEX_DEF(TexFloatOp)
TEX_DEF(TexFloatPerVertexHairSampler)
TEX_DEF(TexFloatToColor)
TEX_DEF(TexFresnel)
TEX_DEF(TexGrid)
TEX_DEF(TexHSVToRGB)
TEX_DEF(TexHairRootSampler)
TEX_DEF(TexHairSampler)
TEX_DEF(TexICC)
TEX_DEF(TexInt)
TEX_DEF(TexIntToFloat)
TEX_DEF(TexInterpLinear)
TEX_DEF(TexInvert)
TEX_DEF(TexInvertFloat)
TEX_DEF(TexLuminance)
TEX_DEF(TexLut)
TEX_DEF(TexMaskMax)
TEX_DEF(TexMeshVertexColor)
TEX_DEF(TexMeshVertexColorChannel)
TEX_DEF(TexMeshVertexColorWithDefault)
TEX_DEF(TexMix)
TEX_DEF(TexMotionOcclusion)
TEX_DEF(TexMulti)
TEX_DEF(TexMultiFloat)
TEX_DEF(TexMultiProjection)
TEX_DEF(TexNoise)
TEX_DEF(TexNormalMapFlip)
TEX_DEF(TexOCIO)
TEX_DEF(TexOutput)
TEX_DEF(TexParticleDiffuse)
TEX_DEF(TexParticleId)
TEX_DEF(TexParticleSampler)
TEX_DEF(TexParticleShape)
TEX_DEF(TexPerVertexHairSampler)
TEX_DEF(TexPlusMinusAverage)
TEX_DEF(TexPtex)
TEX_DEF(TexRGBMultiplyMax)
TEX_DEF(TexRGBTintMax)
TEX_DEF(TexRGBToHSV)
TEX_DEF(TexRamp)
TEX_DEF(TexRaySwitch)
TEX_DEF(TexRgbaCombine)
TEX_DEF(TexRgbaSplit)
TEX_DEF(TexRock)
TEX_DEF(TexSampler)
TEX_DEF(TexScalarCurve)
TEX_DEF(TexScalarExponential)
TEX_DEF(TexScalarHairRootSampler)
TEX_DEF(TexScalarMathBasic)
TEX_DEF(TexSetRange)
TEX_DEF(TexSky)
TEX_DEF(TexSmoke)
TEX_DEF(TexSnow)
TEX_DEF(TexSoftbox)
TEX_DEF(TexSpeckle)
TEX_DEF(TexSplat)
TEX_DEF(TexStencil)
TEX_DEF(TexSurfIncidence)
TEX_DEF(TexSurfaceLuminance)
TEX_DEF(TexSwirl)
TEX_DEF(TexSwitch)
TEX_DEF(TexSwitchFloat)
TEX_DEF(TexSwitchInt)
TEX_DEF(TexSwitchMatrix)
TEX_DEF(TexSwitchTransform)
TEX_DEF(TexTemperature)
TEX_DEF(TexTemperatureToColor)
TEX_DEF(TexThickness)
TEX_DEF(TexTiles)
TEX_DEF(TexUVW)
TEX_DEF(TexUVWGenToTexture)
TEX_DEF(TexUserColor)
TEX_DEF(TexUserScalar)
TEX_DEF(TexVectorOp)
TEX_DEF(TexVectorProduct)
TEX_DEF(TexVectorToColor)
TEX_DEF(TexVertexColorDirect)
TEX_DEF(TexWater)
TEX_DEF(TransformToTex)
TEX_DEF(texRenderHair)
TEX_DEF(TexTriPlanar)

} // namespace VOP
} // namespace VRayForHoudini

#endif // VRAY_FOR_HOUDINI_VOP_NODE_TEX_DEF_H

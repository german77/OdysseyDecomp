#pragma once

#include <basis/seadTypes.h>
#include <gfx/seadColor.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>
#include <nn/gfx/gfx_VariationBase.h>
#include <nvn/nvn.h>

namespace agl {
class DrawContext;
class RenderBuffer;
class GPUMemVoidAddr;
class ConstGPUMemVoidAddr;
}  // namespace agl

namespace nn::g3d {
class MaterialObj;
class ViewVolume;
class ResVertex;
class ResMesh;
}  // namespace nn::g3d

namespace nn::gfx {
template <class TTarget>
class TBuffer;
}  // namespace nn::gfx

namespace sead {
class GraphicsContext;
class GraphicsContextMRT;
}  // namespace sead

namespace al {
struct ActorInitInfo;
class Scene;
class LiveActor;
class FootPrintRequestInfo;
class ModelShaderHolder;
class ModelCtrl;

void calcFovByProjection(sead::Vector2f*, const sead::Matrix44f&);
void setDepthFuncNearDraw(sead::GraphicsContext*);
void setDepthFuncFarDraw(sead::GraphicsContext*);
void setDepthFuncNearDraw(sead::GraphicsContextMRT*);
void setDepthFuncFarDraw(sead::GraphicsContextMRT*);
bool isUsingReverseProjection();
bool isDepthFuncReverse();
f32 getDepthClearValue();
void calcViewVolume(nn::g3d::ViewVolume*, const sead::Matrix34f&, const sead::Matrix44f&);
void calcAndExpandViewVolume(nn::g3d::ViewVolume*, const sead::Matrix34f&, const sead::Matrix44f&,
                             const sead::Vector3f&, float);
bool isUseBlend(const nn::g3d::MaterialObj*);
bool isUseXluZPrepass(const nn::g3d::MaterialObj*);
const char* getBlendMode(const nn::g3d::MaterialObj*);
// TODO: Replace return types with the correspoinding nn enums
s32 getBlendFunc(const nn::g3d::MaterialObj*, bool, bool);
s32 getBlendEquation(const nn::g3d::MaterialObj*, bool);
void getConstantColor(sead::Color4f*, const nn::g3d::MaterialObj*);
bool getDepthTestEnable(const nn::g3d::MaterialObj*);
bool getDepthWriteEnable(const nn::g3d::MaterialObj*);
s32 getDepthCtrlFunc(const nn::g3d::MaterialObj*);
bool getAlphaTestEnable(const nn::g3d::MaterialObj*);
void getAlphaTestValue(const nn::g3d::MaterialObj*);
s32 getAlphaTestFunc(const nn::g3d::MaterialObj*);
s32 getCullingMode(const nn::g3d::MaterialObj*);
void setPolygonOffsetToContext(agl::DrawContext*, sead::GraphicsContext*,
                               const nn::g3d::MaterialObj*, float);
void setPolygonCtrlToContext(sead::GraphicsContext*, const nn::g3d::MaterialObj*);
void setDepthCtrlToContext(sead::GraphicsContext*, const nn::g3d::MaterialObj*);
void setBlendCtrlToContext(sead::GraphicsContext*, const nn::g3d::MaterialObj*);
void setAlphaTestToContext(sead::GraphicsContext*, const nn::g3d::MaterialObj*);
void setPolygonOffsetToContext(agl::DrawContext*, sead::GraphicsContextMRT*,
                               const nn::g3d::MaterialObj*, float);
void setPolygonCtrlToContext(sead::GraphicsContextMRT*, const nn::g3d::MaterialObj*);
void setDepthCtrlToContext(sead::GraphicsContextMRT*, const nn::g3d::MaterialObj*);
void setBlendCtrlToContext(sead::GraphicsContextMRT*, const nn::g3d::MaterialObj*);
void setAlphaTestToContext(sead::GraphicsContextMRT*, const nn::g3d::MaterialObj*);
void copyRenderBuffer(agl::DrawContext*, const agl::RenderBuffer&, const agl::RenderBuffer&);
void setContextMRT(sead::GraphicsContextMRT*);
void setContextMRTBlendBcLbuf(sead::GraphicsContextMRT*, bool);
void setContextMRTAddBcLbuf(sead::GraphicsContextMRT*, bool);
void setContextMRTBlendBcNrmLbuf(sead::GraphicsContextMRT*, bool);
void setContextMRTBlendAll(sead::GraphicsContextMRT*, bool);
void setContextMRTBlendLbuf(sead::GraphicsContextMRT*, bool);
void setContextMRTAddLbuf(sead::GraphicsContextMRT*, bool);
void setContextMRTMulAddLbuf(sead::GraphicsContextMRT*, bool);
void setContextMRTMulLbuf(sead::GraphicsContextMRT*, bool);
void setContextMRTMulBc(sead::GraphicsContextMRT*, bool);
void setContextMRTFootPrint(sead::GraphicsContextMRT*);
void setContextMRTAlphaMask(sead::GraphicsContextMRT*);
void setContextMRTMiiFaceXlu(sead::GraphicsContextMRT*);
void setContextMRTOnlyDepth(sead::GraphicsContextMRT*);
void setContextMRTSilhouette(sead::GraphicsContextMRT*);
}  // namespace al

namespace alGraphicsFunction {
void registerGraphicsPreset(const char*, const char*, const char*, const al::ActorInitInfo&, bool);
void requestGraphicsPreset(const al::LiveActor*, const char*, s32, s32, s32, const sead::Vector3f&);
void appearFootPrint(const al::LiveActor*, al::FootPrintRequestInfo*);
void disappearFootPrint(const al::LiveActor*, al::FootPrintRequestInfo*);
void setRainControl(const al::LiveActor*, float);
void setRainUpdateFlag(al::Scene* scene, bool);
bool isEnableRain(al::Scene* scene);
void setFogNoiseUpdateFlag(al::Scene* scene, bool);
const sead::Vector3f& getDirectionalLightDir(const al::LiveActor*);
const sead::Vector3f& calcDirectionalLightDir(const al::LiveActor*);
f32 getDepthShadowFarLength(const al::LiveActor*);
void invalidateCameraBlurSystem(al::Scene* scene);
void validateCameraBlurSystem(al::Scene* scene);
void invalidateOcclusionCullingSystem(al::Scene* scene);
void validateOcclusionCullingSystem(al::Scene* scene);
bool isGraphicsQualityModeConsole(const al::LiveActor*);
bool isGraphicsQualityModeHandheld(const al::LiveActor*);
void forceGraphicsQualityModeConsole(al::Scene* scene);
void forceGraphicsQualityModeHandheld(al::Scene* scene);
void forceGraphicsQualityModeSnapShot(al::Scene* scene);
void unforceGraphicsQualityMode(al::Scene* scene);
void validateModelLodAll(al::Scene* scene);
void invalidateModelLodAll(al::Scene* scene);
void setSphereMapUpdateEveryFrame(al::Scene* scene, const sead::Vector3f&);
void setMaterialLightUpdateBaseAngleChanged(al::Scene* scene, bool);
void requestChangeShaderVariation(const al::LiveActor*, const char*, const char*, bool);
void requestChangeShaderVariation(al::ModelCtrl*, s32, al::ModelShaderHolder*, const char*,
                                  const char*, bool);
void requestChangeShaderVariation(al::ModelCtrl*, al::ModelShaderHolder*, const char*, const char*,
                                  bool);
void requestChangeShaderVariationWithoutForward(const al::LiveActor*, const char*, const char*,
                                                bool);
void requestUpdateMaterialInfo(al::Scene* scene);
void validateGpuStressAnalyzer(al::Scene* scene);
void invalidateGpuStressAnalyzer(al::Scene* scene);
bool isValidGpuStressAnalyzer(al::Scene* scene);
NVNmemoryPool* getMemoryPoolPtr(
    const nn::gfx::TBuffer<nn::gfx::ApiVariation<nn::gfx::ApiType<4>, nn::gfx::ApiVersion<8>>>&);
u64 getMemoryPoolSize(const NVNmemoryPool*);
u64 calcMemoryPoolOffset(
    const nn::gfx::TBuffer<nn::gfx::ApiVariation<nn::gfx::ApiType<4>, nn::gfx::ApiVersion<8>>>&);
u64 getGPUAddressMemoryPool(NVNmemoryPool*);
u64 getGPUAddress(
    const nn::gfx::TBuffer<nn::gfx::ApiVariation<nn::gfx::ApiType<4>, nn::gfx::ApiVersion<8>>>&);
u64 getGPUAddressMemoryPool(
    const nn::gfx::TBuffer<nn::gfx::ApiVariation<nn::gfx::ApiType<4>, nn::gfx::ApiVersion<8>>>&);
u64 getGPUAddress(const agl::GPUMemVoidAddr&);
u64 getGPUAddress(const agl::ConstGPUMemVoidAddr&);
u64 getGPUAddress(const NVNbuffer&);
u64 calcVertexBufferGPUAddress(const nn::g3d::ResVertex&, s32);
void calcIndexBufferGPUAddress(nn::g3d::ResMesh&);
}  // namespace alGraphicsFunction

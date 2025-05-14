#pragma once

#include <basis/seadTypes.h>

namespace sead {
class Camera;
}  // namespace sead

namespace agl {
class DrawContext;
class TextureData;
class RenderBuffer;
}  // namespace agl

namespace al {
class NoiseTextureKeeper;
class Projection;
class ShaderHolder;
class DepthOfFieldDrawer;
class GraphicsParamRequesterImpl;
class ViewDepthDrawer;
class EdgeDrawer;
class CartoonDrawer;
class RetroColorDrawer;
class ScreenBlurDrawer;
class PencilSketchDrawer;
class ColorClampDrawer;

class PostProcessingFilter {
public:
    PostProcessingFilter(al::ShaderHolder*, al::NoiseTextureKeeper*);

    void incrementPreset();
    void decrementPreset();
    void initProjectResource();
    void endInit();
    void clearRequest();
    void update();
    void findPreset(s32) const;
    void updateViewGpu(s32, sead::Camera const*, al::Projection const*);
    void drawFilter(agl::DrawContext*, s32, al::SimpleModelEnv*, agl::RenderBuffer const&,
                    agl::TextureData const&, agl::TextureData const&, agl::TextureData const&,
                    sead::Camera const&, al::Projection const&, f32, f32, f32);

    void validate() { mIsValid = true; }

    void invalidate() { mIsValid = false; }

    s32 getPresetId() { return mPresetId; }

private:
    bool mIsValid;
    ShaderHolder* ShaderHolder;
    DepthOfFieldDrawer* DepthOfFieldDrawer;
    GraphicsParamRequesterImpl* mGraphicsParamRequesterImpl;
    ViewDepthDrawer* mViewDepthDrawer;
    VignettingDrawer* mVignettingDrawer;
    EdgeDrawer* mEdgeDrawer;
    CartoonDrawer* mCartoonDrawer;
    RetroColorDrawer* mRetroColorDrawer;
    ScreenBlurDrawer* mScreenBlurDrawer;
    PencilSketchDrawer* mPencilSketchDrawer;
    ColorClampDrawer* mColorClampDrawer;
    int _60;
    void** _68;
    s32 mPresetId;
};

}  // namespace al

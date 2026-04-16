#pragma once

#include <basis/seadTypes.h>

#include "Library/Scene/ISceneObj.h"

namespace agl {
class DrawContext;
class SamplerLocation;
class TextureData;
}  // namespace agl

class PlayerHackStartTexKeeper : public al::ISceneObj {
public:
    void clearHackStartTextureOnlyFirstTime(agl::DrawContext* drawContext);
    void activateHackStartTexture(agl::DrawContext* drawContext,
                                  const agl::SamplerLocation& location) const;

    const agl::TextureData* getTextureData() { return mTextureData; }

    bool isEnabled() { return mIsEnabled; }
    void disable() { mIsEnabled = false; }

private:
    void* _0[2];
    bool _18;
    bool mIsEnabled;
    const agl::TextureData* mTextureData;
};

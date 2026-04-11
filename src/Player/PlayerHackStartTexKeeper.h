#pragma once

#include <basis/seadTypes.h>

#include "Library/Scene/ISceneObj.h"

namespace agl {
class DrawContext;
class SamplerLocation;
}  // namespace agl

class PlayerHackStartTexKeeper : public al::ISceneObj {
public:
    void clearHackStartTextureOnlyFirstTime(agl::DrawContext* drawContext);
    void activateHackStartTexture(agl::DrawContext* drawContext,
                                  const agl::SamplerLocation& location) const;

    void* _0[2];
    bool _18;
    bool _19;
    void* _20;
};

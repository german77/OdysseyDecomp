#pragma once

#include "Library/Scene/ISceneObj.h"

namespace agl {
class DrawContext;
}

class MoviePlayer : public al::ISceneObj {
public:
    void draw(agl::DrawContext* drawContext) const;
};

#pragma once

#include "Library/Camera/CameraPoser.h"

namespace al {

struct CameraPoserTalkParam {
    f32 angleH = 60.0f;
    f32 angleV = 5.0f;
    f32 minDistance = 600.0f;
    f32 offsetY = 90.0f;
    bool isKeepPreDir = false;
};

static_assert(sizeof(CameraPoserTalkParam) == 0x14);

class CameraPoserTalk : public CameraPoser {
public:
    CameraPoserTalk(const char* name);

    void init() override;
    void loadParam(const ByamlIter& iter) override;
    void start(const CameraStartInfo& info) override;
    void setMinDistance(f32);

private:
    CameraPoserTalkParam* param;
};

 static_assert(sizeof(CameraPoserTalk) == 0x148);

}  // namespace al

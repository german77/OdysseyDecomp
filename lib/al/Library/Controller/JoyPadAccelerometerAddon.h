#pragma once

#include <basis/seadTypes.h>
#include <controller/seadControllerAddon.h>
#include <math/seadVector.h>

namespace al {
class JoyPadAccelerometerAddon : public sead::ControllerAddon {
    SEAD_RTTI_OVERRIDE(JoyPadAccelerometerAddon, ControllerAddon)
public:
    JoyPadAccelerometerAddon(sead::Controller* controller, s32 index);

    bool calc() override;

private:
    bool mIsValid=false;
    sead::Vector3f mAcceleration = {0.0f,0.0,0.0f};
    s32 mIndex;
};

static_assert(sizeof(JoyPadAccelerometerAddon) == 0x40);
}  // namespace al

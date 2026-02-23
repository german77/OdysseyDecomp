#pragma once

#include <basis/seadTypes.h>
#include <controller/seadController.h>
#include <nn/hid.h>

namespace nn::hid {
struct VibrationDeviceHandle;
}

namespace al {
class NpadController : public sead::Controller {
    SEAD_RTTI_OVERRIDE(NpadController, Controller)
public:
    NpadController(sead::ControllerMgr* mgr);

    bool isConnected() const override;
    bool isValidNpadId() const;
    s32 getNpadId() const;
    void setAnyControllerMode();
    void setIndexControllerMode(s32 modeIndex);
    const nn::hid::VibrationDeviceHandle& getVibrationDeviceHandle(s32 index) const;

    nn::hid::NpadStyleTag getStyleIndex() const { return mNpadStyleIndex; }

    s64 getSixAxisSensorCount() const { return mCount; }

private:
    void calcImpl_() override;

    s32 mControllerModeIndex = -1;
    s32 mNpadId = -1;
    nn::hid::NpadStyleTag mNpadStyleIndex = nn::hid::NpadStyleTag::NpadStyleInvalid;
    s32 mCount = 0;
    s32 e = 0;
    bool mIsConnected = false;
    void* f = nullptr;
};

static_assert(sizeof(NpadController) == 0x198);
}  // namespace al

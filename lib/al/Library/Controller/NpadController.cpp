#include "Library/Controller/NpadController.h"

#include <controller/nin/seadNinJoyNpadDevice.h>
#include <controller/seadControllerMgr.h>

namespace al {
NpadController::NpadController(sead::ControllerMgr* mgr) : sead::Controller(mgr) {
    mId = sead::ControllerDefine::ControllerId::cController_Npad;
    setLeftStickCrossThreshold(0.9f, 0.7f);
    setRightStickCrossThreshold(0.9f, 0.7f);
}

bool NpadController::isConnected() const {
    return isValidNpadId() && mIsConnected;
}

bool NpadController::isValidNpadId() const {
    return mNpadId != -1;
}

s32 NpadController::getNpadId() const {
    return mNpadId;
}

void NpadController::setAnyControllerMode() {
    mControllerModeIndex = -1;
}

void NpadController::setIndexControllerMode(s32 modeIndex) {
    mControllerModeIndex = modeIndex;
}

const nn::hid::VibrationDeviceHandle& NpadController::getVibrationDeviceHandle(s32 index) const {
    sead::ControllerMgr* mgr = sead::ControllerMgr::instance();
    sead::NinJoyNpadDevice* npadDevice = mgr->getControlDeviceAs<sead::NinJoyNpadDevice*>();
    return npadDevice->getNpadState(mNpadId).mVibrationDeviceHandles[index];
}

void NpadController::calcImpl_() {
    sead::ControllerMgr* mgr = sead::ControllerMgr::instance();
    sead::NinJoyNpadDevice* npadDevice = mgr->getControlDeviceAs<sead::NinJoyNpadDevice*>();

    if (!npadDevice) {
        mNpadId = -1;
        mCount = 0;
        mE = 0;
        return;
    }

    int selectedId = -1;
    if (mControllerModeIndex < 0) {
        for (int i = 0; i < 9; ++i) {
            if (npadDevice->getNpadStyleIndex(i) != nn::hid::NpadStyleTag::NpadStyleInvalid) {
                selectedId = i;
                break;
            }
        }
    } else {
        selectedId = mControllerModeIndex;
    }

    mNpadId = selectedId;

    mCount = 0;
    mE = 0;
    mPadHold.makeAllZero();
    mNpadId = selectedId;
    mNpadStyleIndex = nn::hid::NpadStyleTag::NpadStyleInvalid;
    mLeftStick.set(0.0f, 0.0f);
    mRightStick.set(0.0f, 0.0f);

    if (mNpadId == -1) {
        mF = 0;
        mG = 0;
        return;
    }

    // Retrieve state from sead::NinJoyNpadDevice
    const auto& npadState = npadDevice->getNpadState(mNpadId);
    const auto& state = npadState.mStates[0];

    mNpadStyleIndex = npadDevice->getNpadStyleIndex(mNpadId);
    mIsConnected = true;  // Simplified; actual check is at offset 0x328/0x388

    const nn::hid::NpadButtonSet sdkButtons = state.mButtons;
    // u32 seadButtons = (sdkButtons & 0x3); // A, B
    // if (sdkButtons & (1 << 2)) seadButtons |= (1 << 3); // X
    // if (sdkButtons & (1 << 3)) seadButtons |= (1 << 4); // Y
    //  ... continues for L, R, ZL, ZR, etc.
    // mPadHold.setDirect(seadButtons);

    // Stick Normalization (Assembly 0x470 - 0x510)
    auto normalize = [](s32 x, s32 y, sead::Vector2f* out) {
        float fx = (float)x / 32767.0f;
        float fy = (float)y / 32767.0f;
        float magSq = fx * fx + fy * fy;
        if (magSq > 1.0f) {
            float mag = sqrtf(magSq);
            fx /= mag;
            fy /= mag;
        }
        out->x = fx;
        out->y = fy;
    };

    normalize(state.mAnalogStickL.mX, state.mAnalogStickL.mY, &mLeftStick);
    normalize(state.mAnalogStickR.mX, state.mAnalogStickR.mY, &mRightStick);
}

}  // namespace al

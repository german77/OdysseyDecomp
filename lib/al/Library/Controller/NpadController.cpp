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
}

}  // namespace al

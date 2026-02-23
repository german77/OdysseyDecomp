#include "Library/Controller/PadGyroAddon.h"

#include <controller/nin/seadNinJoyNpadDevice.h>
#include <controller/seadControllerMgr.h>

#include "Library/Controller/NpadController.h"

namespace al {
PadGyroAddon::PadGyroAddon(sead::Controller* controller, s32 index)
    : sead::ControllerAddon(controller), mIndex(index) {
    mId = sead::ControllerDefine::AddonId::cAddon_Gyro;
}

bool PadGyroAddon::calc() {
    mGyroStatus = tryUpdateGyroStatus();
    return false;
}

bool PadGyroAddon::tryUpdateGyroStatus() {
    sead::ControllerMgr* mgr = sead::ControllerMgr::instance();
    sead::NinJoyNpadDevice* npadDevice =mgr->getControlDeviceAs<sead::NinJoyNpadDevice*>();
    NpadController* npad = (NpadController*)mController;

    if (!npad->isConnected())
        return false;

    s64 index=mIndex;
    if (index >= npad->getSixAxisSensorCount())
        return false;

    const sead::NinJoyNpadDevice::NpadState& state = npadDevice->getNpadState(npad->getNpadId());
    const nn::hid::SixAxisSensorState& sixaxisState = state.mSixAxisSensorStates[index];

    s64 sampleNumber=sixaxisState.mSamplingNumber;
    s32 samples = sampleNumber - mPrevSamplingNumber;
    if (samples > 15)
        samples = 16;
    if (samples < 0)
        samples = 0;

    mSampleCount = samples;
    mPrevSamplingNumber = sampleNumber;;

    mAngularVelocity.set(-sixaxisState.mAngularVelocity[0], sixaxisState.mAngularVelocity[2], sixaxisState.mAngularVelocity[1]);
    mAngle.set(-sixaxisState.mAngle[0], sixaxisState.mAngle[2], sixaxisState.mAngle[1]);

    const auto& m = sixaxisState.mDirection.mMtx;
    mSide.set(m[0][0], -m[0][1], -m[0][2]);
    mUp.set(-m[1][0], m[1][2], m[1][1]);
    mFront.set(-m[2][0], m[2][2], m[2][1]);

    if (npadDevice->getNpadJoyHoldType() == nn::hid::NpadJoyHoldType::Horizontal) {
        nn::hid::NpadStyleTag style = npad->getStyleIndex();
        if (style == nn::hid::NpadStyleTag::NpadStyleJoyLeft ||
            style == nn::hid::NpadStyleTag::NpadStyleJoyRight) {
            bool isRight = (style == nn::hid::NpadStyleTag::NpadStyleJoyRight);

    f32 vX = sixaxisState.mAngularVelocity[0];
    f32 vY = sixaxisState.mAngularVelocity[1];
    f32 vZ = sixaxisState.mAngularVelocity[2];
            mAngularVelocity.set(isRight ? vZ : -vZ, isRight ? -vX : vX, vY);

            f32 pAx = mSide.x, pAy = mSide.y, pAz = mSide.z;
            f32 pBx = mUp.x, pBy = mUp.y, pBz = mUp.z;
            f32 pCx = mFront.x, pCy = mFront.y, pCz = mFront.z;

            if (isRight) {
                mSide.set(pAy, -pAx, pAz);
                mUp.set(-pBy, pBx, -pBz);
                mFront.set(pCy, -pCx, pCz);
            } else {
                mSide.set(-pAy, pAx, -pAz);
                mUp.set(pBy, -pBx, pBz);
                mFront.set(-pCy, pCx, -pCz);
            }
        }
    }
    return true;
}

void PadGyroAddon::getPose(sead::Vector3f* outSide, sead::Vector3f* outUp,
                           sead::Vector3f* outFront) const {
    if (outSide)
        outSide->set(mSide);
    if (outUp)
        outUp->set(mUp);
    if (outFront)
        outFront->set(mFront);
}

}  // namespace al

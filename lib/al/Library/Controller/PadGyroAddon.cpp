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
    sead::NinJoyNpadDevice* npadDevice = mgr->getControlDeviceAs<sead::NinJoyNpadDevice*>();
    NpadController* npad = (NpadController*)mController;

    if (!npad->isConnected())
        return false;

    s64 index = mIndex;
    if (index >= npad->getSixAxisSensorCount())
        return false;

    const sead::NinJoyNpadDevice::NpadState& state = npadDevice->getNpadState(npad->getNpadId());
    const nn::hid::SixAxisSensorState& sixaxisState = state.mSixAxisSensorStates[index].state[0];

    s64 sampleNumber = sixaxisState.mSamplingNumber;
    s32 samples = sampleNumber - mPrevSamplingNumber;
    if (samples > 15)
        samples = 16;
    if (samples < 0)
        samples = 0;

    mSampleCount = samples;
    mPrevSamplingNumber = sampleNumber;

    f32 angVx = sixaxisState.mAngularVelocity[0];
    f32 angVy = sixaxisState.mAngularVelocity[1];
    mAngularVelocity.y = sixaxisState.mAngularVelocity[2];
    mAngularVelocity.z = angVy;
    f32 nangVx = -angVx;
    mAngularVelocity.x = nangVx;

    f32 angx = sixaxisState.mAngle[0];
    f32 angy = sixaxisState.mAngle[1];
    mAngle.y = sixaxisState.mAngle[2];
    mAngle.z = angy;
    f32 nangx = -angx;
    mAngle.x = nangx;

    f32 xx = sixaxisState.mDirection.mMtx[0][0];
    f32 xy = sixaxisState.mDirection.mMtx[0][1];
    f32 xz = sixaxisState.mDirection.mMtx[0][2];
    mSide.x = xx;
    mSide.y = -xz;
    mSide.z = -xy;

    f32 yx = sixaxisState.mDirection.mMtx[2][0];
    f32 fVar9 = -yx;
    f32 yy = sixaxisState.mDirection.mMtx[2][1];
    f32 yz = sixaxisState.mDirection.mMtx[2][2];
    mUp.x = fVar9;
    mUp.y = yz;
    mUp.z = yy;

    f32 fVar11 = sixaxisState.mDirection.mMtx[1][0];
    f32 fVar12 = -fVar11;
    f32 fVar10 = sixaxisState.mDirection.mMtx[1][1];
    f32 fVar7 = sixaxisState.mDirection.mMtx[1][2];
    mFront.x = fVar12;
    mFront.y = fVar7;
    mFront.z = fVar10;

    if (npadDevice->getNpadJoyHoldType() == nn::hid::NpadJoyHoldType::Horizontal) {
        nn::hid::NpadStyleTag style = npad->getStyleIndex();
        if (style == nn::hid::NpadStyleTag::NpadStyleJoyLeft ||
            style == nn::hid::NpadStyleTag::NpadStyleJoyRight) {
            bool isRight = style != nn::hid::NpadStyleTag::NpadStyleJoyRight;

            if (isRight) {
                nangVx = angVx;
            }
            mAngularVelocity.z = nangVx;
            angVx = -angVy;
            nangVx = -angy;
            if (isRight) {
                angVx = angVy;
                nangVx = angy;
            }
            mAngle.x = nangVx;
            if (isRight) {
                nangx = angx;
            }
            mAngularVelocity.x = angVx;
            mAngle.z = nangx;

            if (isRight) {
                xx = -xx;
            }
            else {
                fVar7 = -fVar7;
                fVar10 = -fVar10;
                xz = -xz;
                xy = -xy;
                fVar12 = fVar11;
            }
            isRight = style != nn::hid::NpadStyleTag::NpadStyleJoyRight;
            if (isRight) {
                fVar9 = yx;
            }
            mUp.z = fVar9;
            if (isRight) {
                fVar12 = -fVar12;
            }
            angVy = -fVar10;
            if (isRight) {
                angVy = fVar10;
            }
            fVar10 = -yy;
            if (isRight) {
                fVar10 = yy;
            }
            angy = -xy;
            if (isRight) {
                angy = xy;
            }
            if (isRight) {
                xx = -xx;
            }
            mSide.y = fVar7;
            mUp.y = yz;
            mFront.y = xz;
            mSide.z = fVar12;
            mSide.x = angVy;
            mUp.x = fVar10;
            mFront.x = angy;
            mFront.z = xx;

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

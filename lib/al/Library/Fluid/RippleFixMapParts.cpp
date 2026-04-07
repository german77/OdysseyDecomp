#include "Library/Fluid/RippleFixMapParts.h"

#include <math/seadBoundBox.h>

#include "Library/Camera/CameraUtil.h"
#include "Library/Demo/DemoFunction.h"
#include "Library/Fluid/RippleCtrl.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorResourceFunction.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Matrix/MatrixUtil.h"
#include "Library/Nature/NatureUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Resource/ResourceFunction.h"
#include "Library/Yaml/ByamlIter.h"
#include "Library/Yaml/ParameterBase.h"

namespace al {
RippleFixMapParts::RippleFixMapParts(const char* name) : LiveActor(name) {}

void RippleFixMapParts::init(const ActorInitInfo& info) {
    const char* suffix = nullptr;
    tryGetStringArg(&suffix, info, "Suffix");
    initMapPartsActor(this, info, suffix);

    tryGetArg(&mIsAddRippleByCamera, info, "IsAddRippleByCamera");
    mRippleCtrl = RippleCtrl::tryCreate(this);
    if (!mRippleCtrl) {
        kill();
        return;
    }

    mRippleCtrl->init(info);
    if (isExistResourceYaml(getModelResource(this), "InitSinkParam", nullptr)) {
        mParameters = new Parameters;
        mParameters->parameterIo = new ParameterIo;
        mParameters->parameterObj = new ParameterObj;
        ParameterObj* obj = mParameters->parameterObj;
        mParameters->sinkStartDist =
            new ParameterF32(2000.0f, "沈み始める距離", "SinkStartDist", "Min=0.f, Max=7000.f",
                             mParameters->parameterObj, true);
        mParameters->sinkEndDist =
            new ParameterF32(4000.0f, "沈み終える距離", "SinkEndDist", "Min=0.f, Max=10000.f",
                             mParameters->parameterObj, true);
        mParameters->sinkDistance =
            new ParameterV3f({0.0f, -100.0f, 0.0f}, "沈み距離", "SinkDistance",
                             "Min=-1000.f,Max=1000.f", mParameters->parameterObj, true);

        mParameters->parameterIo->addObj(mParameters->parameterObj, "SinkParams");

        ByamlIter iter;
        if (tryGetActorInitFileIter(&iter, getModelResource(this), "InitSinkParam", nullptr)) {
            initParameterIoAsActorInfo(mParameters->parameterIo, this, "InitSinkParam", nullptr);
            mParameters->parameterIo->tryGetParam(iter);
        }
    }

    if (getModelKeeper() && !isExistAction(this) && !isViewDependentModel(this) && !mParameters)
        mIsCalcViewModel = true;

    registActorToDemoInfo(this, info);
    mTrans.set(getTrans(this));
    trySyncStageSwitchAppearAndKill(this);
}

void RippleFixMapParts::initAfterPlacement() {
    if (mRippleCtrl)
        mRippleCtrl->initAfterPlacement();
}

void RippleFixMapParts::appear() {
    LiveActor::appear();
    tryStartAction(this, "Appear");
}

void RippleFixMapParts::control() {
    if (mIsAddRippleByCamera) {
        sead::Vector3f cameraPos = getCameraPos(this, 0);
        tryAddRippleSmall(this, cameraPos);
    }

    if (mParameters) {
        f32 sinkStartDist = mParameters->sinkStartDist->getValue();
        f32 sinkEndDist = mParameters->sinkEndDist->getValue();
        sead::Vector3f sinkDistance = mParameters->sinkDistance->getValue();

        f32 distance;
        const sead::Vector3f& trans = mTrans;
        sead::Vector3f cameraPos = getCameraPos(this, 0);
        {
            sead::BoundBox3f modelBoundBox;
            sead::Matrix34f mtx;
            calcModelBoundingBox(&modelBoundBox, this);

            sead::Vector3f frontDir;
            calcFrontDir(&frontDir, this);

            sead::Vector3f upDir;
            calcUpDir(&upDir, this);

            cameraPos.y = trans.y + modelBoundBox.getMin().y;

            sead::Quatf quat;
            makeQuatFrontUp(&quat, frontDir, upDir);
            makeMtxQuatPos(&mtx, quat, trans);

            distance = calcDistanceToObb(cameraPos, mtx, {1.0f, 1.0f, 1.0f}, modelBoundBox);
        }

        if (distance > sinkEndDist) {
            setTrans(this, sinkDistance + mTrans);
            hideModelIfShow(this);
        } else if (distance > sinkStartDist) {
            f32 rate = sead::Mathf::clamp(
                (distance - sinkStartDist) / (sinkEndDist - sinkStartDist), 0.0f, 1.0f);
            setTrans(this, sinkDistance * rate + mTrans);
            showModelIfHide(this);
        } else {
            showModelIfHide(this);
            setTrans(this, mTrans);
        }
    }

    if (mRippleCtrl && !isHideModel(this)) {
        if (alDemoUtilTmp::isActiveDemo(this))
            mRippleCtrl->forceResetCount();
        mRippleCtrl->update();
    }
}

void RippleFixMapParts::movement() {
    LiveActor::movement();
}

void RippleFixMapParts::calcAnim() {
    if (mIsCalcViewModel) {
        calcViewModel(this);
        return;
    }

    LiveActor::calcAnim();
}

bool RippleFixMapParts::receiveMsg(const SensorMsg* message, HitSensor* other, HitSensor* self) {
    if (isMsgAskSafetyPoint(message))
        return true;

    if (isMsgShowModel(message)) {
        showModelIfHide(this);
        return true;
    }

    if (isMsgHideModel(message)) {
        hideModelIfShow(this);
        return true;
    }
    return false;
}

}  // namespace al

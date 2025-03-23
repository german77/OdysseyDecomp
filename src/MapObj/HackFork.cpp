#include "MapObj/HackFork.h"

#include "Library/Camera/CameraTicket.h"
#include "Library/Camera/CameraUtil.h"
#include "Library/Collision/PartsConnector.h"
#include "Library/Controller/PadRumbleFunction.h"
#include "Library/Event/EventFlowUtil.h"
#include "Library/Joint/JointControllerKeeper.h"
#include "Library/Joint/JointLocalAxisRotator.h"
#include "Library/Layout/LayoutActorUtil.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorAnimFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorResourceFunction.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Play/Camera/ActorMatrixCameraTarget.h"
#include "Library/Se/SeFunction.h"
#include "Library/Yaml/ByamlUtil.h"

#include "Player/CapTargetInfo.h"
#include "Player/IUsePlayerHack.h"
#include "Player/PlayerHackStartShaderCtrl.h"
#include "Scene/GuidePosInfoHolder.h"
#include "System/GameDataUtil.h"
#include "Util/Hack.h"
#include "Util/NpcAnimUtil.h"
#include "Util/NpcEventFlowUtil.h"
#include "Util/PlayerUtil.h"
#include "Util/SensorMsgFunction.h"

namespace {
NERVE_IMPL(HackFork, Wait);
NERVE_IMPL(HackFork, HackStartWait);
NERVE_IMPL(HackFork, Damping);
NERVE_IMPL(HackFork, HackStart);
NERVE_IMPL(HackFork, HackWait);
NERVE_IMPL(HackFork, HackBend);
NERVE_IMPL(HackFork, HackShoot);

NERVES_MAKE_STRUCT(HackFork, Wait, HackStartWait, Damping, HackStart, HackWait, HackBend,
                   HackShoot);
}  // namespace

PlayerHackStartShaderParam sPlayerHackStartShaderParam(true, 100.0f, 10, 20);

HackFork::HackFork(const char* name) : al::LiveActor(name) {}

void HackFork::init(const al::ActorInitInfo& initInfo) {
    const char* modelName = nullptr;
    if (alPlacementFunction::tryGetModelName(&modelName, initInfo))
        al::initActorWithArchiveName(this, initInfo, modelName, nullptr);
    else
        al::initActor(this, initInfo);

    al::calcSideDir(&leJump, this);
    al::initJointControllerKeeper(this, 10);
    ptrArray.allocBuffer(5, nullptr);

    ptrArray.pushBack(al::initJointLocalAxisRotator(this, leJump, &damping, "Stick01", false));
    al::initJointLocalXRotator(this, &damping2, "Stick01");
    ptrArray.pushBack(al::initJointLocalAxisRotator(this, leJump, &damping, "Stick02", false));
    al::initJointLocalXRotator(this, &damping2, "Stick02");
    ptrArray.pushBack(al::initJointLocalAxisRotator(this, leJump, &damping, "Stick03", false));
    al::initJointLocalXRotator(this, &damping2, "Stick03");
    ptrArray.pushBack(al::initJointLocalAxisRotator(this, leJump, &damping, "Stick04", false));
    al::initJointLocalXRotator(this, &damping2, "Stick04");
    ptrArray.pushBack(al::initJointLocalAxisRotator(this, leJump, &damping, "Stick05", false));
    al::initJointLocalXRotator(this, &damping2, "Stick05");

    al::initNerve(this, &NrvHackFork.Wait, 0);
    al::tryGetArg(&isLongJump, initInfo, "LimitterFree");
    bool isCamera = false;
    bool isArg = al::tryGetArg(&isCamera, initInfo, "Camera");
    if (isCamera && isArg)
        mCameraTicket = al::initObjectCamera(this, initInfo, nullptr, nullptr);

    mMatrixCameraTarget = al::createActorMatrixCameraTarget(this, &matrix5);
    if (!al::isEqualString(modelName, "HackBoard")) {
        zeControl = {0.0f, 0.0f, 100.0f};
        zeMessage = true;
    }

    bool isballon = false;
    bool isBArg = al::tryGetArg(&isballon, initInfo, "Balloon");
    if (isballon && isBArg && !rs::isSequenceTimeBalloonOrRace(this)) {
        mEventFlowExecutor = rs::initEventFlow(this, initInfo, nullptr, nullptr);
        rs::startEventFlow(mEventFlowExecutor, "Init");
    }
    if (al::isMtpAnimExist(this)) {
        rs::setNpcMaterialAnimFromPlacementInfo(this, initInfo);
        al::tryStartMclAnimIfExist(this, al::getPlayingMtpAnimName(this));
    }
    mMtxConnector = al::tryCreateMtxConnector(this, initInfo);
    makeActorAlive();
    mCapTargetInfo = rs::createCapTargetInfo(this, nullptr);

    al::ByamlIter iter(al::getModelResourceYaml(this, "InitHackCap", nullptr));
    mJointName = al::tryGetByamlKeyStringOrNULL(iter, "JointName");
    sead::Vector3f localTrans = {0.0f, 0.0f, 0.0f};
    al::tryGetByamlV3f(&localTrans, iter, "LocalTrans");
    sead::Vector3f localRotate = {0.0f, 0.0f, 0.0f};
    al::tryGetByamlV3f(&localRotate, iter, "LocalRotate");

    sead::Matrix34f tmp;
    tmp.makeR(localRotate);

    localTrans *= al::getScaleY(this);

    matrix3.makeSR(localTrans, {0.0f, 0.0f, 0.0f});
    matrix3 = tmp * matrix3;

    sead::Matrix34f jointMtx = *al::getJointMtxPtr(this, mJointName);
    al::normalize(&jointMtx);
    matrix2.setInverse(jointMtx);

    mCapTargetInfo->setMatrix18(matrix1);
    quat3.inverse(&quat2);

    initBasicPoseInfo();
    mHackStartShaderCtrl = new PlayerHackStartShaderCtrl(this, &sPlayerHackStartShaderParam);
}

void HackFork::attackSensor(al::HitSensor* self, al::HitSensor* other) {
    if (al::isSensorName(self, "Push") && !al::sendMsgPush(other, self) && !isSensor) {
        const sead::Vector3f& velocity = al::getActorVelocity(other);
        if (velocity.x * velocity.x + velocity.z * velocity.z < 4.999696f)
            rs::sendMsgPushToPlayer(other, self);
    }
}

bool HackFork::receiveMsg(const al::SensorMsg* message, al::HitSensor* other, al::HitSensor* self) {
    if (rs::isMsgEnableMapCheckPointWarp(message))
        return false;

    if (rs::isMsgMotorcycleDashAttack(message)) {
        if (delay != 0) {
            delay = 30;
            return false;
        }
        if (al::isNerve(this, &NrvHackFork.Wait) || al::isNerve(this, &NrvHackFork.Damping)) {
            touchForce = 5.0f;
            delay = 30;
            al::setNerve(this, &NrvHackFork.Damping);
            al::startHitReaction(this, "タッチ（強）");
            return true;
        }
        return false;
    }
    if (al::isMsgPlayerObjTouch(message) || al::isMsgKickStoneAttackReflect(message) ||
        rs::isMsgRadishReflect(message) || rs::isMsgSeedReflect(message)) {
        if (delay != 0) {
            delay = 30;
            return false;
        }
        if (al::isNerve(this, &NrvHackFork.Wait) || al::isNerve(this, &NrvHackFork.Damping)) {
            touchForce = 2.0f;
            delay = 30;
            al::setNerve(this, &NrvHackFork.Damping);
            al::startHitReaction(this, "タッチ（弱）");
            return true;
        }
        return false;
    }
    if (rs::isMsgHammerBrosHammerHackAttack(message) || al::isMsgPlayerFireBallAttack(message)) {
        if (delay == 0) {
            if (al::isNerve(this, &NrvHackFork.Wait) || al::isNerve(this, &NrvHackFork.Damping)) {
                touchForce = 2.0f;
                delay = 30;
                al::setNerve(this, &NrvHackFork.Damping);
                al::startHitReaction(this, "タッチ（弱）");
            }
        } else {
            delay = 30;
        }
        rs::requestHitReactionToAttacker(message, self, other);
        return true;
    }
    if (rs::tryReceiveMsgInitCapTargetAndSetCapTargetInfo(message, mCapTargetInfo)) {
        resetCapMtx(self);
        return true;
    }
    if (rs::isMsgPlayerDisregardTargetMarker(message)) {
        if (mEventFlowExecutor != nullptr)
            return al::isActive(mEventFlowExecutor);
        return false;
    }
    if (rs::isMsgTargetMarkerPosition(message)) {
        sead::Vector3f position = al::getSensorPos(this, "Stick05") + 50.0f * sead::Vector3f::ey;
        rs::setMsgTargetMarkerPosition(message, position);
        return true;
    }
    if (al::isNerve(this, &NrvHackFork.Wait) || al::isNerve(this, &NrvHackFork.Damping)) {
        if (rs::isMsgCapEnableLockOn(message))
            return true;
        if (rs::isMsgStartHack(message)) {
            al::invalidateClipping(this);
            mPlayerHack = rs::startHack(self, other, nullptr);
            rs::startHackStartDemo(mPlayerHack, this);
            damping2 = 0.0f;
            damping = 0.0f;
            touchForce = 0.0f;
            resetCapMtx(self);
            rs::setRouteHeadGuidePosPtr(this, &leControl);
            al::setNerve(this, &NrvHackFork.HackStartWait);
            al::startHitReaction(this, "ひょうい開始");
            return true;
        }
        if (rs::isMsgCapCancelLockOn(message))
            return true;
    }
    if (isHack()) {
        if (rs::isMsgHackerDamageAndCancel(message)) {
            if (al::isSensorName(self, "Body"))
                return rs::requestDamage(mPlayerHack);
            return false;
        }
        if (rs::isMsgHackSyncDamageVisibility(message)) {
            rs::syncDamageVisibility(this, mPlayerHack);
            return true;
        }

        if (rs::receiveMsgRequestTransferHack(message, mPlayerHack, other))
            return true;
        if (rs::isMsgCancelHack(message)) {
            rs::tryEndHackStartDemo(mPlayerHack, this);
            if (isSensor) {
                rs::endHack(&mPlayerHack);
                rs::resetRouteHeadGuidePosPtr(this);
                al::tryStartAction(this, "HackEnd");
                al::setNerve(this, &NrvHackFork.Damping);
                return true;
            }

            if (zeMessage) {
                sead::Vector3f upDir;
                al::calcUpDir(&upDir, this);
                rs::endHackDir(&mPlayerHack, upDir);
            } else {
                sead::Vector3f sasa;
                if (al::isNearZero(newJump, 0.001f))
                    sasa.set(-hack);
                else
                    sasa.set(newJump);
                sasa.y = 0.0f;
                al::tryNormalizeOrDirZ(&sasa);
                sead::Quatf kiki;
                al::makeQuatUpFront(&kiki, sead::Vector3f::ey, sasa);
                rs::endHackTargetQuat(&mPlayerHack, kiki, sasa);
            }
            rs::resetRouteHeadGuidePosPtr(this);
            al::tryStartAction(this, "HackEnd");
            al::setNerve(this, &NrvHackFork.Damping);
            return true;
        }

        if (rs::isMsgHackMarioDemo(message) || rs::isMsgHackMarioDead(message)) {
            rs::endHack(&mPlayerHack);
            rs::resetRouteHeadGuidePosPtr(this);
            al::tryStartAction(this, "HackEnd");
            al::setNerve(this, &NrvHackFork.Damping);
            return true;
        }
    }
    return false;
}

void HackFork::initBasicPoseInfo() {
    al::calcUpDir(&upDir, this);

    quat.set(al::getQuat(this));

    sead::Vector3f frontDir;
    al::calcFrontDir(&frontDir, this);

    sead::Quatf tmpq;
    sead::QuatCalcCommon<f32>::setAxisAngle(tmpq, frontDir, 180.0f);
    quat3 = tmpq * quat;

    sead::Vector3f even;
    al::calcFrontDir(&even, this);
    if (sead::Mathf::abs(even.y) > 0.5f) {
        al::invalidateShadow(this);
        isSensor = false;
    } else {
        isSensor = true;
    }
}

void HackFork::initAfterPlacement() {
    if (mMtxConnector != nullptr) {
        sead::Vector3f frontDir;
        al::calcFrontDir(&frontDir, this);
        sead::Vector3f a = al::getTrans(this) + frontDir * 100.0f;
        frontDir = frontDir *= -200.0f;
        al::attachMtxConnectorToCollision(mMtxConnector, this, a, frontDir);
    }
}

bool HackFork::tryTouch(float force, char const* reaction) {
    if (delay != 0) {
        delay = 30;
        return false;
    }

    if (isNerveHackable()) {
        touchForce = force;
        delay = 30;
        al::setNerve(this, &NrvHackFork.Damping);
        al::startHitReaction(this, reaction);
        return true;
    }
    return false;
}

void HackFork::resetCapMtx(al::HitSensor* sensor) {
    calcHackDir(sensor);
    sead::Matrix34f mtx = *al::getJointMtxPtr(this, mJointName);
    al::normalize(&mtx);

    sead::Matrix34f mat;
    mat.setMul(mtx, matrix3);

    sead::Vector3f beto;
    sead::Vector3f vecy;
    if (isSensor)
        vecy.set({0.0f, 1.0f, 0.0f});
    else
        vecy = -hack;

    al::verticalizeVec(&vecy, beto, vecy);

    if (!al::tryNormalizeOrZero(&vecy)) {
        matrix4 = matrix3;
    } else {
        sead::Matrix34f nike;
        nike.setInverse(mat);
        sead::Quatf saul;
        al::makeQuatRotationRate(&saul, sead::Vector3f::ez, vecy, 1.0f);
        sead::Matrix34f pina;
        pina.setInverse(mtx);
        matrix4.setMul(mat, pina);
    }
    updateCapMtx();
}

bool HackFork::isNerveHackable() const {
    return al::isNerve(this, &NrvHackFork.Wait) || al::isNerve(this, &NrvHackFork.Damping);
}

bool HackFork::isHack() const {
    return al::isNerve(this, &NrvHackFork.HackStartWait) ||
           al::isNerve(this, &NrvHackFork.HackStart) || al::isNerve(this, &NrvHackFork.HackWait) ||
           al::isNerve(this, &NrvHackFork.HackBend) || al::isNerve(this, &NrvHackFork.HackShoot);
}

void HackFork::controlSpring() {
    f32 fVar2 = damping * 0.92f;
    touchForce += fVar2 * -0.5f;
    damping = fVar2 + touchForce;
}

void HackFork::checkSwing() {
    if (rs::isTriggerHackSwing(mPlayerHack)) {
        isHackSwing = true;
        hackDelay = 0;
    } else {
        hackDelay++;
    }
}

bool HackFork::trySwingJump() {
    if (rs::isTriggerHackSwing(mPlayerHack)) {
        isHackSwing = true;
        hackDelay = 0;
    } else {
        bool ish = isHackSwing;
        hackDelay++;
        if (!ish || 10 < hackDelay)
            return false;
    }

    if (isSensor) {
        newJump.set({0.0f, -1.0f, 0.0f});
    } else {
        sead::Vector3f tmpF = hack;
        tmpF.rotate(quat * quat2);
        tmpF.y = 0;
        al::tryNormalizeOrDirZ(&tmpF);
        newJump.set(tmpF);
    }
    isJump = newJump.dot(upDir) < 0.0f;

    damping = 22.5f;
    sead::Vector3f frontDir;
    al::calcFrontDir(&frontDir, this);
    bendAndTwist(newJump, frontDir);
    isShoot = true;
    al::setNerve(this, &NrvHackFork.HackShoot);
    return true;
}

bool HackFork::updateInput(sead::Vector3f* out, sead::Vector3f in) {
    sead::Vector3f incpy = in;
    sead::Vector3f lookdir;
    al::calcCameraLookDir(&lookdir, this, 0);

    sead::Mathf::cos(0.7853982f);
    if (in.dot(sead::Vector3f::ey) < 0.7071068f && 0.0f < in.dot(lookdir))
        incpy = -in;

    sead::Vector3f moveDir = {0.0f, 0.0f, 0.0f};
    bool isgood = rs::calcHackerMoveDir(&moveDir, mPlayerHack, incpy);
   
    inputBuffer.forcePushBack(moveDir);

    *out = {0.0f,0.0f,0.0f};
    
    for(s32 i=0;i<inputBuffer.size();i++){
        *out +=inputBuffer[i];
    }
    

    if (!al::tryNormalizeOrZero(out))
        al::calcUpDir(out, this);
    return isgood;
}

f32 HackFork::getJumpRange() const {
    return isLongJump ? 180.0f : 45.0f;
}

void HackFork::bendAndTwist(const sead::Vector3f& initInfo, const sead::Vector3f& param_2) {
    damping = sead::Mathf::clampMax(damping + 1.0f, 22.5f);
    touchForce = 0.0f;

    leJump.setCross(param_2, initInfo);
    if (!al::tryNormalizeOrZero(&leJump))
        al::calcSideDir(&leJump, this);
    for (s32 i = 0; i < ptrArray.size(); i++)
        ptrArray[i]->setVector28(leJump);
}

void HackFork::shoot() {
    sead::Vector3f frontDir;
    al::calcFrontDir(&frontDir, this);
    sead::Quatf quack;
    al::makeQuatUpFront(&quack, frontDir, newJump);
    if (quack.length() > 0)
        upDir = frontDir;
    rs::resetRouteHeadGuidePosPtr(this);
    al::startHitReaction(this, "ジャンプ");
    al::tryStartAction(this, "HackEnd");
}

void HackFork::control() {
    mHackStartShaderCtrl->update();
    if (mMtxConnector != nullptr) {
        al::connectPoseQT(this, mMtxConnector);
        initBasicPoseInfo();
    }
    if (!isHack()) {
        if (mCameraTicket != nullptr && al::isActiveCamera(mCameraTicket))
            al::endCamera(this, mCameraTicket, -1, false);
        if (al::isActiveCameraTarget(mMatrixCameraTarget))
            al::resetCameraTarget(this, mMatrixCameraTarget);
    }
    if (delay != 0)
        delay--;
    if (isHack()) {
        sead::Vector3f upDir;
        if (isSensor == false) {
            sead::Vector3f upDir;
            al::calcUpDir(&upDir, this);
            upDir.y = 0.0;
            al::tryNormalizeOrDirZ(&upDir);
            sead::Vector3f holt;
            holt.setCross(upDir, sead::Vector3f::ey);
            if (0.0 <= holt.dot(al::getCameraPos(this, 0) - al::getTrans(this))) {
                upDir = -upDir;
                holt = -holt;
            }
            matrix5.setBase(0, holt);
            matrix5.setBase(1, sead::Vector3f::ey);
            matrix5.setBase(2, upDir);
        } else {
            al::calcFrontDir(&upDir, this);
            upDir.y = 0.0f;
            al::tryNormalizeOrDirZ(&upDir);
            sead::Vector3f holt;
            holt.setCross(upDir, sead::Vector3f::ey);
            matrix5.setBase(0, holt);
            matrix5.setBase(1, sead::Vector3f::ey);
            matrix5.setBase(2, -upDir);
            upDir.x = -upDir.x;
            upDir.y = -upDir.y;
        }

        zeControl.rotate(al::getQuat(this));
        if (isSensor != false) {
            al::calcFrontDir(&upDir, this);
            zeControl += upDir * 100.0f;
        }
        matrix5.setBase(3, zeControl + al::getTrans(this));
        al::calcJointPos(&leControl, this, "Stick05");
        leControl.y = sead::Mathf::max(leControl.y, al::getTrans(this).y) + 100.0f;
    }
}

void HackFork::calcAnim() {
    al::LiveActor::calcAnim();
    if (isHack())
        updateCapMtx();
}

void HackFork::updateCapMtx() {
    sead::Matrix34f mtx = *al::getJointMtxPtr(this, mJointName);
    al::normalize(&mtx);
    matrix1.setMul(mtx, matrix4);
}

void HackFork::calcHackDir(al::HitSensor* sensor) {
    if (isSensor)
        hack.set({0.0f, -1.0f, 0.0f});
    else
        hack = rs::getPlayerPos(this) - al::getSensorPos(sensor);

    sead::Vector3f frontDir;
    al::calcFrontDir(&frontDir, this);
    al::verticalizeVec(&hack, frontDir, hack);
    if (!al::tryNormalizeOrZero(&hack))
        al::calcUpDir(&hack, this);

    quat.inverse(&quat2);
}

void HackFork::exeWait() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "Wait");
        al::showModelIfHide(this);
    }
    if (mEventFlowExecutor != nullptr)
        rs::updateEventFlow(mEventFlowExecutor);
}

void HackFork::exeHackStartWait() {
    if (al::isFirstStep(this)) {
        if (mCameraTicket != nullptr) {
            al::startCamera(this, mCameraTicket, -1);
            al::requestStopCameraVerticalAbsorb(this);
        }
        al::setCameraTarget(this, mMatrixCameraTarget);
        isHackSwing = false;
        hackDelay = 0;
        isShoot = false;
        airVel = 0;
    }
    if (!rs::isTriggerHackSwing(mPlayerHack)) {
        hackDelay++;
    } else {
        isHackSwing = true;
        hackDelay = 0;
    }
    if (rs::isHackStartDemoEnterMario(mPlayerHack))
        al::setNerve(this, &NrvHackFork.HackStart);
}

void HackFork::exeDamping() {
    if (al::isFirstStep(this))
        al::showModelIfHide(this);
    if (!al::isNearZero(touchForce, 0.001f) || !al::isNearZero(damping, 0.001f)) {
        controlSpring();
    } else {
        if (al::isNearZero(damping2, 0.001f)) {
            damping2 = 0.0f;
            damping = 0.0f;
            touchForce = 0.0f;
            al::validateClipping(this);
            al::setNerve(this, &NrvHackFork.Wait);
            return;
        }
        damping2 *= 0.9f;
    }
}

void HackFork::exeHackStart() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "HackStart");
        mHackStartShaderCtrl->start();
        newJump = {0.0f, 0.0f, 0.0f};
    }
    checkSwing();
    if (al::isActionEnd(this)) {
        rs::endHackStartDemo(mPlayerHack, this);
        al::setNerve(this, &NrvHackFork.HackWait);
    }
}

void HackFork::exeHackWait() {
    controlSpring();
    inputBuffer.clear();
    sead::Vector3f frontDir;
    al::calcFrontDir(&frontDir, this);
    sead::Vector3f input;
    if (!trySwingJump() && updateInput(&input, frontDir) &&
        sead::Mathf::abs(input.dot(upDir)) >
            sead::Mathf::cos(isLongJump ? 3.1415927f : 0.78539819f)) {
        newJump.set(input);
        isJump = newJump.dot(upDir) < 0.0f;
        al::setNerve(this, &NrvHackFork.HackBend);
    }
}

void HackFork::exeHackBend() {
    if (al::isFirstStep(this))
        isSheep = false;

    if (!al::isLessEqualStep(this, 15) || !trySwingJump()) {
        sead::Vector3f frontDir;
        al::calcFrontDir(&frontDir, this);

        sead::Vector3f input;
        bool isInput = updateInput(&input, frontDir);
        if (!isInput || rs::isTriggerHackSwing(mPlayerHack)) {
            if (isSheep == false) {
                al::setNerve(this, &NrvHackFork.HackWait);
            } else {
                if (isInput) {
                    airVel = 60;
                    isShoot = true;
                }
                al::setNerve(this, &NrvHackFork.HackShoot);
            }
        }
        sead::Vector3f oldJump = newJump;
        f32 jumpDir = isJump ? -1.0f : 1.0f;

        f32 angle = sead::Mathf::clamp((jumpDir * upDir).dot(input), -1.0f, 1.0f);
        if (sead::Mathf::rad2deg(acosf(angle)) < (isLongJump ? 180.0f : 45.0f))
            newJump = newJump * 0.5f + input * 0.5f;
        al::normalize(&newJump);
        f32 oldDamping = damping;
        bendAndTwist(newJump, frontDir);
        f32 rumbleVolume = (damping - oldDamping) * 0.5f + (newJump - oldJump).length() * 3.3f;
        if (!al::isNearZero(rumbleVolume, 0.001f)) {
            al::holdSe(this, "PgBendLv");
            sead::Vector3f jointPos;
            al::calcJointPos(&jointPos, this, "Stick03");
            al::PadRumbleParam param =
                al::PadRumbleParam(0.0f, 1300.0f, rumbleVolume, rumbleVolume);
            alPadRumbleFunction::startPadRumbleWithParam(this, jointPos, "パルス（中）", param, -1);
        }
        if (4.5 <= damping)
            isSheep = true;
    }
}

void HackFork::exeHackShoot() {
    controlSpring();
    if (damping < 0.0f) {
        shoot();
        al::setNerve(this, &NrvHackFork.Damping);
    }
}

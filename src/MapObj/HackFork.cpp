#include "MapObj/HackFork.h"

#include "Library/Camera/CameraTicket.h"
#include "Library/Camera/CameraUtil.h"
#include "Library/Collision/PartsConnector.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"

#include "Player/IUsePlayerHack.h"
#include "Player/PlayerHackStartShaderCtrl.h"
#include "Scene/GuidePosInfoHolder.h"
#include "Util/Hack.h"
#include "Util/NpcEventFlowUtil.h"
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

HackFork::HackFork(const char* name) : al::LiveActor(name),vptr(&inputArray[0]) {}

void HackFork::init(const al::ActorInitInfo& initInfo) {}

void HackFork::attackSensor(al::HitSensor* self, al::HitSensor* other) {
    if (al::isSensorName(self, "Push") && !al::sendMsgPush(other, self) && !isSensor) {
        const sead::Vector3f& velocity = al::getActorVelocity(other);
        if (velocity.x * velocity.x + velocity.z * velocity.z < 4.999696f)
            rs::sendMsgPushToPlayer(other, self);
    }
}

bool HackFork::receiveMsg(const al::SensorMsg* message, al::HitSensor* other, al::HitSensor* self) {
}

void HackFork::initBasicPoseInfo() {}

void HackFork::initAfterPlacement() {
    if (mMtxConnector != nullptr) {
        sead::Vector3f frontDir;
        al::calcFrontDir(&frontDir, this);
        sead::Vector3f a = al::getTrans(this) + frontDir * 100.0f;
        frontDir = frontDir *= -200.0f;
        al::attachMtxConnectorToCollision(mMtxConnector, this, a, frontDir);
    }
}

void HackFork::tryTouch(float, char const*) {}

void HackFork::resetCapMtx(al::HitSensor*) {}

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

void HackFork::trySwingJump() {}

void HackFork::updateInput(sead::Vector3f*, sead::Vector3f) {}

f32 HackFork::getJumpRange() const {
    return isLongJump ? 180.0f : 45.0f;
}

void HackFork::bendAndTwist(const sead::Vector3f&, const sead::Vector3f&) {}

void HackFork::shoot() {
    sead::Vector3f frontDir;
    al::calcFrontDir(&frontDir, this);
    sead::Quatf quack;
    al::makeQuatUpFront(&quack, frontDir, newJump);
    if (quack.length() > 0)
        newDir = frontDir;
    rs::resetRouteHeadGuidePosPtr(this);
    al::startHitReaction(this, "ジャンプ");
    al::tryStartAction(this, "HackEnd");
}

void HackFork::control() {}

void HackFork::calcAnim() {}

void HackFork::updateCapMtx() {}

void HackFork::calcHackDir(al::HitSensor*) {}

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
        al::setCameraTarget(this, mCameraTargetBase);
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

void HackFork::exeHackWait() {}

void HackFork::exeHackBend() {}

void HackFork::exeHackShoot() {
    controlSpring();
    if (damping < 0.0f) {
        shoot();
        al::setNerve(this, &NrvHackFork.Damping);
    }
}

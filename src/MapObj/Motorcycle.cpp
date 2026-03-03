#include "MapObj/Motorcycle.h"

#include "Library/Camera/CameraUtil.h"
#include "Library/Collision/PartsConnectorUtil.h"
#include "Library/Effect/EffectSystemInfo.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorAreaFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nature/NatureUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Shadow/ActorShadowUtil.h"
#include "Library/Controller/PadRumbleFunction.h"

#include "MapObj/MotorcyclePlayerAnimator.h"
#include "Util/CameraUtil.h"
#include "Util/InputInterruptTutorialUtil.h"
#include "Util/PlayerCollisionUtil.h"
#include "Util/PlayerPuppetFunction.h"
#include "Util/SensorMsgFunction.h"

namespace {
NERVE_IMPL(Motorcycle, Wait)
NERVE_IMPL(Motorcycle, Creep)
NERVE_IMPL(Motorcycle, Fall)
NERVE_IMPL(Motorcycle, Jump)
NERVE_IMPL(Motorcycle, Reaction)
NERVE_IMPL(Motorcycle, Reset)
NERVE_IMPL_(Motorcycle, ResetNoReaction, Reset)
NERVE_IMPL_(Motorcycle, RideStartLeft, RideStart)
NERVE_IMPL_(Motorcycle, RideStartRight, RideStart)
NERVE_IMPL(Motorcycle, RideStartOn)
NERVE_IMPL(Motorcycle, RideWait)
NERVE_IMPL(Motorcycle, RideWaitJump)
NERVE_IMPL(Motorcycle, RideWaitLand)
NERVE_IMPL(Motorcycle, RideRunStart)
NERVE_IMPL(Motorcycle, RideRun)
NERVE_IMPL(Motorcycle, RideRunCollide)
NERVE_IMPL(Motorcycle, RideRunFall)
NERVE_IMPL(Motorcycle, RideRunWheelie)
NERVE_IMPL(Motorcycle, RideRunLand)
NERVE_IMPL(Motorcycle, RideRunJump)
NERVE_IMPL(Motorcycle, RideRunBoundStart)
NERVE_IMPL(Motorcycle, RideRunBound)
NERVE_IMPL(Motorcycle, RideRunClash)
NERVE_IMPL(Motorcycle, RideParkingSnap)
NERVE_IMPL(Motorcycle, RideParkingStart)
NERVE_IMPL(Motorcycle, RideParking)
NERVE_IMPL(Motorcycle, RideParkingAfter)

NERVES_MAKE_STRUCT(Motorcycle, Wait, Jump, RideWaitJump, RideRunFall, RideRunWheelie, RideRunJump,
                   RideRunBoundStart, RideRunCollide, RideRunLand, Fall, ResetNoReaction,
                   RideStartLeft, RideStartRight, RideStartOn, RideWait, Creep, Reaction, Reset,
                   RideWaitLand, RideRunStart, RideRun, RideRunBound)
NERVES_MAKE_NOSTRUCT(Motorcycle, RideRunClash, RideParking, RideParkingAfter)
}  // namespace

const sead::Vector3f verticalUp = {0.0f, -1.0f, 0.0};

bool funA(Motorcycle* actor) {
    if (!rs::isOnGround(actor, actor))
        return false;

    if (al::isInWater(actor)) {
        al::setVelocityZero(actor);
        al::invalidateHitSensors(actor);
        al::hideModelIfShow(actor);
        al::setNerve(actor, &NrvMotorcycle.Reset);
        return true;
    }

    if (rs::isCollisionCodeJump(actor)) {
        al::HitSensor* sensor = rs::tryGetCollidedGroundSensor(actor);
        if (sensor)
            rs::sendMsgPlayerTouchFloorJumpCode(sensor, al::getHitSensor(actor, "PlayerBody"));
        al::setNerve(actor, &NrvMotorcycle.Jump);
        return true;
    }

    al::setNerve(actor, &NrvMotorcycle.Creep);
    return true;
}

void funV(Motorcycle* actor) {
    if (rs::isCollidedCeiling(actor) && al::getVelocity(actor).dot(verticalUp) < 0.0) {
        sead::Vector3f* velocity = al::getVelocityPtr(actor);
        al::verticalizeVec(velocity, verticalUp, *velocity);

        al::HitSensor* sensor = rs::tryGetCollidedCeilingSensor(actor);
        if (sensor)
            al::sendMsgPlayerUpperPunch(sensor, al::getHitSensor(actor, "PlayerAttack"));
    }
}

bool funF(Motorcycle* actor, IUsePlayerPuppet* playerPuppet) {
    if (!rs::isOnGround(actor, actor))
        return false;

    if (rs::isCollisionCodeJump(actor)) {
        al::HitSensor* sensor = rs::tryGetCollidedGroundSensor(actor);
        if (sensor)
            rs::sendMsgPlayerTouchFloorJumpCode(sensor, al::getHitSensor(actor, "PlayerBody"));
        al::setNerve(actor, &NrvMotorcycle.RideRunJump);
        return true;
    }

    if (rs::isPuppetTrigJumpButton(playerPuppet)) {
        al::setNerve(actor, &NrvMotorcycle.RideWaitJump);
        return true;
    }

    return false;
}

bool funR(Motorcycle* actor, IUsePlayerPuppet* playerPuppet) {
    if (!rs::isOnGround(actor, actor))
        return false;

    if (rs::isCollisionCodeJump(actor)) {
        al::HitSensor* sensor = rs::tryGetCollidedGroundSensor(actor);
        if (sensor)
            rs::sendMsgPlayerTouchFloorJumpCode(sensor, al::getHitSensor(actor, "PlayerBody"));

        al::setNerve(actor, &NrvMotorcycle.RideRunJump);
        return true;
    }

    if (rs::isPuppetTrigJumpButton(playerPuppet)) {
        if (sead::Mathf::rad2deg(sead::Mathf::asin(al::getFront(actor).y)) > 17.5f)
            al::requestStopCameraVerticalAbsorb(actor);
        al::setNerve(actor, &NrvMotorcycle.RideRunJump);
        return true;
    }

    return false;
}

void funH(Motorcycle* actor, MotorcyclePlayerAnimator* playerAnimator,
          al::CameraTargetBase* cameraTarget, al::CameraSubTargetBase* cameraSubTarget) {
    playerAnimator->endBind();
    alPadRumbleFunction::stopPadRumbleDirectValue(actor, -1);
    rs::tryCloseBindTutorial(actor);
    al::invalidateHitSensors(actor);
    al::validateHitSensorBindableAll(actor);
    al::validateHitSensorRideAll(actor);
    al::hideSilhouetteModel(actor);
    al::validateOcclusionQuery(actor);
    al::showModelIfHide(actor);
    al::resetCameraTarget(actor, cameraTarget);
    al::offCameraRideObj(actor);
    al::invalidateDepthShadowMap(actor);
    al::onDepthShadowModel(actor);
    if (al::isActiveCameraSubTarget(cameraSubTarget))
        al::resetCameraSubTarget(actor, cameraSubTarget);

    if (!funA(actor))
        al::setNerve(actor, &NrvMotorcycle.Fall);
}

Motorcycle::Motorcycle(const char* name) : al::LiveActor(name) {}

void Motorcycle::init(const al::ActorInitInfo& info) {}

void Motorcycle::initAfterPlacement() {
    if (mCollisionPartsConnector)
        al::attachCollisionPartsConnectorToGround(mCollisionPartsConnector, this);
}

void Motorcycle::attackSensor(al::HitSensor* self, al::HitSensor* other) {}

bool Motorcycle::receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                            al::HitSensor* self) {}

void Motorcycle::movement() {}

void Motorcycle::kill() {
    al::LiveActor::kill();
    al::onDepthShadowModel(this);
    al::invalidateDepthShadowMap(this);
}

void Motorcycle::updateCollider() {}

void Motorcycle::calcAnim() {
    al::LiveActor::calcAnim();
    if (mPlayerPuppet && !_249) {
        // FUN_71002c83d4(mPlayerPuppet,this,&field_0x1bc,&vector3);
    }
}

PlayerCollider* Motorcycle::getPlayerCollider() const {
    return mPlayerCollider;
}

void Motorcycle::exeWait() {}

void Motorcycle::exeCreep() {}

void Motorcycle::exeFall() {
    bool isInWater = al::isInWater(this);
    f32 fVar3 = -2.0f;
    if (!isInWater)
        fVar3 = -1.0f;
    al::addVelocityY(this, fVar3);
    if (!isInWater)
        al::scaleVelocityExceptDirection(this, verticalUp, 0.95f);
    else
        al::scaleVelocityParallelVertical(this, verticalUp, 0.8f, 0.925f);
    if (0.0f < al::getVelocity(this).dot({0.0f, -1.0f, 0.0f}))
        al::limitVelocityDir(this, verticalUp, 35.0f);
    if (!al::isInDeathArea(this)) {
        if (!funA(this)) {
            if (al::isGreaterEqualStep((IUseNerve*)this, 0x168))
                goto LAB_71002ca074;

            if (al::isInWater(this) && !_248) {
                al::startHitReaction(this, "着水");
                return;
            }
        }
        return;
    }
LAB_71002ca074:
    al::setVelocityZero(this);
    al::invalidateHitSensors(this);
    al::hideModelIfShow(this);
    al::setNerve((IUseNerve*)this, &NrvMotorcycle.Reset);
    return;
}

void Motorcycle::exeJump() {
    mPlayerAnimator->tryStartBindRideLandIfJump();
    floatJump = 0.0;
}

void Motorcycle::endJump() {}

void Motorcycle::exeReaction() {}

void Motorcycle::exeReset() {
    if (al::isFirstStep(this)) {
        al::tryKillEmitterAndParticleAll(this);
        al::stopAction(this);
        if (al::isNerve(this, &NrvMotorcycle.Reset))
            al::startHitReaction(this, "リセット");
        al::offCollide(this);
        floatA = 0.0f;
        floatB = 0.0f;
        floatC = 0.0f;
        floatJump = 0.0f;
        valA = -1;
    }

    if (al::isStep(this, 1)) {
        al::resetQuatPosition(this, quat, vector);
        rs::resetCollision(this);
    }

    if (al::isGreaterEqualStep(this, 60)) {
        al::validateClipping(this);
        al::restartAction(this);
        al::showModelIfHide(this);
        al::invalidateHitSensors(this);
        al::validateHitSensorBindableAll(this);
        al::validateHitSensorRideAll(this);
        al::startHitReaction(this, "出現");
        al::setNerve(this, &NrvMotorcycle.Wait);
    }
}

void Motorcycle::exeRideStart() {
    if (al::isFirstStep(this)) {
        al::tryStartActionIfNotPlaying(this, "Wait");
        if (al::isNerve(this, &NrvMotorcycle.RideStartLeft))
            mPlayerAnimator->startBindRideStartL();
        else
            mPlayerAnimator->startBindRideStartR();
    }

    if (rs::isPuppetActionEnd(mPlayerPuppet))
        al::setNerve(this, &NrvMotorcycle.RideStartOn);
}

void Motorcycle::exeRideStartOn() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "RideOn");
        mPlayerAnimator->startBindRideOn();
        rs::requestResetCameraAngleV(this, 0);
    }

    if (rs::isPuppetActionEnd(mPlayerPuppet)) {
        rs::validatePuppetLookAt(mPlayerPuppet);
        al::setNerve(this, &NrvMotorcycle.RideWait);
    }
}

void Motorcycle::exeRideWait() {}

void Motorcycle::exeRideWaitJump() {}

void Motorcycle::endRideWaitJump() {}

void Motorcycle::exeRideWaitLand() {}

void Motorcycle::exeRideRunStart() {}

void Motorcycle::exeRideRun() {
    funV(this);
}

void Motorcycle::exeRideRunCollide() {}

void Motorcycle::exeRideRunFall() {}

void Motorcycle::exeRideRunWheelie() {
    mPlayerAnimator->tryStartBindRideLandIfJump();
    floatJump = 0.0;
}

void Motorcycle::endRideRunWheelie() {}

void Motorcycle::exeRideRunLand() {}

void Motorcycle::exeRideRunJump() {
    mPlayerAnimator->tryStartBindRideLandIfJump();
    floatJump = 0.0;
}

void Motorcycle::endRideRunJump() {
    mPlayerAnimator->tryStartBindRideLandIfJump();
    floatJump = 0.0;
}

void Motorcycle::exeRideRunBoundStart() {}

void Motorcycle::exeRideRunBound() {}

void Motorcycle::exeRideRunClash() {}

void Motorcycle::exeRideParkingSnap() {}

void Motorcycle::exeRideParkingStart() {
    if (al::isFirstStep(this))
        al::startAction(this, "ParkingStart");
    al::setNerveAtActionEnd(this, &RideParking);
}

void Motorcycle::exeRideParking() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "Parking");
        // al::tryOnStageSwitch(_150,"SwitchMotorcycleFloorTouchBothWheelOn");
        al::onCollide(this);
    }

    // FUN_71002c9950(floatA,this);
    al::addVelocityToGravity(this, 2.0f);
    al::scaleVelocity(this, 0.95f);
    if (rs::isCollidedGround(this))
        al::limitVelocityDirSign(this, al::getGravity(this), 3.0f);
    al::setNerveAtGreaterEqualStep(this, &RideParkingAfter, 8);
}

void Motorcycle::exeRideParkingAfter() {}

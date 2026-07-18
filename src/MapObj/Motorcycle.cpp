#include "MapObj/Motorcycle.h"

#include "Library/Camera/CameraUtil.h"
#include "Library/Collision/PartsConnectorUtil.h"
#include "Library/Controller/PadRumbleFunction.h"
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
#include "Library/Stage/StageSwitchUtil.h"

#include "MapObj/MotorcyclePlayerAnimator.h"
#include "Util/CameraUtil.h"
#include "Util/InputInterruptTutorialUtil.h"
#include "Util/ObjUtil.h"
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

void funC(IUsePlayerPuppet* playerPuppet, Motorcycle* actor, const sead::Quatf& startQuat,
          const sead::Vector3f& startTrans) {
    sead::Quatf jointQuat = sead::Quatf::unit;
    sead::Vector3f trans = {0.0f, 0.0f, 0.0f};
    sead::Matrix34f* jointMtx = al::getJointMtxPtr(actor, "JointRoot");

    jointMtx->toQuat(jointQuat);
    jointQuat.normalize();
    jointMtx->getTranslation(trans);

    if (al::isNerve(actor, &NrvMotorcycle.RideStartLeft) ||
        al::isNerve(actor, &NrvMotorcycle.RideStartRight)) {
        f32 rate = al::calcNerveRate(actor, rs::getPuppetAnimFrameMax(playerPuppet));
        al::slerpQuat(&jointQuat, startQuat, jointQuat, rate);
        al::lerpVec(&trans, startTrans, trans, rate);
    }

    rs::setPuppetTrans(playerPuppet, trans);
    rs::setPuppetQuat(playerPuppet, jointQuat);
}

void funB(f32 bloat, Motorcycle* actor) {
    sead::Vector3f normal;
    if (rs::isCollidedGround(actor))
        normal = rs::getCollidedGroundNormal(actor);
    else
        normal = actor->getParams()->normal;

    if (al::isParallelDirection(al::getGravity(actor), -normal, 0.01f)) {
    }

    MotorcycleParams* params = actor->getParams();
    if (al::isNormalize(params->vector_8) && params->bool_2 && params->bool_3) {
        sead::Vector3f front = al::getFront(actor);
        al::verticalizeVec(&front, verticalUp, front);
        if (al::tryNormalizeOrZero(&front)) {
            if (al::getFront(actor).y <= 0.0f) {
                al::rotateVectorDegree(&front, front, front.cross({0.0f, -1.0f, 0.0f}), 45.0f);
                al::normalize(&front);
            }

            al::turnVecToVecDegree(&front, al::getFront(actor), front, 2.0f);
            sead::Vector3f* frontPtr = al::getFrontPtr(actor);
            al::turnVecToVecRate(frontPtr, *frontPtr, front, 0.15f);
            al::normalize(al::getFrontPtr(actor));

            sead::Vector3f* gravityPtr = al::getGravityPtr(actor);
            al::verticalizeVec(gravityPtr, al::getFront(actor), *gravityPtr);
            al::normalize(al::getGravityPtr(actor));
        }
    }

    if (al::isNormalize(params->vector_8) && params->bool_2 && params->bool_3)
        rs::tryFollowRotateFrontAxisUpIfCollidedGround(actor, actor);
}

void funT(Motorcycle* actor, const sead::Vector3f& vecA, const sead::Vector3f& vecB) {
    sead::Vector3f velH = {0.0f, 0.0f, 0.0f};
    sead::Vector3f velV = {0.0f, 0.0f, 0.0f};
    al::separateVectorHV(&velH, &velV, vecA, al::getVelocity(actor));

    sead::Vector3f front = al::getFront(actor);
    al::verticalizeVec(&front, vecB, front);
    al::tryNormalizeOrZero(&front);

    al::setVelocity(actor, (velH.length() * front) + (velV.length() * vecB));
}

void funI(sead::Vector2f* outStick, Motorcycle* actor, IUsePlayerPuppet* playerPuppet) {
    if (!actor->isStickWorldPose()) {
        outStick->set(rs::getPuppetMoveStick(playerPuppet));
        return;
    }

    outStick->set(0.0f, 0.0f);
    sead::Vector3f vecView = {0.0f, 0.0f, 0.0f};
    al::calcVecViewInput(&vecView, rs::getPuppetMoveStick(playerPuppet), -al::getGravity(actor),
                         al::getViewMtxPtr(actor, 0));

    sead::Vector3f sideDir = {0.0f, 0.0f, 0.0f};
    al::calcSideDir(&sideDir, actor);
    al::verticalizeVec(&sideDir, sead::Vector3f::ey, sideDir);
    al::normalize(&sideDir);

    sead::Vector3f front = al::getFront(actor);
    al::verticalizeVec(&front, sead::Vector3f::ey, front);
    al::normalize(&front);

    sead::Vector3f parallelVec = {0.0f, 0.0f, 0.0f};
    al::parallelizeVec(&parallelVec, sideDir, vecView);
    sead::Vector3f diff = vecView - parallelVec;

    if (!al::isNearZero(parallelVec)) {
        f32 signX = (parallelVec.dot(sideDir) > 0.0f) ? -1.0f : 1.0f;
        outStick->x = signX * parallelVec.length();
    }

    if (!al::isNearZero(diff)) {
        f32 signY = (diff.dot(front) > 0.0f) ? 1.0f : -1.0f;
        outStick->y = signY * diff.length();
    }
}

void funJ(f32 a, Motorcycle* actor, IUsePlayerPuppet* playerPuppet) {
    sead::Vector2f stick = {0.0f, 0.0f};
    funI(&stick, actor, playerPuppet);

    al::rotateVectorDegreeY(al::getFrontPtr(actor), stick.x * -0.5f);
    al::normalize(al::getFrontPtr(actor));

    sead::Vector3f front = al::getFront(actor);
    front.y = 0;
    al::tryNormalizeOrZero(&front);
    const sead::Vector3f& vel = al::getVelocity(actor);
    sead::Vector3f velocity =
        front * sead::Mathf::sqrt(sead::Mathf::pow(vel.x, 2) + sead::Mathf::pow(vel.z, 2));

    f32 friction = 0.95f;
    if (rs::isPuppetHoldActionButton(playerPuppet))
        friction = 1.0f;

    al::setVelocity(actor, velocity * friction +
                               (al::getVelocity(actor).y - a) * sead::Vector3f::ey * 0.95f);
}

Motorcycle::Motorcycle(const char* name) : al::LiveActor(name) {}

void Motorcycle::init(const al::ActorInitInfo& info) {}

void Motorcycle::initAfterPlacement() {
    if (mCollisionPartsConnector)
        al::attachCollisionPartsConnectorToGround(mCollisionPartsConnector, this);
}

bool Motorcycle::isRideRun_() {
    return al::isNerve(this, &NrvMotorcycle.RideRunStart) ||
           al::isNerve(this, &NrvMotorcycle.RideRun) ||
           al::isNerve(this, &NrvMotorcycle.RideRunCollide) ||
           al::isNerve(this, &NrvMotorcycle.RideRunFall) ||
           al::isNerve(this, &NrvMotorcycle.RideRunWheelie) ||
           al::isNerve(this, &NrvMotorcycle.RideRunLand) ||
           al::isNerve(this, &NrvMotorcycle.RideRunJump) ||
           al::isNerve(this, &NrvMotorcycle.RideRunBoundStart) ||
           al::isNerve(this, &NrvMotorcycle.RideRunBound);
}

void Motorcycle::attackSensor(al::HitSensor* self, al::HitSensor* other) {
    if (mPlayerPuppet && al::isSensorPlayerAttack(self)) {
        if (al::isNerve(this, &NrvMotorcycle.Jump) ||
            al::isNerve(this, &NrvMotorcycle.RideWaitJump) ||
            al::isNerve(this, &NrvMotorcycle.RideRunFall) ||
            al::isNerve(this, &NrvMotorcycle.RideRunWheelie) ||
            al::isNerve(this, &NrvMotorcycle.RideRunJump) ||
            al::isNerve(this, &NrvMotorcycle.RideRunBoundStart) ||
            al::isNerve(this, &NrvMotorcycle.RideRunBound)) {
            if (al::sendMsgPlayerAttackTrample(other, self, nullptr))
                return;
        }

        if (isRideRun_()) {
            if (rs::sendMsgMotorcycleDashAttack(other, self) ||
                rs::sendMsgMotorcycleAttack(other, self) || rs::sendMsgBirdFlyAway(other, self) ||
                mPlayerPuppet)
                return;
        }
    }

    if (al::isSensorBindableAll(self) || al::isSensorPlayerAll(self) ||
        rs::sendMsgIgnorePushMotorcycle(other, self) || al::sendMsgPush(other, self)) {
        return;
    }

    if (_23c <= 0) {
        if (rs::sendMsgPushToPlayer(other, self))
            return;
    }

    rs::sendMsgPushToMotorcycle(other, self);
}

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
    if (mPlayerPuppet && !_249)
        funC(mPlayerPuppet, this, mStartQuat, mStartTrans);
}

void Motorcycle::exeWait() {}

void Motorcycle::exeCreep() {
    if (al::isFirstStep(this))
        al::tryStartActionIfNotPlaying(this, "Wait");

    floatA = al::lerpValue(floatA, 0.0f, 0.2f);
    floatB = al::lerpValue(floatB, 0.0f, 0.2f);
    floatC = al::lerpValue(floatC, 0.0f, 0.2f);
    floatJump = al::lerpValue(floatJump, 0.0f, 0.2f);
    funB(floatA, this);

    al::addVelocityToGravity(this, 2.0f);
    al::scaleVelocity(this, 0.9f);
    al::limitVelocityDir(this, al::getGravity(this), 360.0f);

    if (!rs::isCollidedGround(this) && al::isGreaterEqualStep(this, 2)) {
        al::setNerve(this, &NrvMotorcycle.Fall);
        return;
    }

    if (!al::isInDeathArea(this) && !al::isGreaterEqualStep(this, 360)) {
        if (al::isVelocitySlowH(this, 3.0f))
            al::setNerve(this, &NrvMotorcycle.Wait);
        return;
    }

    al::setVelocityZero(this);
    al::invalidateHitSensors(this);
    al::hideModelIfShow(this);
    al::setNerve(this, &NrvMotorcycle.Reset);
}

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

void Motorcycle::exeJump() {}

void Motorcycle::endJump() {
    mPlayerAnimator->tryStartBindRideLandIfJump();
    floatJump = 0.0f;
}

void Motorcycle::exeReaction() {
    if (al::isFirstStep(this))
        al::startAction(this, "Reaction");

    floatA = al::lerpValue(floatA, 0.0f, 0.2f);
    floatB = al::lerpValue(floatB, 0.0f, 0.2f);
    floatC = al::lerpValue(floatC, 0.0f, 0.2f);
    floatJump = al::lerpValue(floatJump, 0.0f, 0.2f);
    al::setNerveAtActionEnd(this, &NrvMotorcycle.Wait);
}

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
        al::resetQuatPosition(this, mStartQuat, vector);
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

void Motorcycle::endRideWaitJump() {
    mPlayerAnimator->tryStartBindRideLandIfJump();
    floatJump = 0.0f;
}

void Motorcycle::exeRideWaitLand() {}

void Motorcycle::exeRideRunStart() {}

void Motorcycle::exeRideRun() {
    funV(this);
}

void Motorcycle::exeRideRunCollide() {}

void Motorcycle::exeRideRunFall() {}

void Motorcycle::exeRideRunWheelie() {}

void Motorcycle::endRideRunWheelie() {
    mPlayerAnimator->tryStartBindRideLandIfJump();
    floatJump = 0.0f;
}

void Motorcycle::exeRideRunLand() {}

void Motorcycle::exeRideRunJump() {}

void Motorcycle::endRideRunJump() {
    mPlayerAnimator->tryStartBindRideLandIfJump();
    floatJump = 0.0;
}

void Motorcycle::exeRideRunBoundStart() {}

void Motorcycle::exeRideRunBound() {
    if (al::isFirstStep(this))
        al::startAction(this, "Run");

    if (!rs::isActiveBindKeepDemo(mBindKeepDemoInfo)) {
        if (rs::isTriggerGetOff(mPlayerPuppet)) {
            if (!al::isInAreaObj(this, "MotorcycleInvalidGetOffArea")) {
                // funD(this,&mPlayerPuppet,&field_0x23c,mPlayerAnimator,mCameraTargetBase,mCameraSubTargetBase);
                return;
            }
            al::startHitReaction(this, "降車キャンセル");
        }
    }
    mParams->normal.set(0.0f, 1.0f, 0.0f);
    // funE(this,mPlayerPuppet,floatA,field11_0x148,_248);
}

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
        al::tryOnStageSwitch(mParkingParams->actor, "SwitchMotorcycleFloorTouchBothWheelOn");
        al::onCollide(this);
    }

    funB(floatA, this);
    al::addVelocityToGravity(this, 2.0f);
    al::scaleVelocity(this, 0.95f);
    if (rs::isCollidedGround(this))
        al::limitVelocityDirSign(this, al::getGravity(this), 3.0f);
    al::setNerveAtGreaterEqualStep(this, &RideParkingAfter, 8);
}

void Motorcycle::exeRideParkingAfter() {}

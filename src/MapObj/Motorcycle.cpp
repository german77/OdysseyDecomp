#include "MapObj/Motorcycle.h"

#include "Library/Area/AreaObj.h"
#include "Library/Area/AreaObjUtil.h"
#include "Library/Camera/CameraUtil.h"
#include "Library/Camera/TransCameraSubTarget.h"
#include "Library/Collision/CollisionParts.h"
#include "Library/Collision/CollisionPartsKeeperUtil.h"
#include "Library/Collision/PartsConnector.h"
#include "Library/Collision/PartsConnectorUtil.h"
#include "Library/Controller/PadRumbleFunction.h"
#include "Library/Effect/EffectSystemInfo.h"
#include "Library/Item/ItemUtil.h"
#include "Library/Joint/JointControllerKeeper.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorAnimFunction.h"
#include "Library/LiveActor/ActorAreaFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nature/NatureUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Se/SeFunction.h"
#include "Library/Shadow/ActorShadowUtil.h"
#include "Library/Stage/StageSwitchUtil.h"
#include "Project/Camera/CameraSubTargetTurnParam.h"

#include "MapObj/MotorcyclePlayerAnimator.h"
#include "Player/CollisionShapeKeeper.h"
#include "Player/PlayerCollider.h"
#include "System/GameDataFunction.h"
#include "System/GameDataUtil.h"
#include "Util/CameraUtil.h"
#include "Util/InputInterruptTutorialUtil.h"
#include "Util/NpcAnimUtil.h"
#include "Util/ObjUtil.h"
#include "Util/PlayerCollisionUtil.h"
#include "Util/PlayerPuppetFunction.h"
#include "Util/PlayerUtil.h"
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
NERVES_MAKE_NOSTRUCT(Motorcycle, RideRunClash, RideParking, RideParkingAfter, RideParkingStart)
}  // namespace

const sead::Vector3f forceField5 = {0.0f, 140.0f, -85.0f};
const sead::Vector3f forceField6 = {0.0f, -70.0f, 0.0f};
const sead::Vector3f forceField7 = {0.0f, 80.0f, -120.0f};
const sead::Vector3f forceField8 = {0.0f, 80.0f, -60.0f};
const sead::Vector3f forceField = {0.0f, 50.0f, 10.0f};
const sead::Vector3f forceField2 = {-25.0f, 50.0f, -150.0f};
const sead::Vector3f forceField3 = {25.0f, 50.0f, -150.0f};
const sead::Vector3f forceField4 = {0.0f, 50.0f, -80.0f};
const sead::Vector3f verticalUp = {0.0f, -1.0f, 0.0f};

bool checkGroundStatus(Motorcycle* actor) {
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
    if (rs::isCollidedCeiling(actor) && al::getVelocity(actor).dot(verticalUp) < 0.0f) {
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

void resetState(Motorcycle* actor, MotorcyclePlayerAnimator* playerAnimator,
                al::CameraTargetBase* cameraTarget, al::TransCameraSubTarget* cameraSubTarget) {
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

    if (!checkGroundStatus(actor))
        al::setNerve(actor, &NrvMotorcycle.Fall);
}

void syncPuppetPose(IUsePlayerPuppet* playerPuppet, Motorcycle* actor, const sead::Quatf& startQuat,
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

void updateOrientation(f32 bloat, Motorcycle* actor) {
    sead::Vector3f normal;
    if (rs::isCollidedGround(actor))
        normal = rs::getCollidedGroundNormal(actor);
    else
        normal = actor->getParams()->groundNormal;

    if (al::isParallelDirection(al::getGravity(actor), -normal, 0.01f)) {
    }

    MotorcycleParams* params = actor->getParams();
    if (al::isNormalize(params->floorNormalAvg) && params->isOnJump && params->bool_3) {
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

    if (al::isNormalize(params->floorNormalAvg) && params->isOnJump && params->bool_3)
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

void calcInputStick(sead::Vector2f* outStick, Motorcycle* actor, IUsePlayerPuppet* playerPuppet) {
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

void applyAirPhysics(f32 a, Motorcycle* actor, IUsePlayerPuppet* playerPuppet) {
    sead::Vector2f stick = {0.0f, 0.0f};
    calcInputStick(&stick, actor, playerPuppet);

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

bool checkDashCollision(Motorcycle* actor) {
    if (rs::isCollisionCodePoleClimbWall(actor))
        return false;

    s32 hitNum = rs::getWallHitInfoNum(actor);
    s32 i = 0;
    bool isCollision = false;
    for (; i < hitNum; i++) {
        isCollision |= rs::sendMsgMotorcycleDashCollide(rs::getWallHitInfoSensor(actor, i),
                                                        al::getHitSensor(actor, "PlayerBody"));
    }
    if (isCollision) {
        al::LiveActor* host = al::getSensorHost(rs::tryGetCollidedWallSensor(actor));
        if (al::isDead(host))
            return false;
        al::CollisionParts* parts = host->getCollisionParts();
        if (parts && !parts->isValidCollision())
            return false;
    }

    sead::Vector3f front = {al::getFront(actor).x, 0.0f, al::getFront(actor).z};
    if (!al::tryNormalizeOrZero(&front))
        return false;

    if (sead::Mathf::cos(sead::Mathf::deg2rad(15.0f)) <
        front.dot(-rs::getCollidedWallNormal(actor))) {
        al::setVelocityZeroX(actor);
        al::setVelocityZeroZ(actor);
        al::setNerve(actor, &RideRunClash);
        return true;
    }
    return false;
}

bool funP(Motorcycle* actor, const sead::Vector3f& vec) {
    if (!rs::isCollidedWallVelocity(actor, actor))
        return false;

    s32 hitNum = rs::getWallHitInfoNum(actor);
    bool isCollision = false;
    for (s32 i = 0; i < hitNum; i++) {
        isCollision |= rs::sendMsgMotorcycleDashCollide(rs::getWallHitInfoSensor(actor, i),
                                                        al::getHitSensor(actor, "PlayerBody"));
    }
    if (isCollision)
        return false;

    if (al::calcSpeedMax(1.8f, 0.95f) * 0.9f >= al::calcSpeedH(actor))
        return true;
    if (checkDashCollision(actor))
        return true;

    sead::Vector3f sudovec = vec;
    al::verticalizeVec(&sudovec, al::getGravity(actor), sudovec);
    if (sudovec.length() >= 10.0f) {
        al::setNerve(actor, &NrvMotorcycle.RideRunCollide);
        return true;
    }
    return false;
}

void funPY(Motorcycle* actor, IUsePlayerPuppet* playerPuppet, AccelerationState* accelerationState,
           f32* steerAngle) {
    f32 accelRate =
        al::lerpValue(1.0f, 0.75f, al::normalize(actor->getParams()->framesInAir, 0, 15));
    f32 speed = al::normalize(al::calcSpeedH(actor), 0.0f, al::calcSpeedMax(1.8f, 0.95f));
    f32 lerpSpeed;
    if (actor->isStickWorldPose())
        lerpSpeed = al::lerpValue(3.4f, 2.6f, speed);
    else
        lerpSpeed = al::lerpValue(3.6f, 2.8f, speed);

    if (!al::isNearZero(rs::getPuppetMoveStick(playerPuppet).x)) {
        accelRate *= lerpSpeed * al::normalizeAbs(*steerAngle, 0.0f, 55.0f);
        if (rs::isCollidedWall(actor)) {
            sead::Vector3f front = al::getFront(actor);
            al::rotateVectorDegree(&front, front, al::getGravity(actor), accelRate);
            sead::Vector3f diff = front - al::getFront(actor);
            if (al::tryNormalizeOrZero(&diff) && actor->getParams()->isInFront(diff) &&
                actor->getParams()->isInBack(diff))
                accelRate = 0.0;
        }
    } else {
        al::AreaObj* frontSnapArea = al::tryFindAreaObj(actor, "MotorcycleFrontSnapArea");
        if (frontSnapArea) {
            sead::Vector3f front = {0.0f, 0.0f, 0.0f};
            al::getAreaObjDirFront(&front, frontSnapArea);
            accelRate =
                al::calcAngleOnPlaneDegree(front, al::getFront(actor), -al::getGravity(actor)) *
                0.035f;
        } else {
            accelRate = 0.0f;
        }
    }

    sead::Vector3f* frontPtr = al::getFrontPtr(actor);
    al::rotateVectorDegree(frontPtr, *frontPtr, al::getGravity(actor), accelRate);
    al::normalize(al::getFrontPtr(actor));
    accelerationState->isAccelerating = rs::isPuppetHoldActionButton(playerPuppet);
    if (accelerationState->isAccelerating)
        accelRate =
            al::lerpValue(1.0f, 0.3f, al::normalize(sead::Mathf::abs(accelRate), 0.0f, 0.7f));
    else
        accelRate = -0.75f;

    accelerationState->accelRate += accelRate;
    accelerationState->accelRate = sead::Mathf::clamp(accelerationState->accelRate, 0.0f, 5.0f);
}

bool funS(Motorcycle* actor) {
    if (rs::isCollidedGround(actor))
        return false;
    if (actor->getParams()->framesInAir > 1) {
        if (sead::Mathf::rad2deg(sead::Mathf::sin(al::getFront(actor).y)) > 17.5f) {
            sead::Vector3f trans = {0.0f, 0.0f, 0.0f};
            al::calcTransLocalOffset(&trans, actor, forceField);

            if (!alCollisionUtil::getFirstPolyOnArrow(actor, nullptr, nullptr, trans,
                                                      al::getGravity(actor) * 125.0f, nullptr,
                                                      nullptr)) {
                al::limitVelocityDirSign(actor, al::getGravity(actor), 0.0f);
                al::setNerve(actor, &NrvMotorcycle.RideRunWheelie);
                return true;
            }
        }
    }

    MotorcycleParams* params = actor->getParams();
    if (params->framesInAir >= 6) {
        funT(actor, -params->groundNormal, verticalUp);
        al::limitVelocityDirSign(actor, verticalUp, 3.0f);
        al::setNerve(actor, &NrvMotorcycle.RideRunFall);
        return true;
    }
    return false;
}

static inline f32 getDistance(Motorcycle* actor, MotorcycleParams* params) {
    sead::Vector3f facingDir = al::getTrans(actor) - params->lastGroundPos;
    al::parallelizeVec(&facingDir, al::getGravity(actor), facingDir);
    return facingDir.dot(al::getGravity(actor));
}

bool funL(Motorcycle* actor) {
    if (!rs::isOnGround(actor, actor))
        return false;

    MotorcycleParams* params = actor->getParams();
    if (!rs::isCollidedGround(actor) || params->isOnGround)
        return false;

    if (getDistance(actor, params) > 700.0f) {
        al::startHitReaction(actor, "バウンド");
        f32 distance = al::normalize(getDistance(actor, actor->getParams()), 700.0f, 5000.0f);
        al::setVelocityY(actor, al::lerpValue(25.0f, 40.0f, distance));
        al::setNerve(actor, &NrvMotorcycle.RideRunBoundStart);
        return true;
    }
    return false;
}

bool funO(Motorcycle* actor) {
    if (!rs::isOnGround(actor, actor))
        return false;
    funT(actor, verticalUp, -rs::getCollidedGroundNormal(actor));
    al::limitVelocityDirSign(actor, -rs::getCollidedGroundNormal(actor), 5.0f);

    if (al::isGreaterEqualStep(actor, 15)) {
        MotorcycleParams* params = actor->getParams();
        if (rs::isCollidedGround(actor) && !params->isOnGround) {
            if (getDistance(actor, params) > 150.0f) {
                funT(actor, verticalUp, -rs::getCollidedGroundNormal(actor));
                al::limitVelocityDirSign(actor, -rs::getCollidedGroundNormal(actor), 5.0f);
                al::setNerve(actor, &NrvMotorcycle.RideRunLand);
                return true;
            }
        }
    }
    al::setNerve(actor, &NrvMotorcycle.RideRun);
    return true;
}

void startGetOff(Motorcycle* actor, IUsePlayerPuppet** playerPuppet, s32* valueA,
                 MotorcyclePlayerAnimator* valueB, al::CameraTargetBase* valueC,
                 al::TransCameraSubTarget* param_6) {
    const sead::Vector3f supip = {0.0f, -1.0f, 0.0f};

    al::limitVelocityDirSign(actor, -supip, 0.0f);

    rs::showPuppetShadow(*playerPuppet);
    rs::validatePuppetReceivePush(*playerPuppet);

    MotorcycleParams* params = actor->getParams();
    if (params->backContactPoints.size() > 0 && params->frontContactPoints.size() > 0) {
        sead::Vector3f offset = {0.0f, 0.0f, 0.0f};
        al::calcTransLocalOffset(&offset, actor, forceField4);
        rs::resetPuppetPosition(*playerPuppet, offset);
    }

    sead::Vector2f stick = rs::getPuppetMoveStick(*playerPuppet);
    rs::getPuppetMoveStick(*playerPuppet);
    stick.y = -stick.y;

    sead::Vector3f keko = {0.0f, 0.0f, 0.0f};
    if (al::isNearZero(stick, 0.2f) ||
        !al::calcDirViewInput(&keko, stick, verticalUp, al::getViewMtxPtr(actor, 0))) {
        al::verticalizeVec(&keko, -supip, al::getFront(actor));
        al::normalize(&keko);
        keko = -keko;
    }

    rs::setPuppetFront(*playerPuppet, keko);
    rs::setPuppetUp(*playerPuppet, -supip);

    rs::endBindJumpAndPuppetNull(playerPuppet, sead::Vector3f::ey * 20.0f + keko * 9.0f, 3);

    *valueA = 5;
    resetState(actor, valueB, valueC, param_6);
}

bool tryParking(Motorcycle* actor, IUsePlayerPuppet* playerPuppet, ParkingParams* params,
                f32* value) {
    if (rs::isPuppetHoldActionButton(playerPuppet) || al::calcSpeedH(actor) > 8.0f)
        return false;

    if (!rs::isCollidedGround(actor))
        return false;

    al::HitSensor* groundSensor = rs::tryGetCollidedGroundSensor(actor);
    if (!groundSensor)
        return false;

    sead::Vector3f backDir = {0.0f, 0.0f, 0.0f};
    al::calcBackDir(&backDir, al::getSensorHost(groundSensor));
    if (sead::Mathf::abs(backDir.dot(al::getFront(actor))) >=
        sead::Mathf::cos(sead::Mathf::deg2rad(25.0f))) {
        al::parallelizeVec(&backDir, backDir, al::getFront(actor));
        al::normalize(&backDir);
        sead::Quatf quat = sead::Quatf::unit;
        sead::Vector3f up = {0.0f, 0.0f, 0.0f};
        sead::Vector3f side = {0.0f, 0.0f, 0.0f};

        al::calcUpDir(&up, al::getSensorHost(groundSensor));
        al::makeQuatFrontUp(&quat, backDir, up);
        al::calcQuatSide(&side, quat);
        /*
        fVar10 = (local_70.y * 75.0f - local_70.z * 30.0f) + local_70.w * 0.0f;
        fVar16 = local_70.w * 30.0f + (local_70.z * 0.0f - local_70.x * 75.0f);
        fVar18 = local_70.z * fVar16;
        fVar17 = local_70.w * 75.0f + (local_70.x * 30.0f - local_70.y * 0.0f);
        fVar11 = (-(local_70.x * 0.0f) - local_70.y * 30.0f) - local_70.z * 75.0f;
        fVar12 = local_70.w * fVar10;
        fVar14 = local_70.y * fVar17;
        fVar19 = local_70.x * fVar11;
        fVar13 = local_70.z * fVar10;
        fVar20 = local_70.w * fVar16;
        fVar15 = local_70.x * fVar17;
        fVar21 = local_70.y * fVar11;
        fVar10 = local_70.y * fVar10;
        fVar16 = local_70.x * fVar16;
        fVar17 = local_70.w * fVar17;
        fVar11 = local_70.z * fVar11;
        pVVar7 = al::getActorTrans(groundSensor);
        fVar14 = pVVar7->x + ((fVar14 + (fVar12 - fVar18)) - fVar19);
        fVar12 = pVVar7->y + (((fVar13 + fVar20) - fVar15) - fVar21);
        fVar17 = pVVar7->z + ((fVar17 + (fVar16 - fVar10)) - fVar11);
        local_a0.x = 0.0f;
        local_a0.y = 0.0f;
        local_a0.z = 0.0f;
        pVVar7 = al::getTrans(actor);
        local_b0.y = pVVar7->y - fVar12;
        local_b0.x = pVVar7->x - fVar14;
        local_b0.z = pVVar7->z - fVar17;
        al::parallelizeVec(&local_a0, &local_90, &local_b0);
        fVar11 = local_a0.x * local_a0.x + local_a0.y * local_a0.y + local_a0.z * local_a0.z;
        fVar10 = SQRT(fVar11);
        if (NAN(fVar10))
            fVar10 = sqrtf(fVar11);
        if (fVar10 <= 100.0f) {
            local_c0.y = -backDir.y;
            local_c0.x = -backDir.x;
            local_c0.z = -backDir.z;
            local_b0.x = 0.0f;
            local_b0.y = 0.0f;
            local_b0.z = 0.0f;
            pVVar7 = al::getTrans(actor);
            local_d0.x = pVVar7->x - fVar14;
            local_d0.y = pVVar7->y - fVar12;
            local_d0.z = pVVar7->z - fVar17;
            al::parallelizeVec(&local_b0, &local_c0, &local_d0);
            fVar11 = local_b0.x * local_b0.x + local_b0.y * local_b0.y + local_b0.z *
        local_b0.z; fVar10 = SQRT(fVar11); if (NAN(fVar10)) fVar10 = sqrtf(fVar11); if (100.0f <
        fVar10) { bVar9 = 0; goto LAB_71002cac64;
            }
            pHVar8 = (HitSensor*)al::getHitSensor(actor, "PlayerBody");
            uVar3 = rs::sendMsgMotorcycleCollideParkingLot(groundSensor, pHVar8);
            if ((uVar3 & 1) != 0) {
                pLVar5 = al::getSensorHost(groundSensor);
                params->actor = pLVar5;
                fVar10 = value[3];
                params->mLeanAngle = value[2];
                params->mJumpAngle = fVar10;
                fVar10 = value[1];
                params->steerAngle = value[0];
                params->handleAngle = fVar10;
                al::calcQuat(&params->quatA, actor);
                (params->quatB).z = local_70.z;
                (params->quatB).w = local_70.w;
                (params->quatB).x = local_70.x;
                (params->quatB).y = local_70.y;
                pVVar7 = al::getTrans(actor);
                (params->field7_0x38).z = pVVar7->z;
                fVar10 = pVVar7->x;
                fVar11 = pVVar7->y;
                (params->field8_0x44).x = fVar14;
                (params->field8_0x44).y = fVar12;
                (params->field8_0x44).z = fVar17;
                (params->field7_0x38).x = fVar10;
                (params->field7_0x38).y = fVar11;
                al::offCollide(actor);
                al::setVelocityZero(actor);
                al::setNerve((IUseNerve*)actor, (Nerve*)&PTR_PTR_7101d16e28);
                bVar9 = 1;
                goto LAB_71002cac64;
            }
            bVar2 = false;
            bVar9 = extraout_w8;
        } else {
            bVar9 = 0;
        LAB_71002cac64:
            bVar2 = true;
        }
        if (bVar2)
            return (bool)(bVar9 & 1);
        */
    }

    return false;
}

void updateSeRumble(f32 valueA, SeRumbleState* valueB, Motorcycle* actor,
                    IUsePlayerPuppet* playerPuppet, bool isaval) {
    int iVar5;
    float fVar6;
    float fVar7;
    float fVar8;
    float fVar9;
    float fVar10;
    float fVar11;
    bool isHold = rs::isPuppetHoldActionButton(playerPuppet);

    int iVar2 = valueB->rumble;
    if (!isHold) {
        fVar11 = 0.6f;
        iVar5 = 0;
        if (-1 < iVar2 + -0xf)
            iVar5 = iVar2 + -0xf;
    } else {
        iVar5 = 0xf0;
        if (iVar2 + 1 < 0xf1)
            iVar5 = iVar2 + 1;
        fVar11 = 1.0f;
    }
    valueB->rumble = iVar5;
    fVar6 = (float)al::calcSpeedH(actor);
    fVar10 = (float)valueB->rumble;
    fVar8 = fVar10 / -240.0f + 1.0f;
    fVar7 = fVar8;
    if (1.0f < fVar8)
        fVar7 = 1.0f;
    fVar9 = 0.25f;
    if (0.25f <= fVar8)
        fVar9 = fVar7;
    valueB->volume = fVar6;
    fVar7 = 0.0f;
    if (valueB->rumble < 0x28) {
        fVar8 = (fVar10 / 40.0f + -1.0f) * 3.1415927f;
        fVar7 = sinf(fVar8);
        fVar6 = valueB->volume;
        fVar7 = (fVar8 * fVar7) / 1.8f;
    }
    fVar10 = fVar6 + -5.0f;
    bool bVar3 = (isaval & 1) == 0;
    fVar8 = 272.0f;
    if (bVar3)
        fVar8 = 160.0f;
    if (fVar10 <= 0.0f)
        fVar10 = 0.0f;
    fVar9 = fVar11 * fVar9 * 0.35f * (fVar7 * 1.45f + 1.0f);
    fVar11 = fVar9 * 0.6f;
    if (bVar3)
        fVar11 = fVar9;
    fVar10 = (fVar7 * 0.08f + 1.0f) * (fVar10 / 70.0f + 1.0f) * 168.0f;
    fVar7 = fVar10 * 1.7f;
    if (bVar3)
        fVar7 = fVar10;

    al::holdSeWithParam(actor, "CurveLv", fVar6 * valueA, "回転角(Degree)");

    fVar10 = 0.01f;
    fVar6 = 0.0f;
    if (valueA <= 0.0f) {
        fVar10 = 0.0f;
        fVar6 = 0.01f;
    }

    alPadRumbleFunction::startPadRumbleDirectValue(actor, fVar8, fVar7, fVar11, fVar11,
                                                   (0.7f - fVar6 * valueA) * 0.7f,
                                                   (fVar10 * valueA + 0.7f) * 0.7f, -1);

    f32 isHold2 = rs::isPuppetHoldActionButton(playerPuppet) ? 9.0f : 0.0f;

    al::holdSeWithParam(actor,
                        rs::isPuppetHoldActionButton(playerPuppet) ? "MoveStartLv" : "MoveEndLv",
                        isHold2 + valueB->volume, "");
    al::holdSeWithParam(actor, "MoveLv", isHold2 + valueB->volume, "");
    al::holdSeWithParam(actor, "IdleLv", isHold2 + valueB->volume, "");
    return;
}

static inline f32 getJointDistance(Motorcycle* actor, const char* jointNameA,
                                   const char* jointNameB) {
    sead::Vector3f frontTirePos = {0.0f, 0.0f, 0.0f};
    sead::Vector3f backTirePos = {0.0f, 0.0f, 0.0f};
    al::calcJointPos(&frontTirePos, actor, jointNameA);
    al::calcJointPos(&backTirePos, actor, jointNameB);
    return (frontTirePos - backTirePos).length();
}

void updateAirOrientation(Motorcycle* actor) {
    if (al::getVelocity(actor).y > 0.0f)
        return;

    sead::Vector3f hitPos = {0.0f, 0.0f, 0.0f};
    sead::Vector3f poss = (al::getTrans(actor) -
                           al::getFront(actor) * getJointDistance(actor, "FrontTire", "BackTire")) -
                          al::getGravity(actor) * 150.0f;
    if (!alCollisionUtil::getFirstPolyOnArrow(actor, &hitPos, nullptr, poss,
                                              al::getGravity(actor) * 150.0f, nullptr, nullptr)) {
        return;
    }

    sead::Vector3f front = al::getTrans(actor) - hitPos;
    al::normalize(&front);
    al::setFront(actor, front);

    sead::Vector3f* gravity = al::getGravityPtr(actor);
    al::verticalizeVec(gravity, front, *gravity);
    al::normalize(al::getGravityPtr(actor));
}

static inline void updateStick(Motorcycle* actor, IUsePlayerPuppet* playerPuppet, f32* valA, f32 kA,
                               f32 kB, f32 kC) {
    sead::Vector2f stick = {0.0f, 0.0f};
    calcInputStick(&stick, actor, playerPuppet);
    f32 stickH = stick.x * kA;
    f32 leftAngle = al::diffNearAngleDegree(0.0f, rs::getPuppetPoseRotZDegreeLeft(playerPuppet));
    f32 rigthAngle = al::diffNearAngleDegree(0.0f, rs::getPuppetPoseRotZDegreeRight(playerPuppet));

    f32 angle = al::normalizeAbs(
        sead::Mathf::abs(leftAngle) < sead::Mathf::abs(rigthAngle) ? rigthAngle : leftAngle, 45.0f,
        135.0f);

    *valA = al::lerpValue(*valA, stickH + angle * kB, kC);
}

void funE(Motorcycle* actor, IUsePlayerPuppet* playerPuppet, float* valueA,
          SeRumbleState* seRumbleState, bool valueC) {
    updateStick(actor, playerPuppet, valueA, 12.5f, -7.5f, 0.1f);
    sead::Vector2f stick = {0.0f, 0.0f};
    calcInputStick(&stick, actor, playerPuppet);
    al::rotateVectorDegreeY(al::getFrontPtr(actor), stick.x * -0.5f);
    al::normalize(al::getFrontPtr(actor));

    /*fVar8 = al::getVelocity(actor)->x;
    fVar10 = al::getVelocity(actor)->z;
    fVar9 = al::getVelocity(actor)->y;
    uVar5 = rs::isPuppetHoldActionButton(playerPuppet);
    if ((uVar5 & 1) == 0) {
      fVar10 = fVar10 * 0.95f;
      fVar8 = fVar8 * 0.95f;
    }
    puVar6 = (uint *)al::getFront(actor);
    uVar3 = *puVar6;
    lVar7 = al::getFront(actor);
    local_78.z = *(float *)(lVar7 + 8);
    local_78.y = 0.0f;
    local_78.x = (float)uVar3;
    al::tryNormalizeOrZero(&local_78);
    fVar10 = fVar10 * fVar10 + fVar8 * fVar8 + 0.0f;
    fVar8 = SQRT(fVar10);
    if (NAN(fVar8)) {
      fVar8 = sqrtf(fVar10);
    }
    local_88.y = fVar9 + fVar8 * local_78.y;
    local_88.z = fVar8 * local_78.z + 0.0f;
    local_88.x = fVar8 * local_78.x + 0.0f;
    al::setVelocity(actor,&local_88);*/
    al::addVelocityY(actor, -2.0f);
    updateSeRumble(*valueA, seRumbleState, actor, playerPuppet, true);
    if (!funL(actor)) {
        if (!rs::isCollidedWallVelocity(actor, actor) || !checkDashCollision(actor)) {
            updateAirOrientation(actor);
            if (!funO(actor) && !al::isInWater(actor) && valueC)
                al::startHitReaction(actor, "着水");
        }
    }
    return;
}

// TODO: might be moved into `sead`
inline f32 normalize2(sead::Vector3f* v, f32 scalar) {
    const f32 len = v->length();
    if (len > 0) {
        const f32 inv_len = scalar / len;
        v->x *= inv_len;
        v->y *= inv_len;
        v->z *= inv_len;
    }

    return len;
}

void funQ(Motorcycle* actor, f32 value) {
    const sead::Vector3f groundNormal = {0.0f, -1.0f, 0.0f};
    rs::reboundVelocityPart(actor, actor, 0.0f, -0.5f, 0.0f, 0.0f);
    sead::Vector3f velH = {0.0f, 0.0f, 0.0f};
    sead::Vector3f velV = {0.0f, 0.0f, 0.0f};
    al::separateVectorHV(&velH, &velV, actor->getParams()->groundNormal, al::getVelocity(actor));

    sead::Vector3f normal = {0.0f, 0.0f, 0.0f};
    if (rs::isCollidedGround(actor)) {
        MotorcycleParams* params = actor->getParams();
        if (!al::isNormalize(params->floorNormalAvg) || !params->bool_3)
            normal.set(rs::getCollidedGroundNormal(actor));
        else
            normal.set(actor->getParams()->floorNormalAvg);
    } else {
        normal.set(-groundNormal);
        al::verticalizeVec(&normal, al::getFront(actor), normal);
        if (!al::tryNormalizeOrZero(&normal))
            normal.set(-groundNormal);
    }

    velV = -normal * (velV.length() + 3.0f);
    al::limitLength(&velV, velV, rs::isCollidedGround(actor) ? 5.0f : 35.0f);

    if (!al::isNearZero(velH)) {
        f32 length = velH.length();
        al::parallelizeVec(&velH, al::getFront(actor), velH);
        if (!al::isNearZero(velH))
            normalize2(&velH, length);
        sead::Vector3f front = al::getFront(actor);
        al::verticalizeVec(&front, normal, front);
        al::normalize(&front);
        if (!al::isParallelDirection(al::getFront(actor), velH, 0.01f)) {
            sead::Vector3f faceDir = velH;
            al::normalize(&faceDir);
            al::turnVecToVecRate(&front, faceDir, al::getFront(actor), 0.15f);
            al::normalize(&front);
        }
        velH = (front * value + velH) * 0.95f;
    } else {
        velH = (al::getFront(actor) * value) * 0.95f;
    }

    if (rs::isCollidedGround(actor)) {
        const al::CollisionParts* parts = rs::getCollidedGroundCollisionParts(actor);

        sead::Vector3f transformedVelH = velH;
        transformedVelH.rotate(parts->getPrevBaseInvMtx());
        transformedVelH.rotate(parts->getBaseMtx());

        al::verticalizeVec(&transformedVelH, al::getGravity(actor), transformedVelH);
        normalize2(&transformedVelH, velH.length());
        velH.set(transformedVelH);
    }
    al::setVelocity(actor, velH + velV);
}

Motorcycle::Motorcycle(const char* name) : al::LiveActor(name) {}

void Motorcycle::init(const al::ActorInitInfo& info) {
    al::tryGetArg(&mIsOnLight, info, "IsOnLight");
    al::initActorSuffix(this, info, mIsOnLight ? "Night" : nullptr);
    al::initNerve(this, &NrvMotorcycle.Wait, 0);
    al::hideSilhouetteModel(this);
    al::invalidateDepthShadowMap(this);
    al::invalidateHitSensors(this);
    al::validateHitSensorBindableAll(this);
    al::validateHitSensorRideAll(this);
    al::startAction(this, "Wait");
    if (mIsOnLight)
        al::startMclAnim(this, "FrontOnTailOff");
    else
        al::startMclAnim(this, "FrontOffTailOff");
    al::offCollide(this);
    al::calcQuat(&mQuat, this);

    mTrans.set(al::getTrans(this));
    mPlayerAnimator = new MotorcyclePlayerAnimator();
    mAccelerationState = new AccelerationState;
    mParams = new MotorcycleParams;
    mSeRumbleState = new SeRumbleState;
    mParkingParams = new ParkingParams;

    al::initJointControllerKeeper(this, 6);
    al::initJointLocalZRotator(this, &steerAngle, "AllRoot");
    al::initJointLocalXRotator(this, &handleAngle, "Handle");
    al::initJointLocalYRotator(this, &handleAngle, "FrontWheel");
    al::initJointLocalYRotator(this, &handleAngle, "Cowl");
    al::initJointLocalXRotator(this, &mLeanAngle, "AllRoot");
    al::initJointLocalXRotator(this, &mJumpAngle, "JointRoot");

    CollisionShapeKeeper* collision = new CollisionShapeKeeper(7, 0x40, 0x10);
    collision->createShapeArrow("FrontCenter", forceField, forceField6, 20.0f, 0);
    collision->createShapeArrow("FrontLeft", forceField2, forceField6, 20.0f, 1);
    collision->createShapeArrow("FrontRight", forceField3, forceField6, 20.0f, 2);

    collision->createShapeSphereSupportGround("FrontFace", 60.0f, {0.0f, 80.0f, -35.0f},
                                              sead::Vector3f::ey, 20.0f);
    collision->createShapeSphereSupportGround("BackFace", 60.0f, forceField7, sead::Vector3f::ey,
                                              20.0f);
    collision->createShapeSphereSupportGround("CenterFace", 60.0f, forceField8, sead::Vector3f::ey,
                                              20.0f);

    collision->createShapeSphereIgnoreGround("Head", 40.0f, {0.0f, 155.0f, -80.0f});
    collision->updateShape();

    PlayerCollider* collider = new PlayerCollider(getCollisionDirector(), getBaseMtx(),
                                                  al::getTransPtr(this), &verticalUp, false);
    collider->setCollisionShapeKeeper(collision);
    collider->onInvalidate();
    mPlayerCollider = collider;
    rs::createAndSetColliderFilterSpecialPurpose(this, "MoveLimit");

    mColliderCameraTarget = new MotorcycleColliderCameraTarget(this);
    mTransCameraSubTarget = al::createTransCameraSubTarget("バイク進行方向", &vectorA);
    mCameraSubTargetTurnParam = new al::CameraSubTargetTurnParam();
    al::initCameraSubTargetTurnParam(mTransCameraSubTarget, mCameraSubTargetTurnParam);

    mCollisionPartsConnector = al::tryCreateCollisionPartsConnector(this, info, mQuat);
    mBindKeepDemoInfo = rs::initBindKeepDemoInfo();
    al::tryGetArg(&mIsStickWorldPose, info, "IsStickWorldPose");

    mCoursePointSize = al::calcLinkChildNum(info, "CoursePoint");
    if (mCoursePointSize > 0) {
        mCoursePoints = new sead::Vector3f[mCoursePointSize];
        for (s32 i = 0; i < mCoursePointSize; i++)
            al::getChildLinkT(&mCoursePoints[i], info, "CoursePoint", i);
    }

    al::setEffectNamedMtxPtr(this, "WaterSurface", &mWaterSurfaceMtx);
    al::setSeUserSyncParamPtr(this, &mSeRumbleState->volume, "スピード");
    makeActorAlive();
}

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
                            al::HitSensor* self) {
    if (rs::isMsgTargetMarkerPosition(message)) {
        sead::Vector3f position = {0.0f, 0.0f, 0.0f};
        al::calcTransLocalOffset(&position, this, forceField5);
        rs::setMsgTargetMarkerPosition(message, position);
        return true;
    }

    if (rs::isMsgIgnoredByRunawayNpc(message))
        return true;

    if (al::isSensorPlayer(self)) {
        if (rs::isMsgPushToMotorcycle(message)) {
            rs::calcNpcPushVecBetweenSensors(&vector2, other, self);
            return true;
        }

        if (al::isSensorNpc(other) &&
            (al::isMsgPushAll(message) || rs::isMsgPushToPlayer(message))) {
            sead::Vector3f sideDir = {0.0f, 0.0f, 0.0f};
            al::calcSideDir(&sideDir, this);
            sead::Vector3f sensorDistance = al::getSensorPos(self) - al::getSensorPos(other);
            al::parallelizeVec(&sensorDistance, sideDir, sensorDistance);
            f32 radius = sead::Mathf::max(al::getSensorRadius(other), al::getSensorRadius(self)) -
                         sensorDistance.length();
            if (radius > 0.0f && al::tryNormalizeOrZero(&sensorDistance)) {
                vector2 += sead::Mathf::min(radius, 10.0f) * sensorDistance;
                al::limitLength(&vector2, vector2, radius);

                return true;
            }
        }
    }

    if (rs::isMsgKillByShineGet(message) || rs::isMsgKillByHomeDemo(message)) {
        if (mPlayerPuppet) {
            rs::endBindAndPuppetNull(&mPlayerPuppet);
            resetState(this, mPlayerAnimator, mColliderCameraTarget, mTransCameraSubTarget);
        }
        al::setVelocityZero(this);
        al::invalidateHitSensors(this);
        al::hideModelIfShow(this);
        al::setNerve(this, &NrvMotorcycle.Reset);
        al::setNerve(this, &NrvMotorcycle.ResetNoReaction);
        return true;
    }

    if (mPlayerPuppet) {
        if (rs::isActiveBindKeepDemo(mBindKeepDemoInfo)) {
            if (rs::isMsgBindKeepDemoExecute(message)) {
                mPlayerAnimator->tryStartBindRideRunIfNotPlaying();
                mPlayerAnimator->update(0.0f, 1.0f, 0.0f);
                if (mPlayerPuppet)
                    syncPuppetPose(mPlayerPuppet, this, mStartQuat, mStartTrans);
                _249 = false;
                return true;
            }

            if (rs::tryEndBindKeepDemo(mBindKeepDemoInfo, message, mPlayerPuppet)) {
                mPlayerAnimator->tryStartBindRideRunIfNotPlaying();
                _249 = false;
                if (al::isNerve(this, &NrvMotorcycle.RideStartLeft) ||
                    al::isNerve(this, &NrvMotorcycle.RideStartRight) ||
                    al::isNerve(this, &NrvMotorcycle.RideStartOn)) {
                    al::setNerve(this, &NrvMotorcycle.RideWait);
                    return true;
                }
                return true;
            }
        } else if (rs::tryStartBindKeepDemo(mBindKeepDemoInfo, message, mPlayerPuppet)) {
            mPlayerAnimator->startBindKeepDemo();
            _249 = true;
            return true;
        }

        if (al::isMsgPlayerDisregard(message))
            return true;

        if (rs::isMsgEnableMapCheckPointWarp(message) || rs::isMsgBindCollidedGround(message))
            return rs::isCollidedGround(this);

        if (!al::isSensorName(self, "Pushed") && al::isSensorPlayer(self) &&
            (al::isMsgExplosion(message) || rs::isMsgEnemyAttackTRex(message) || !isRideRun_()) &&
            al::isMsgEnemyAttack(message)) {
            return rs::requestDamage(mPlayerPuppet);
        }

        if (rs::tryReceiveBindCancelMsgAndPuppetNull(&mPlayerPuppet, message)) {
            resetState(this, mPlayerAnimator, mColliderCameraTarget, mTransCameraSubTarget);
            return true;
        }
        if (al::isMsgBindEnd(message)) {
            startGetOff(this, &mPlayerPuppet, &_23c, mPlayerAnimator, mColliderCameraTarget,
                        mTransCameraSubTarget);
            return true;
        }
        return rs::isMsgBindRecoveryLife(message) && al::isNerve(this, &NrvMotorcycle.RideWait);
    }

    if (rs::isMsgCheckCarObstacle(message))
        return true;

    if (rs::isMsgFireSwitchFire(message)) {
        al::setVelocityZero(this);
        al::invalidateHitSensors(this);
        al::hideModelIfShow(this);
        al::setNerve(this, &NrvMotorcycle.Reset);
        return true;
    }

    if (!al::isSensorRide(self)) {
        if (!al::isSensorBindableAll(self))
            return false;
        if (rs::isPlayer2D(this))
            return false;
        if (al::isMsgBindStart(message) &&
            (al::isNerve(this, &NrvMotorcycle.Wait) || al::isNerve(this, &NrvMotorcycle.Creep) ||
             al::isNerve(this, &NrvMotorcycle.Reaction))) {
            if (!rs::isPlayerOnGround(this)) {
                if (al::getActorVelocity(other).y >= -6.0f) {
                    sead::Vector3f velocity = al::getActorVelocity(other);
                    if (!al::tryNormalizeOrZero(&velocity) || velocity.y > 0.0f) {
                        if (_23c > 0) {
                            _23c = 5;
                            return false;
                        }
                    } else if (_23c < 1) {
                        sead::Vector3f dits = al::getActorTrans(other) - al::getTrans(this);
                        al::parallelizeVec(&dits, al::getFront(this), dits);
                        f32 distLimit = (al::getFront(this).dot(dits) <= 0.0f ? 200.0f : 50.0f);
                        if (dits.length() <= distLimit) {
                            if (al::calcDistanceV(al::getGravity(this), other, self) <= 100.0f)
                                return true;
                        }
                    }
                }
                return false;
            }

            return rs::isPlayerInputTriggerRide(this);
        }
        if (!al::isMsgBindInit(message))
            return false;
        mPlayerPuppet = rs::startPuppet(self, other);
        rs::validateRecoveryArea(mPlayerPuppet);
        mPlayerAnimator->startBind(mPlayerPuppet);
        rs::setPuppetVelocity(mPlayerPuppet, sead::Vector3f::zero);
        bool uVar4 = rs::isCollidedPuppetGround(mPlayerPuppet);
        al::validateDepthShadowMap(this);
        al::offDepthShadowModel(this);
        rs::hidePuppetShadow(mPlayerPuppet);
        rs::invalidatePuppetCollider(mPlayerPuppet);
        rs::invalidatePuppetReceivePush(mPlayerPuppet);
        rs::invalidateWaterSurfaceShadow(mPlayerPuppet);
        al::setCameraTarget(this, mColliderCameraTarget);
        al::onCameraRideObj(this);
        if (mIsStickWorldPose == false) {
            al::setCameraSubTarget(this, mTransCameraSubTarget);
            floatD = 0.0f;
            _19c = 0;
        }
        al::invalidateHitSensors(this);
        al::validateHitSensorPlayerAll(this);
        al::validateHitSensorEnemyBodyAll(this);
        al::invalidateClipping(this);
        al::onCollide(this);
        al::showSilhouetteModel(this);
        al::invalidateOcclusionQuery(this);
        mAccelerationState->isAccelerating = false;
        mAccelerationState->accelRate = 0.0f;
        valA = 1200;
        rs::calcPuppetQuat(&mStartQuat, mPlayerPuppet);
        mStartTrans.set(rs::getPuppetTrans(mPlayerPuppet));
        rs::tryAppearBindTutorial(this, {"Motorcycle"});
        rs::rideMotorcycle(this);
        _23css = rs::getPuppetPoseRotZDegreeLeft(mPlayerPuppet);
        _23cxs = rs::getPuppetPoseRotZDegreeRight(mPlayerPuppet);
        if (!uVar4) {
            al::setNerve(this, &NrvMotorcycle.RideStartOn);
            return true;
        }
        sead::Vector3f dirH = {0.0f, 0.0f, 0.0f};
        if (al::calcDirBetweenSensorsH(&dirH, self, other)) {
            sead::Vector3f sideDir = {0.0f, 0.0f, 0.0f};
            al::calcSideDir(&sideDir, this);
            if (sideDir.dot(dirH) > 0.0f) {
                al::setNerve(this, &NrvMotorcycle.RideStartLeft);
                return true;
            }
        }
        al::setNerve(this, &NrvMotorcycle.RideStartRight);
        return true;
    }

    if (rs::isMsgPlayerObjectWallHit(message))
        return false;

    if (!rs::isMsgNpcCapReactionAll(message) && !al::isMsgExplosion(message)) {
        if (!rs::isPlayerHack(this))
            return false;
        if (!rs::checkMsgNpcTrampleReactionAll(message, other, self, false))
            return false;
    }

    rs::requestHitReactionToAttacker(message, self, other);

    if (al::isNerve(this, &NrvMotorcycle.Wait) || al::isNerve(this, &NrvMotorcycle.Creep) ||
        al::isNerve(this, &NrvMotorcycle.Reaction)) {
        if (al::isNerve(this, &NrvMotorcycle.Reaction) && al::isLessStep(this, 7))
            return false;

        if (_240 > 0) {
            sead::Vector3f itemPos = {0.0f, 150.0f, -90.0f};
            al::multVecPose(&itemPos, this, itemPos);
            sead::Quatf itemQuat = sead::Quatf::unit;
            al::calcQuat(&itemQuat, this);
            al::appearItem(this, itemPos, itemQuat, nullptr);
            _240--;
        }
        al::setNerve(this, &NrvMotorcycle.Reaction);
    }
    return true;
}

void Motorcycle::movement() {
    if (rs::isActiveBindKeepDemo(mBindKeepDemoInfo)) {
        mPlayerAnimator->update(0.0f, 1.0f, 0.0f);
        return;
    }

    mAccelerationState->isAccelerating = false;
    al::LiveActor::movement();

    if (mPlayerPuppet) {
        rs::syncPuppetVisibility(this, mPlayerPuppet);
        rs::copyPuppetDitherAlpha(mPlayerPuppet, this);
        al::AreaObj* areaObj =
            al::tryFindAreaObj(this, "MotorcycleInCheckArea", al::getTrans(this));
        if (areaObj) {
            al::tryOnStageSwitch(areaObj, "SwitchAreaOn");
            areaObj->invalidate();
        }
        if (mParams->framesInAir >= 60)
            rs::requestUpToTargetCameraAngleBySpeed(this, 50.0f, 3.0f, 0);
        rs::requestUpdateSafetyPoint(mPlayerPuppet, this, this);
        if (rs::isCollidedGround(this))
            rs::sendPlayerCollisionTouchMsg(this, al::getHitSensor(this, "PlayerBody"), this);
        if (rs::isCollidedGround(this))
            al::holdSe(this, "GroundNoiseLv");
    }

    /*if (!al::isActiveCameraSubTarget(mTransCameraSubTarget))
        goto LAB_71002c8d1c;
    if (_198 < 1 || !al::isOnStageSwitch(this, "SwitchValidCoursePoint"), )) {
            fVar25 = 0.0f;
            fVar27 = al::lerpValue(floatD, 0.0f, 0.045f);
            iVar21 = _19c;
            fVar26 = fVar25;
        LAB_71002c8ba4:
            iVar17 = 0;
            if (-1 < iVar21 + -1)
                iVar17 = iVar21 + -1;
        }
    else {
        if (*(int*)&field21_0x198 < 1) {
            fVar25 = 0.0f;
            fVar26 = fVar25;
        LAB_71002c8b88:
            fVar27 = al::lerpValue(floatD, 0.0f, 0.045f);
            iVar21 = _19c;
            goto LAB_71002c8ba4;
        }
        fVar27 = 180.0f;
        fVar25 = 0.0f;
        lVar20 = 0;
        lVar22 = 0;
        iVar21 = -1;
        fVar26 = fVar25;
        do {
            pVVar9 = al::getTrans(this);
            pfVar16 = (float*)(field20_0x190 + lVar20);
            fVar28 = pVVar9->y - pfVar16[1];
            fVar24 = pVVar9->x - *pfVar16;
            fVar29 = pVVar9->z - pfVar16[2];
            fVar24 = fVar24 * fVar24 + fVar28 * fVar28 + fVar29 * fVar29;
            fVar28 = SQRT(fVar24);
            if (NAN(fVar28))
                fVar28 = sqrtf(fVar24);
            if ((750.0f <= fVar28) && (fVar28 <= 3000.0f)) {
                local_140 = (undefined1[8])0x0;
                local_138 = (KCPrismData*)((ulong)local_138 & 0xffffffff00000000);
                al::calcCameraLookDir(local_140, this, 0);
                lVar23 = field20_0x190;
                pVVar9 = al::getTrans(this);
                pfVar16 = (float*)(lVar23 + lVar20);
                local_b0.y = pfVar16[1] - pVVar9->y;
                local_b0.x = *pfVar16 - pVVar9->x;
                local_b0.z = pfVar16[2] - pVVar9->z;
                fVar24 = (float)al::calcAngleDegree(local_140, &local_b0);
                if ((fVar24 <= 120.0f) && ((iVar21 < 0 || (fVar24 < fVar27)))) {
                    fVar25 = (float)al::normalize(fVar28, 750.0f, 3000.0f);
                    fVar26 = (float)al::normalize(fVar24, 0.0f, 120.0f);
                    iVar21 = (int)lVar22;
                    fVar27 = fVar24;
                }
            }
            lVar22 = lVar22 + 1;
            lVar20 = lVar20 + 0xc;
        } while (lVar22 < *(int*)&field21_0x198);
        if (iVar21 < 0)
            goto LAB_71002c8b88;
        pVVar9 = al::getFront(this);
        lVar22 = field20_0x190;
        pVVar12 = al::getTrans(this);
        pfVar16 = (float*)(lVar22 + (long)iVar21 * 0xc);
        local_140._4_4_ = pfVar16[1] - pVVar12->y;
        local_140._0_4_ = *pfVar16 - pVVar12->x;
        local_138 = (KCPrismData*)CONCAT44(local_138._4_4_, pfVar16[2] - pVVar12->z);
        pVVar12 = al::getGravity(this);
        fVar24 = (float)al::calcAngleOnPlaneDegree(pVVar9, local_140, pVVar12);
        fVar27 = -50.0f;
        if ((-50.0f <= fVar24) && (fVar27 = fVar24, 50.0f < fVar24))
            fVar27 = 50.0f;
        fVar27 = al::lerpValue(floatD, fVar27, 0.3f);
        fVar27 = al::lerpValue(floatD, fVar27, 0.15f);
        iVar21 = _19c;
        iVar17 = 30;
        if (iVar21 + 1 < 0x1f)
            iVar17 = iVar21 + 1;
    }
    floatD = fVar27;
    _19c = iVar17;
    if (al::isNerve(this, &NrvMotorcycle.RideWait)) {
        mCameraSubTargetTurnParam->mTurnSpeedRate1 =
            mCameraSubTargetTurnParam->mTurnSpeedRate1 * 0.98f;
        mCameraSubTargetTurnParam->mTurnSpeedRate2 =
            mCameraSubTargetTurnParam->mTurnSpeedRate2 * 0.98f;
    } else {
        f32 fVar27 = al::normalize(al::calcSpeedH(this), 0.0f, al::calcSpeedMax(1.8f, 0.95f));
        f32 fVar24 = (float)al::normalize(_19c, 0, 30);
        fVar26 = fVar26 * fVar25 * fVar24;
        fVar25 = al::lerpValue(0.3, 0.1, fVar27);
        mCameraSubTargetTurnParam->mTurnSpeedRate1 = al::lerpValue(fVar25, 0.275f, fVar26);
        fVar25 = al::lerpValue(0.15, 0.075, fVar27);
        mCameraSubTargetTurnParam->mTurnSpeedRate2 = al::lerpValue(fVar25, 0.1, fVar26);
    }
    puVar13 = (undefined8*)al::getFront(this);
    local_138 = (KCPrismData*)CONCAT44(local_138._4_4_, *(undefined4*)(puVar13 + 1));
    local_140 = (undefined1[8]) * puVar13;
    pVVar9 = al::getGravity(this);
    al::rotateVectorDegree(local_140, local_140, pVVar9, floatD);
    pVVar9 = al::getTrans(this);
    fVar25 = pVVar9->y;
    fVar26 = pVVar9->z;
    (VectorA).x = (float)local_140._0_4_ * 250.0f + pVVar9->x;
    (VectorA).y = (float)local_140._4_4_ * 250.0f + fVar25;
    (VectorA).z = local_138._0_4_ * 250.0f + fVar26;
LAB_71002c8d1c:
    params = mParams;
    if (0 < (params->backContactPoints).size) {
        lVar22 = 0;
        puVar13 = (undefined8*)params->field14_0x28[0];
        do {
            puVar19 = (params->backContactPoints).frontContactPoints[lVar22];
            *puVar19 = puVar13;
            iVar21 = (params->backContactPoints).size;
            lVar22 = lVar22 + 1;
            params->field14_0x28[0] = puVar19;
            puVar13 = puVar19;
        } while (lVar22 < iVar21);
    }
    (params->backContactPoints).size = 0;
    if (0 < (params->frontContactPoints).size) {
        lVar22 = 0;
        puVar13 = (undefined8*)params->field16_0x648[0];
        do {
            puVar19 = (params->frontContactPoints).frontContactPoints[lVar22];
            *puVar19 = puVar13;
            iVar21 = (params->frontContactPoints).size;
            lVar22 = lVar22 + 1;
            params->field16_0x648[0] = puVar19;
            puVar13 = puVar19;
        } while (lVar22 < iVar21);
    }
    (params->frontContactPoints).size = 0;
    (params->floorNormalAvg).x = 0.0f;
    (params->floorNormalAvg).y = 0.0f;
    (params->floorNormalAvg).z = 0.0f;
    mPlayerPuppet = &this;
    if (!al::isNoCollide(this)) {
        params->isOnJump = false;
        params->bool_3 = false;
        params->bool_4 = false;
        params->bool_5 = false;
        uVar8 = rs::isCollidedGround(this);
        if ((uVar8 & 1) != 0) {
            lVar22 = (*(code*)((actor).vtable)->field28_0xe0)(this);
            this_01 = *(CollisionShapeKeeper**)(lVar22 + 0xf0);
            iVar21 = this_01->mNumCollideResult;
            if (0 < iVar21) {
                iVar17 = 0;
                do {
                    this_00 =
(CollidedShapeResult*)CollisionShapeKeeper::getCollidedShapeResult( this_01, iVar17); uVar8 =
CollidedShapeResult::isArrow(this_00); if ((uVar8 & 1) == 0) { uVar8 =
CollidedShapeResult::isSphere(this_00); if ((uVar8 & 1) != 0) { pTVar14 =
(Triangle*)CollidedShapeResult::getSphereHitInfo(this_00); puVar13 =
(undefined8*)al::Triangle::getNormal(pTVar14, 0); local_138 =
(KCPrismData*)CONCAT44(local_138._4_4_,
                                                               *(undefined4*)(puVar13 + 1));
                            local_140 = (undefined1[8]) * puVar13;
                            lVar22 = CollidedShapeResult::getShapeInfoSphere(this_00);
                            bVar5 = al::isEqualString("FrontFace", *(char**)(lVar22 + 0x10));
                            if (bVar5) {
                                if ((params->backContactPoints).size <
(params->backContactPoints).sizeMax) { puVar13 = (undefined8*)params->field14_0x28[0]; if
(puVar13
!= (undefined8*)0x0) params->field14_0x28[0] = *puVar13;
                                    *(float*)(puVar13 + 1) = local_138._0_4_;
                                    *puVar13 = local_140;
                                    uVar18._0_4_ = (params->backContactPoints).size;
                                    uVar18._4_4_ = (params->backContactPoints).sizeMax;
                                    if ((int)(undefined4)uVar18 < (int)uVar18._4_4_) {
                                        *(undefined8**)((long)(params->backContactPoints).frontContactPoints
+
                                                        (-(uVar18 >> 0x1f & 1) &
                                                             0xfffffff800000000 |
                                                         (uVar18 & 0xffffffff) << 3)) = puVar13;
                                        (params->backContactPoints).size =
(params->backContactPoints).size + 1;
                                    }
                                }
                                uVar8 = al::isFloorPolygon(local_140, &verticalUp);
                                if ((uVar8 & 1) != 0)
                                    params->bool_4 = true;
                            } else {
                                lVar22 = CollidedShapeResult::getShapeInfoSphere(this_00);
                                bVar5 = al::isEqualString("BackFace", *(char**)(lVar22 + 0x10));
                                if (bVar5) {
                                    if ((params->frontContactPoints).size <
(params->frontContactPoints).sizeMax) { puVar13 = (undefined8*)params->field16_0x648[0]; if
(puVar13
!= (undefined8*)0x0) params->field16_0x648[0] = *puVar13;
                                        *(float*)(puVar13 + 1) = local_138._0_4_;
                                        *puVar13 = local_140;
                                        uVar8._0_4_ = (params->frontContactPoints).size;
                                        uVar8._4_4_ = (params->frontContactPoints).sizeMax;
                                        if ((int)(undefined4)uVar8 < (int)uVar8._4_4_) {
                                            *(undefined8**)((long)(params->frontContactPoints).frontContactPoints
+
                                                            (-(uVar8 >> 0x1f & 1) &
                                                                 0xfffffff800000000 |
                                                             (uVar8 & 0xffffffff) << 3)) =
puVar13; (params->frontContactPoints).size = (params->frontContactPoints).size + 1;
                                        }
                                    }
                                    uVar8 = al::isFloorPolygon(local_140, &verticalUp);
                                    if ((uVar8 & 1) != 0)
                                        params->bool_5 = true;
                                }
                            }
                        }
                    } else {
                        pTVar14 = (Triangle*)CollidedShapeResult::getArrowHitInfo(this_00);
                        pVVar9 = al::Triangle::getNormal(pTVar14, 0);
                        uVar8 = al::isFloorPolygon(pVVar9, &verticalUp);
                        if ((uVar8 & 1) != 0) {
                            lVar22 = CollidedShapeResult::getShapeInfoArrow(this_00);
                            if (*(int*)(lVar22 + 0x18) == 0) {
                                params->isOnJump = true;
                                pTVar14 =
(Triangle*)CollidedShapeResult::getArrowHitInfo(this_00); pfVar16 =
(float*)al::Triangle::getNormal(pTVar14, 0); (params->floorNormalAvg).x =
(params->floorNormalAvg).x
+ *pfVar16; (params->floorNormalAvg).y = (params->floorNormalAvg).y + pfVar16[1];
(params->floorNormalAvg).z = (params->floorNormalAvg).z + pfVar16[2]; } else { params->bool_3 =
true;
                            }
                        }
                    }
                    iVar17 = iVar17 + 1;
                } while (iVar21 != iVar17);
            }
        }
        al::tryNormalizeOrZero(&params->floorNormalAvg);
    }
    uVar8 = rs::isCollidedGround(this);
    if ((uVar8 & 1) != 0) {
        params = mParams;
        uVar8 = al::isNormalize(&params->floorNormalAvg, 0.001);
        if ((((uVar8 & 1) != 0) && (params->isOnJump != false)) && (mParams->bool_3 == false)) {
            local_b0.x = 0.0f;
            local_b0.y = 0.0f;
            local_b0.z = 0.0f;
            pfVar16 = (float*)al::getFront(this);
            fVar29 = pfVar16[2];
            fVar27 = *pfVar16;
            fVar28 = pfVar16[1];
            al::Triangle::Triangle((Triangle*)local_140);
            pVVar9 = al::getTrans(this);
            fVar26 = pVVar9->x;
            fVar24 = pVVar9->y;
            fVar25 = pVVar9->z;
            pVVar9 = al::getGravity(this);
            local_c0.y = (fVar28 * -300.0f * 0.5f + fVar24) - pVVar9->y * 100.0f;
            local_c0.x = (fVar27 * -300.0f * 0.5f + fVar26) - pVVar9->x * 100.0f;
            local_c0.z = (fVar29 * -300.0f * 0.5f + fVar25) - pVVar9->z * 100.0f;
            pVVar9 = al::getGravity(this);
            fVar26 = pVVar9->x * 100.0f;
            fVar25 = pVVar9->y * 100.0f;
            local_d0.z = pVVar9->z * 100.0f;
            local_d0.y = fVar25 + fVar25;
            local_d0.x = fVar26 + fVar26;
            local_d0.z = local_d0.z + local_d0.z;
            bVar5 = alCollisionUtil::getFirstPolyOnArrow(
                (IUseCollision*)&(actor).collisionDirector, &local_b0, (Triangle*)local_140,
                &local_c0, &local_d0, (CollisionPartsFilterBase*)0x0, (TriangleFilterBase*)0x0);
            if (bVar5) {
                pVVar9 = al::Triangle::getNormal((Triangle*)local_140, 0);
                uVar8 = al::isFloorPolygon(pVVar9, &verticalUp);
                if ((uVar8 & 1) != 0) {
                    pVVar9 = al::getTrans(this);
                    uVar8 = al::isNear(&local_b0, pVVar9, 0.001);
                    if ((uVar8 & 1) == 0) {
                        pVVar9 = al::getTrans(this);
                        local_c0.y = local_b0.y - pVVar9->y;
                        local_c0.x = local_b0.x - pVVar9->x;
                        local_c0.z = local_b0.z - pVVar9->z;
                        al::normalize(&local_c0);
                        pVVar9 = al::getFrontPtr(this);
                        local_d0.y = -local_c0.y;
                        local_d0.x = -local_c0.x;
                        local_d0.z = -local_c0.z;
                        al::turnVecToVecRate(pVVar9, pVVar9, &local_d0, 0.3f);
                        pVVar9 = al::getFrontPtr(this);
                        al::normalize(pVVar9);
                        pVVar9 = al::getGravityPtr(this);
                        pVVar12 = al::getFront(this);
                        al::verticalizeVec(pVVar9, pVVar12, pVVar9);
                        pVVar9 = al::getGravityPtr(this);
                        al::normalize(pVVar9);
                    }
                }
            }
        }
    }
    fVar25 = 0.0f;
    fVar26 = (float)al::normalizeAbs(steerAngle, 0.0f, 55.0f);
    fVar26 = fVar26 * -32.5;
    handleAngle = fVar26;
    if (mPlayerPuppet == (IUsePlayerPuppet*)0x0) {
        fVar24 = 1.0f;
        fVar27 = fVar25;
    } else {
        fVar27 = 0.0f;
        fVar24 = fVar27;
        if (0.0f < fVar26) {
            fVar24 = (float)al::normalize(fVar26, 0.0f, 32.5);
            fVar26 = handleAngle;
        }
        if (fVar26 < 0.0f) {
            fVar27 = (float)al::normalize(fVar26, -32.5, 0.0f);
            fVar27 = 1.0f - fVar27;
        }
        fVar25 = 1.0f - (fVar24 + fVar27);
    }
    mPlayerAnimator->update( fVar24, fVar25, fVar27);
    if (!rs::isCollidedGround(this))
        pcVar15 = "NoCollide";
    else
        pcVar15 = (char*)rs::getMaterialCodeGround(this);
    al::setMaterialCode(this, pcVar15);
    al::makeMtxSRT(&mtx, this);
    if (mPlayerPuppet || !al::isInWater(this)) {
        al::updateMaterialCodePuddle(this, false);
        al::updateMaterialCodeWater(this, false);
    } else {
        local_140 = (undefined1[8])0x0;
        local_138 = (KCPrismData*)((ulong)local_138 & 0xffffffff00000000);
        local_b0.x = 0.0f;
        local_b0.y = 0.0f;
        local_b0.z = 0.0f;
        local_c0.x = 0.0f;
        local_c0.y = 0.0f;
        local_c0.z = 0.0f;
        local_d0.x = 0.0f;
        local_d0.y = 0.0f;
        local_d0.z = 0.0f;
        al::calcUpDir(&local_d0, this);
        pVVar9 = al::getTrans(this);
        uVar6 =
            al::calcFindWaterSurfaceFlat(local_140, 0x0, this, pVVar9, &sead::Vector3f::ey,
121.0f); pVVar9 = al::getTrans(this); uVar7 = al::calcFindWaterSurface(&local_b0, &local_c0,
this, pVVar9, &sead::Vector3f::ey, 121.0f); if (((uVar6 & 1) == 0) && ((uVar7 & 1) == 0)) {
            fVar26 = 0.0f;
            fVar25 = 0.0f;
            fVar27 = 0.0f;
            bVar5 = false;
        } else {
            if ((uVar6 & uVar7 & 1) == 0) {
                if ((uVar6 & 1) == 0) {
                    fVar26 = local_b0.z;
                    fVar25 = local_b0.y;
                    fVar27 = local_b0.x;
                } else {
                    fVar26 = local_138._0_4_;
                    fVar25 = (float)local_140._4_4_;
                    fVar27 = (float)local_140._0_4_;
                }
            } else {
                pVVar9 = al::getTrans(this);
                fVar26 = (float)local_140._0_4_ - pVVar9->x;
                fVar25 = (float)local_140._4_4_ - pVVar9->y;
                fVar27 = local_138._0_4_ - pVVar9->z;
                fVar26 = fVar26 * fVar26 + fVar25 * fVar25 + fVar27 * fVar27;
                fVar25 = SQRT(fVar26);
                if (NAN(fVar25))
                    fVar25 = sqrtf(fVar26);
                pVVar9 = al::getTrans(this);
                fVar26 = local_b0.x - pVVar9->x;
                fVar27 = local_b0.y - pVVar9->y;
                fVar24 = local_b0.z - pVVar9->z;
                fVar27 = fVar26 * fVar26 + fVar27 * fVar27 + fVar24 * fVar24;
                fVar26 = SQRT(fVar27);
                if (NAN(fVar26))
                    fVar26 = sqrtf(fVar27);
                pVVar9 = &local_b0;
                if (fVar26 <= fVar25)
                    pVVar9 = local_140;
                fVar26 = pVVar9->z;
                fVar25 = pVVar9->y;
                fVar27 = pVVar9->x;
            }
            bVar5 = true;
        }
        if (bVar5) {
            al::updateMaterialCodePuddle(this, true);
            al::updateMaterialCodeWater(this, false);
            (mtx).matrix[0][3] = fVar27;
            (mtx).matrix[1][3] = fVar25;
            (mtx).matrix[2][3] = fVar26;
        } else {
            startGetOff(this, &mPlayerPuppet, &field33_0x23c, (PlayerAnimator*)mPlayerAnimator,
                 (TransCameraSubTarget*)mColliderCameraTarget, mTransCameraSubTarget);
            al::setVelocityZero(this);
            al::invalidateHitSensors(this);
            al::hideModelIfShow(this);
            al::setNerve(this, &NrvMotorcycle.Reset);
        }
    }
    field33_0x23c = field33_0x23c + -1;
    bVar5 = field36_0x245;
    bVar4 = false;
    if (mPlayerPuppet)
        bVar4 = *(char*)field6_0x130 == '\0';
    field36_0x245 = bVar4;
    if (bVar5 != bVar4) {
        pcVar15 = "FrontOnTailOn";
        if (bVar4 == false)
            pcVar15 = "FrontOnTailOff";
        pcVar3 = "FrontOffTailOn";
        if (bVar4 == false)
            pcVar3 = "FrontOffTailOff";
        if (mIsOnLight == false)
            pcVar15 = pcVar3;
        al::startMclAnim(this, pcVar15);
    }*/
}

void Motorcycle::kill() {
    al::LiveActor::kill();
    al::onDepthShadowModel(this);
    al::invalidateDepthShadowMap(this);
}

void Motorcycle::updateCollider() {
    if (rs::isActiveBindKeepDemo(mBindKeepDemoInfo))
        return;
    MotorcycleParams* params = mParams;
    if (al::isNoCollide(this)) {
        params->isOnGround = false;
        params->framesInAir = -1;
        params->isOnJump = false;
    } else {
        params->isOnGround = rs::isCollidedGround(this);
        params->isOnJump = rs::isCollisionCodeJump(this);
        if (params->isOnGround) {
            params->framesInAir = 0;
            params->lastGroundPos.set(al::getTrans(this));
            params->groundNormal.set(rs::getCollidedGroundNormal(this));
        } else
            params->framesInAir++;
    }

    _248 = al::isInWater(this);
    vector4 = -al::getTrans(this);

    rs::updateCollider(this, this, al::getVelocity(this) + vector2 + vector5);
    vector2 = {0.0f, 0.0f, 0.0f};
    vector5 = {0.0f, 0.0f, 0.0f};
    vector4 += al::getTrans(this);
}

void Motorcycle::calcAnim() {
    al::LiveActor::calcAnim();
    if (mPlayerPuppet && !_249)
        syncPuppetPose(mPlayerPuppet, this, mStartQuat, mStartTrans);
}

void Motorcycle::exeWait() {
    if (al::isFirstStep(this)) {
        al::tryStartActionIfNotPlaying(this, "Wait");
        al::setVelocityZero(this);
    }
    steerAngle = al::lerpValue(steerAngle, 0.0f, 0.2f);
    handleAngle = al::lerpValue(handleAngle, 0.0f, 0.2f);
    mLeanAngle = al::lerpValue(mLeanAngle, 0.0f, 0.2f);
    mJumpAngle = al::lerpValue(mJumpAngle, 0.0f, 0.2f);

    if (al::isNoCollide(this)) {
        if (mCollisionPartsConnector) {
            if (!al::isMtxConnectorConnecting(mCollisionPartsConnector)) {
                kill();
                return;
            }
            sead::Quatf quat = sead::Quatf::unit;
            sead::Vector3f trans = {0.0f, 0.0f, 0.0f};
            al::calcConnectQT(&quat, &trans, mCollisionPartsConnector);
            al::resetQuatPosition(this, quat, trans);
        }
    } else {
        updateOrientation(steerAngle, this);
        al::setVelocityToGravity(this, 2.0f);
        if (!rs::isCollidedGround(this) && al::isGreaterEqualStep(this, 2)) {
            al::setNerve(this, &NrvMotorcycle.Fall);
            return;
        }
    }
    if (0 < valA && --valA == 0) {
        al::setVelocityZero(this);
        al::invalidateHitSensors(this);
        al::hideModelIfShow(this);
        al::setNerve(this, &NrvMotorcycle.Reset);
    }
}

void Motorcycle::exeCreep() {
    if (al::isFirstStep(this))
        al::tryStartActionIfNotPlaying(this, "Wait");

    steerAngle = al::lerpValue(steerAngle, 0.0f, 0.2f);
    handleAngle = al::lerpValue(handleAngle, 0.0f, 0.2f);
    mLeanAngle = al::lerpValue(mLeanAngle, 0.0f, 0.2f);
    mJumpAngle = al::lerpValue(mJumpAngle, 0.0f, 0.2f);
    updateOrientation(steerAngle, this);

    al::addVelocityToGravity(this, 2.0f);
    al::scaleVelocity(this, 0.9f);
    al::limitVelocityDir(this, al::getGravity(this), 360.0f);

    if (!rs::isCollidedGround(this) && al::isGreaterEqualStep(this, 2)) {
        al::setNerve(this, &NrvMotorcycle.Fall);
        return;
    }

    if (al::isInDeathArea(this) || al::isGreaterEqualStep(this, 360)) {
        al::setVelocityZero(this);
        al::invalidateHitSensors(this);
        al::hideModelIfShow(this);
        al::setNerve(this, &NrvMotorcycle.Reset);
        return;
    }

    if (al::isVelocitySlowH(this, 3.0f))
        al::setNerve(this, &NrvMotorcycle.Wait);
}

void Motorcycle::exeFall() {
    bool isInWater = al::isInWater(this);
    al::addVelocityY(this, isInWater ? -2.0f : -1.0f);
    if (isInWater)
        al::scaleVelocityParallelVertical(this, verticalUp, 0.8f, 0.925f);
    else
        al::scaleVelocityExceptDirection(this, verticalUp, 0.95f);

    if (al::getVelocity(this).dot({0.0f, -1.0f, 0.0f}) > 0.0f)
        al::limitVelocityDir(this, verticalUp, 35.0f);

    if (al::isInDeathArea(this)) {
        al::setVelocityZero(this);
        al::invalidateHitSensors(this);
        al::hideModelIfShow(this);
        al::setNerve(this, &NrvMotorcycle.Reset);
        return;
    }

    if (checkGroundStatus(this))
        return;

    if (al::isGreaterEqualStep(this, 360)) {
        al::setVelocityZero(this);
        al::invalidateHitSensors(this);
        al::hideModelIfShow(this);
        al::setNerve(this, &NrvMotorcycle.Reset);
        return;
    }

    if (al::isInWater(this) && !_248)
        al::startHitReaction(this, "着水");
}

void Motorcycle::exeJump() {
    if (al::isFirstStep(this))
        al::setVelocity(this, 0.0f, 50.0f, 0.0f);

    bool isInWater = al::isInWater(this);
    al::addVelocityY(this, isInWater ? -2.0f : -1.0f);
    if (!isInWater)
        al::scaleVelocityExceptDirection(this, verticalUp, 0.95f);
    else
        al::scaleVelocityParallelVertical(this, verticalUp, 0.8f, 0.925f);

    if (al::getVelocity(this).dot({0.0f, -1.0f, 0.0f}) > 0.0f)
        al::limitVelocityDir(this, verticalUp, 35.0f);

    if (al::getVelocity(this).dot({0.0f, -1.0f, 0.0f}) > 0.0f || al::isInDeathArea(this)) {
        al::setVelocityZero(this);
        al::invalidateHitSensors(this);
        al::hideModelIfShow(this);
        al::setNerve(this, &NrvMotorcycle.Reset);
        return;
    }

    if (!checkGroundStatus(this) && al::isInWater(this) && !_248)
        al::startHitReaction(this, "着水");
}

void Motorcycle::endJump() {
    mPlayerAnimator->tryStartBindRideLandIfJump();
    mJumpAngle = 0.0f;
}

void Motorcycle::exeReaction() {
    if (al::isFirstStep(this))
        al::startAction(this, "Reaction");

    steerAngle = al::lerpValue(steerAngle, 0.0f, 0.2f);
    handleAngle = al::lerpValue(handleAngle, 0.0f, 0.2f);
    mLeanAngle = al::lerpValue(mLeanAngle, 0.0f, 0.2f);
    mJumpAngle = al::lerpValue(mJumpAngle, 0.0f, 0.2f);
    al::setNerveAtActionEnd(this, &NrvMotorcycle.Wait);
}

void Motorcycle::exeReset() {
    if (al::isFirstStep(this)) {
        al::tryKillEmitterAndParticleAll(this);
        al::stopAction(this);
        if (al::isNerve(this, &NrvMotorcycle.Reset))
            al::startHitReaction(this, "リセット");
        al::offCollide(this);
        steerAngle = 0.0f;
        handleAngle = 0.0f;
        mLeanAngle = 0.0f;
        mJumpAngle = 0.0f;
        valA = -1;
    }

    if (al::isStep(this, 1)) {
        al::resetQuatPosition(this, mStartQuat, mTrans);
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

static inline bool doTheImportanStuff(Motorcycle* actor, IUsePlayerPuppet** puppy,
                                      MotorcyclePlayerAnimator* animator,
                                      al::CameraTargetBase* camera,
                                      al::TransCameraSubTarget* subCamera) {
    if (!rs::isActiveBindKeepDemo(actor->getBindKeepDemoInfo()) && rs::isTriggerGetOff(*puppy)) {
        if (!al::isInAreaObj(actor, "MotorcycleInvalidGetOffArea")) {
            startGetOff(actor, puppy, actor->get_23c(), animator, camera, subCamera);
            return true;
        }
        al::startHitReaction(actor, "降車キャンセル");
    }
    return false;
}

void Motorcycle::exeRideWait() {
    if (al::isFirstStep(this)) {
        al::tryStartActionIfNotPlaying(this, "Wait");
        mPlayerAnimator->startBindWait();
    }
    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mColliderCameraTarget,
                           mTransCameraSubTarget))
        return;

    steerAngle = al::lerpValue(steerAngle, 0.0f, 0.025f);
    alPadRumbleFunction::startPadRumbleDirectValue(this, 160.0f, 168.0f, 0.035f, 0.035f, 0.7f, 0.7f,
                                                   -1);
    updateOrientation(steerAngle, this);
    al::addVelocityToGravity(this, 2.0f);
    al::scaleVelocity(this, 0.95f);

    if (rs::isCollidedGround(this))
        al::limitVelocityDirSign(this, al::getGravity(this), 3.0f);

    if (rs::isPuppetHoldActionButton(mPlayerPuppet)) {
        al::setNerve(this, &NrvMotorcycle.RideRunStart);
        return;
    }

    if (!funF(this, mPlayerPuppet))
        tryParking(this, mPlayerPuppet, mParkingParams, &steerAngle);
}

void Motorcycle::exeRideWaitJump() {
    if (al::isFirstStep(this)) {
        mParams->groundNormal = {0.0f, 1.0f, 0.0f};

        _247 = mParams->isOnJump;
        al::setVelocity(this, 0.0f, !_247 ? 50.0f : 18.5f, 0.0f);
        al::startHitReaction(this, "ジャンプ開始");
        mPlayerAnimator->startBindRideJump();
    }

    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mColliderCameraTarget,
                           mTransCameraSubTarget))
        return;

    updateSeRumble(steerAngle, mSeRumbleState, this, mPlayerPuppet, true);
    funV(this);
    funPY(this, mPlayerPuppet, mAccelerationState, &steerAngle);
    sead::Vector2f stick = {0.0f, 0.0f};
    calcInputStick(&stick, this, mPlayerPuppet);
    f32 angle = stick.x * 2.5f;
    if (angle * angle > 0.1f) {
        sead::Vector3f front = al::getFront(this);
        sead::Vector3f front2 = al::getFront(this);
        al::rotateVectorDegree(&front2, front2, al::getGravity(this), angle);
        al::normalize(&front2);

        sead::Vector3f diff = front2 - front;
        al::normalize(&diff);
        al::setFront(this, diff);

        f32 fVar10 = getJointDistance(this, "AllRoot", "JointRoot") *
                     sead::Mathf::sin(sead::Mathf::deg2rad(sead::Mathf::abs(angle * 0.5f)));
        vector5.x = diff.x * fVar10 * fVar10;
        vector5.z = diff.z * fVar10 * fVar10;
    }

    al::addVelocityToDirection(
        this, al::getFront(this),
        al::lerpValue(0.0f, 1.8f,
                      al::easeIn(al::normalize(mAccelerationState->accelRate, 0.0f, 5.0f))));
    if (!_247 || !rs::isPuppetHoldJumpButton(mPlayerPuppet) || al::isGreaterStep(this, 6)) {
        _247 = false;
        al::addVelocityY(this, -2.0f);
    }

    al::scaleVelocityExceptDirection(this, verticalUp, 0.95f);
    if (al::getVelocity(this).dot({0.0f, -1.0f, 0.0f}) > 0.0f)
        al::limitVelocityDir(this, verticalUp, 35.0f);

    if (rs::isOnGround(this, this)) {
        al::startHitReaction(this, "着地");
        if (!funF(this, mPlayerPuppet)) {
            mLeanAngle = 0.0f;
            al::setNerve(this, &NrvMotorcycle.RideWaitLand);
        } else {
            al::startAction(this, "WaitLand");
        }
    }
}

void Motorcycle::endRideWaitJump() {
    mPlayerAnimator->tryStartBindRideLandIfJump();
    mJumpAngle = 0.0f;
}

void Motorcycle::exeRideWaitLand() {
    if (al::isFirstStep(this))
        al::startAction(this, "WaitLand");

    if (funF(this, mPlayerPuppet))
        return;

    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mColliderCameraTarget,
                           mTransCameraSubTarget))
        return;

    steerAngle = al::lerpValue(steerAngle, 0.0f, 0.025f);
    alPadRumbleFunction::startPadRumbleDirectValue(this, 160.0f, 168.0f, 0.035f, 0.035f, 0.7f, 0.7f,
                                                   -1);
    updateOrientation(steerAngle, this);
    al::addVelocityToGravity(this, 2.0f);
    al::scaleVelocity(this, 0.95f);

    if (rs::isCollidedGround(this))
        al::limitVelocityDirSign(this, al::getGravity(this), 3.0f);

    if (rs::isPuppetHoldActionButton(mPlayerPuppet)) {
        al::setNerve(this, &NrvMotorcycle.RideRunStart);
        return;
    }

    al::setNerveAtActionEnd(this, &NrvMotorcycle.RideWait);
}

void Motorcycle::exeRideRunStart() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "RunStart");
        mPlayerAnimator->startBindRideRunStart();
        mSeRumbleState->reset();
    }
    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mColliderCameraTarget,
                           mTransCameraSubTarget))
        return;
    if (funP(this, vector4))
        return;

    updateStick(this, mPlayerPuppet, &steerAngle, 40.0f, -15.0f, 0.3f);
    updateOrientation(steerAngle, this);
    funPY(this, mPlayerPuppet, mAccelerationState, &steerAngle);

    funQ(this, al::lerpValue(0.0f, 1.8f,
                             al::easeIn(al::normalize(mAccelerationState->accelRate, 0.0f, 5.0f))));
    updateSeRumble(steerAngle, mSeRumbleState, this, mPlayerPuppet, 0);

    if (funR(this, mPlayerPuppet))
        return;

    if (funS(this))
        return;

    MotorcycleParams* params = mParams;
    if (rs::isCollidedGround(this) && !params->isOnGround) {
        if (150.0f < getDistance(this, params)) {
            funT(this, verticalUp, -rs::getCollidedGroundNormal(this));
            al::limitVelocityDirSign(this, -rs::getCollidedGroundNormal(this), 5.0f);
            al::setNerve(this, &NrvMotorcycle.RideRunLand);
            return;
        }
    }
    if (al::isActionEnd(this))
        al::setNerve(this, &NrvMotorcycle.RideRun);
}

void Motorcycle::exeRideRun() {
    if (al::isFirstStep(this)) {
        al::tryStartActionIfNotPlaying(this, "Run");
        mPlayerAnimator->tryStartBindRideRunIfNotPlaying();
        _234 = 0;
    }
    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mColliderCameraTarget,
                           mTransCameraSubTarget))
        return;
    if (funP(this, vector4))
        return;
    updateStick(this, mPlayerPuppet, &steerAngle, 40.0f, -15.0f, 0.3f);
    updateOrientation(steerAngle, this);
    funPY(this, mPlayerPuppet, mAccelerationState, &steerAngle);

    funQ(this, al::lerpValue(0.0f, 1.8f,
                             al::easeIn(al::normalize(mAccelerationState->accelRate, 0.0f, 5.0f))));
    updateSeRumble(steerAngle, mSeRumbleState, this, mPlayerPuppet, 0);
    if (!mAccelerationState->isAccelerating)
        _234 = 0;
    else if (_234++ > 13)
        rs::requestDownToDefaultCameraAngleBySpeed(this, 6.0f, 0);

    if (funR(this, mPlayerPuppet) || funS(this))
        return;

    MotorcycleParams* params = mParams;
    if (rs::isCollidedGround(this) && !params->isOnGround) {
        if (getDistance(this, params) > 150.0f) {
            funT(this, verticalUp, -rs::getCollidedGroundNormal(this));
            al::limitVelocityDirSign(this, -rs::getCollidedGroundNormal(this), 5.0f);
            al::setNerve(this, &NrvMotorcycle.RideRunLand);
            return;
        }
    }

    if (!tryParking(this, mPlayerPuppet, mParkingParams, &steerAngle) &&
        al::isVelocitySlowH(this, 3.0f) && mAccelerationState->accelRate < 0.01f &&
        rs::isOnGround(this, this)) {
        al::setNerve(this, &NrvMotorcycle.RideWait);
    }
}

void Motorcycle::exeRideRunCollide() {
    if (al::isFirstStep(this))
        al::startAction(this, "RunCollide");
    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mColliderCameraTarget,
                           mTransCameraSubTarget))
        return;
    updateStick(this, mPlayerPuppet, &steerAngle, 40.0f, -15.0f, 0.3f);
    updateOrientation(steerAngle, this);
    funPY(this, mPlayerPuppet, mAccelerationState, &steerAngle);

    funQ(this, al::lerpValue(0.0f, 1.8f,
                             al::easeIn(al::normalize(mAccelerationState->accelRate, 0.0f, 5.0f))));
    updateSeRumble(steerAngle, mSeRumbleState, this, mPlayerPuppet, 0);

    if (funR(this, mPlayerPuppet))
        return;

    if (funS(this))
        return;

    MotorcycleParams* params = mParams;
    if (rs::isCollidedGround(this) && !params->isOnGround) {
        if (150.0f < getDistance(this, params)) {
            funT(this, verticalUp, -rs::getCollidedGroundNormal(this));
            al::limitVelocityDirSign(this, -rs::getCollidedGroundNormal(this), 5.0f);
            al::setNerve(this, &NrvMotorcycle.RideRunLand);
            return;
        }
    }

    if (rs::isCollidedWallVelocity(this, this))
        return;
    if (mParams->backContactPoints.size() > 0 && mParams->frontContactPoints.size() > 0)
        return;
    al::setNerve(this, &NrvMotorcycle.RideRun);
}

void Motorcycle::exeRideRunFall() {
    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mColliderCameraTarget,
                           mTransCameraSubTarget))
        return;

    mParams->groundNormal = {0.0f, 1.0f, 0.0f};
    updateStick(this, mPlayerPuppet, &steerAngle, 12.5f, 7.5f, 0.1f);

    applyAirPhysics(al::isInWater(this) ? 1.0f : 2.0f, this, mPlayerPuppet);
    updateSeRumble(steerAngle, mSeRumbleState, this, mPlayerPuppet, true);
    if (!funL(this) && (!rs::isCollidedWallVelocity(this, this) || !checkDashCollision(this))) {
        updateAirOrientation(this);
        if (!funO(this) && al::isInWater(this) && !_248) {
            al::startHitReaction(this, "着水");
            return;
        }
    }
    return;
}

void Motorcycle::exeRideRunWheelie() {
    if (al::isFirstStep(this))
        mPlayerAnimator->startBindRideJump();

    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mColliderCameraTarget,
                           mTransCameraSubTarget))
        return;
    mParams->groundNormal = {0.0f, 1.0f, 0.0f};

    updateStick(this, mPlayerPuppet, &steerAngle, 12.5f, -7.0f, 0.1f);

    applyAirPhysics(al::calcNerveEaseInValue(this, 30, 0.0f, al::isInWater(this) ? 1.0f : 2.0f),
                    this, mPlayerPuppet);
    updateSeRumble(steerAngle, mSeRumbleState, this, mPlayerPuppet, true);
    if (!funL(this) && (!rs::isCollidedWallVelocity(this, this) || !checkDashCollision(this))) {
        updateAirOrientation(this);
        if (!funO(this) && al::isInWater(this) && !_248) {
            al::startHitReaction(this, "着水");
            return;
        }
    }
}

void Motorcycle::endRideRunWheelie() {
    mPlayerAnimator->tryStartBindRideLandIfJump();
    mJumpAngle = 0.0f;
}

void Motorcycle::exeRideRunLand() {
    if (al::isFirstStep(this))
        al::startAction(this, "RunLand");

    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mColliderCameraTarget,
                           mTransCameraSubTarget))
        return;

    if (!funP(this, vector4)) {
        updateStick(this, mPlayerPuppet, &steerAngle, 40.0f, -15.0f, 0.3f);
        updateOrientation(steerAngle, this);
        funPY(this, mPlayerPuppet, mAccelerationState, &steerAngle);

        funQ(this,
             al::lerpValue(0.0f, 1.8f,
                           al::easeIn(al::normalize(mAccelerationState->accelRate, 0.0f, 5.0f))));
        updateSeRumble(steerAngle, mSeRumbleState, this, mPlayerPuppet, al::isLessStep(this, 3));
        if (!funR(this, mPlayerPuppet)) {
            if (rs::isCollidedGround(this) || mParams->framesInAir < 6) {
                al::setNerveAtActionEnd(this, &NrvMotorcycle.RideRun);
                return;
            }
            funT(this, -mParams->groundNormal, verticalUp);
            al::limitVelocityDirSign(this, verticalUp, 3.0f);
            al::setNerve(this, &NrvMotorcycle.RideRunFall);
        }
    }
}

void Motorcycle::exeRideRunJump() {
    if (al::isFirstStep(this)) {
        al::tryStartActionIfNotPlaying(this, "Run");
        al::startHitReaction(this, "ジャンプ開始");
        _247 = mParams->isOnJump;
        al::setVelocity(this, 0.0f, !_247 ? 55.0f : 21.5f, 0.0f);
        mPlayerAnimator->startBindRideJump();
    }
    mParams->groundNormal = {0.0f, 1.0f, 0.0f};
    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mColliderCameraTarget,
                           mTransCameraSubTarget))
        return;

    updateStick(this, mPlayerPuppet, &steerAngle, 40.0f, -15.0f, 0.3f);
    updateSeRumble(steerAngle, mSeRumbleState, this, mPlayerPuppet, 1);
    updateOrientation(steerAngle, this);
    funV(this);
    funPY(this, mPlayerPuppet, mAccelerationState, &steerAngle);
    mJumpAngle = al::lerpValue(0.0f, -20.0f, al::calcNerveRate(this, 4));
    if (!_247 || !rs::isPuppetHoldJumpButton(mPlayerPuppet) || al::isGreaterEqualStep(this, 6)) {
        _247 = 0;
        sead::Vector3f front = al::getFront(this);
        al::verticalizeVec(&front, verticalUp, front);
        al::tryNormalizeOrZero(&front);
        f32 fVar14 = al::lerpValue(
            0.0f, 1.8, al::easeIn(al::normalize(mAccelerationState->accelRate, 0.0f, 5.0f)));
        al::addVelocity(this, front * fVar14 + (al::isInWater(this) ? 1.0f : 2.0f) *
                                                   sead::Vector3f(0.0f, -1.0f, 0.0f));
    }
    al::scaleVelocityExceptDirection(this, verticalUp, 0.95f);
    if (0.0f < al::getVelocity(this).dot({0.0f, -1.0f, 0.0f}))
        al::limitVelocityDir(this, verticalUp, 35.0f);
    if (!funL(this)) {
        if (!rs::isOnGround(this, this)) {
            if (al::isInWater(this) && !_248)
                al::startHitReaction(this, "着水");
        } else if (!al::isVelocitySlowH(this, 3.0f) || 0.01f <= mAccelerationState->accelRate) {
            if (funR(this, mPlayerPuppet)) {
                al::startAction(this, "RunLand");
                return;
            }
            funT(this, verticalUp, -rs::getCollidedGroundNormal(this));
            al::limitVelocityDirSign(this, -rs::getCollidedGroundNormal(this), 5.0f);
            al::setNerve(this, &NrvMotorcycle.RideRunLand);
        } else {
            if (funF(this, mPlayerPuppet))
                return;
            al::setNerve(this, &NrvMotorcycle.RideWaitLand);
        }
    }
}

void Motorcycle::endRideRunJump() {
    mPlayerAnimator->tryStartBindRideLandIfJump();
    mJumpAngle = 0.0f;
}

void Motorcycle::exeRideRunBoundStart() {
    if (al::isFirstStep(this))
        al::startAction(this, "RunLand");

    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mColliderCameraTarget,
                           mTransCameraSubTarget))
        return;

    mParams->groundNormal.set(0.0f, 1.0f, 0.0f);
    funE(this, mPlayerPuppet, &steerAngle, mSeRumbleState, _248);
    al::setNerveAtActionEnd(this, &NrvMotorcycle.RideRunBound);
}

void Motorcycle::exeRideRunBound() {
    if (al::isFirstStep(this))
        al::startAction(this, "Run");

    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mColliderCameraTarget,
                           mTransCameraSubTarget))
        return;
    mParams->groundNormal.set(0.0f, 1.0f, 0.0f);
    funE(this, mPlayerPuppet, &steerAngle, mSeRumbleState, _248);
}

void Motorcycle::exeRideRunClash() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "Clash");
        mPlayerAnimator->startBindRideClash();
    }

    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mColliderCameraTarget,
                           mTransCameraSubTarget))
        return;

    steerAngle = al::lerpValue(steerAngle, 0.0f, 0.025f);
    updateSeRumble(steerAngle, mSeRumbleState, this, mPlayerPuppet, true);
    al::addVelocityY(this, -2.0f);
    al::scaleVelocityY(this, 0.95f);
    if (al::isActionEnd(this) && rs::isOnGround(this, this))
        al::setNerve(this, &NrvMotorcycle.RideWait);
}

void Motorcycle::exeRideParkingSnap() {
    if (al::isFirstStep(this)) {
        mSeRumbleState->reset();
        alPadRumbleFunction::stopPadRumbleDirectValue(this, -1);
    }

    f32 rate = al::calcNerveRate(this, 6);
    ParkingParams* params = mParkingParams;
    f32 invRate = 1.0f - rate;
    steerAngle = invRate * params->steerAngle;
    handleAngle = invRate * params->handleAngle;
    mLeanAngle = invRate * params->leanAngle;
    mJumpAngle = invRate * params->jumpAngle;

    sead::Quatf quat = sead::Quatf::unit;
    al::slerpQuat(&quat, params->quatA, params->quatB, rate);

    sead::Vector3f pos = {0.0f, 0.0f, 0.0f};
    al::lerpVec(&pos, mParkingParams->vectorA, mParkingParams->vectorB, rate);
    al::resetQuatPosition(this, quat, pos);
    al::setNerveAtGreaterEqualStep(this, &RideParkingStart, 6);
}

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

    updateOrientation(steerAngle, this);
    al::addVelocityToGravity(this, 2.0f);
    al::scaleVelocity(this, 0.95f);

    if (rs::isCollidedGround(this))
        al::limitVelocityDirSign(this, al::getGravity(this), 3.0f);

    al::setNerveAtGreaterEqualStep(this, &RideParkingAfter, 8);
}

void Motorcycle::exeRideParkingAfter() {
    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mColliderCameraTarget,
                           mTransCameraSubTarget))
        return;

    updateOrientation(steerAngle, this);
    al::addVelocityToGravity(this, 2.0f);
    al::scaleVelocity(this, 0.95f);

    if (rs::isCollidedGround(this))
        al::limitVelocityDirSign(this, al::getGravity(this), 3.0f);

    if (rs::isPuppetHoldActionButton(mPlayerPuppet)) {
        al::setNerve(this, &NrvMotorcycle.RideRunStart);
        return;
    }

    funF(this, mPlayerPuppet);
}

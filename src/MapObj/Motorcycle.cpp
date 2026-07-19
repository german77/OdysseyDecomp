#include "MapObj/Motorcycle.h"

#include "Library/Camera/CameraUtil.h"
#include "Library/Collision/CollisionParts.h"
#include "Library/Collision/CollisionPartsKeeperUtil.h"
#include "Library/Collision/PartsConnectorUtil.h"
#include "Library/Controller/PadRumbleFunction.h"
#include "Library/Effect/EffectSystemInfo.h"
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

#include "MapObj/MotorcyclePlayerAnimator.h"
#include "Player/CollisionShapeKeeper.h"
#include "Player/PlayerCollider.h"
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

const sead::Vector3f verticalUp = {0.0f, -1.0f, 0.0f};
const sead::Vector3f forceField = {0.0f, 50.0f, 10.0f};
const sead::Vector3f forceField2 = {-25.0f, 50.0f, -150.0f};
const sead::Vector3f forceField3 = {25.0f, 50.0f, -150.0f};
const sead::Vector3f forceField4 = {0.0f, 50.0f, -80.0f};

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

bool funM(Motorcycle* actor) {
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
    if (funM(actor))
        return true;

    sead::Vector3f sudovec = vec;
    al::verticalizeVec(&sudovec, al::getGravity(actor), sudovec);
    if (sudovec.length() >= 10.0f) {
        al::setNerve(actor, &NrvMotorcycle.RideRunCollide);
        return true;
    }
    return false;
}

bool funS(Motorcycle* actor) {
    if (rs::isCollidedGround(actor))
        return false;
    if (1 < actor->getParams()->val_c58) {
        if (17.5f < sead::Mathf::rad2deg(sead::Mathf::sin(al::getFront(actor).y))) {
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
    if (params->val_c58 >= 6) {
        funT(actor, -params->normal, verticalUp);
        al::limitVelocityDirSign(actor, verticalUp, 3.0f);
        al::setNerve(actor, &NrvMotorcycle.RideRunFall);
        return true;
    }
    return false;
}

static inline f32 getDistance(Motorcycle* actor, MotorcycleParams* params) {
    sead::Vector3f facingDir = al::getTrans(actor) - params->vector_5c;
    al::parallelizeVec(&facingDir, al::getGravity(actor), facingDir);
    return facingDir.dot(al::getGravity(actor));
}

bool funL(Motorcycle* actor) {
    if (!rs::isOnGround(actor, actor))
        return false;

    MotorcycleParams* params = actor->getParams();
    if (!rs::isCollidedGround(actor) || params->bool_0)
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
        if (rs::isCollidedGround(actor) && !params->bool_0) {
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

void funD(Motorcycle* actor, IUsePlayerPuppet** playerPuppet, s32* valueA,
          MotorcyclePlayerAnimator* valueB, al::CameraTargetBase* valueC,
          al::CameraSubTargetBase* param_6) {
    const sead::Vector3f supip = {0.0f, -1.0f, 0.0f};

    al::limitVelocityDirSign(actor, -supip, 0.0f);

    rs::showPuppetShadow(*playerPuppet);
    rs::validatePuppetReceivePush(*playerPuppet);

    MotorcycleParams* params = actor->getParams();
    if (params->array2.size() > 0 && params->array.size() > 0) {
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
        ;
    }

    rs::setPuppetFront(*playerPuppet, keko);
    rs::setPuppetUp(*playerPuppet, -supip);

    rs::endBindJumpAndPuppetNull(playerPuppet, sead::Vector3f::ey * 20.0f + keko * 9.0f, 3);

    *valueA = 5;
    funH(actor, valueB, valueC, param_6);
}

bool funU(Motorcycle* actor, IUsePlayerPuppet* playerPuppet, ParkingParams* params, f32* value) {
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
        fVar10 = (local_70.y * 75.0 - local_70.z * 30.0) + local_70.w * 0.0;
        fVar16 = local_70.w * 30.0 + (local_70.z * 0.0 - local_70.x * 75.0);
        fVar18 = local_70.z * fVar16;
        fVar17 = local_70.w * 75.0 + (local_70.x * 30.0 - local_70.y * 0.0);
        fVar11 = (-(local_70.x * 0.0) - local_70.y * 30.0) - local_70.z * 75.0;
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
        local_a0.x = 0.0;
        local_a0.y = 0.0;
        local_a0.z = 0.0;
        pVVar7 = al::getTrans(actor);
        local_b0.y = pVVar7->y - fVar12;
        local_b0.x = pVVar7->x - fVar14;
        local_b0.z = pVVar7->z - fVar17;
        al::parallelizeVec(&local_a0, &local_90, &local_b0);
        fVar11 = local_a0.x * local_a0.x + local_a0.y * local_a0.y + local_a0.z * local_a0.z;
        fVar10 = SQRT(fVar11);
        if (NAN(fVar10))
            fVar10 = sqrtf(fVar11);
        if (fVar10 <= 100.0) {
            local_c0.y = -backDir.y;
            local_c0.x = -backDir.x;
            local_c0.z = -backDir.z;
            local_b0.x = 0.0;
            local_b0.y = 0.0;
            local_b0.z = 0.0;
            pVVar7 = al::getTrans(actor);
            local_d0.x = pVVar7->x - fVar14;
            local_d0.y = pVVar7->y - fVar12;
            local_d0.z = pVVar7->z - fVar17;
            al::parallelizeVec(&local_b0, &local_c0, &local_d0);
            fVar11 = local_b0.x * local_b0.x + local_b0.y * local_b0.y + local_b0.z * local_b0.z;
            fVar10 = SQRT(fVar11);
            if (NAN(fVar10))
                fVar10 = sqrtf(fVar11);
            if (100.0 < fVar10) {
                bVar9 = 0;
                goto LAB_71002cac64;
            }
            pHVar8 = (HitSensor*)al::getHitSensor(actor, "PlayerBody");
            uVar3 = rs::sendMsgMotorcycleCollideParkingLot(groundSensor, pHVar8);
            if ((uVar3 & 1) != 0) {
                pLVar5 = al::getSensorHost(groundSensor);
                params->actor = pLVar5;
                fVar10 = value[3];
                params->floatC = value[2];
                params->floatJump = fVar10;
                fVar10 = value[1];
                params->floatA = value[0];
                params->floatB = fVar10;
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

void funK(float valueA, float* valueB, Motorcycle* actor, IUsePlayerPuppet* playerPuppet,
          bool isaval) {
    /*uVar3 = rs::isPuppetHoldActionButton(playerPuppet);
    fVar5 = playerPuppet[1];
    if ((uVar3 & 1) == 0) {
        fVar9 = 0.6;
        fVar4 = 0.0;
        if (-1 < (int)fVar5 + -0xf)
            fVar4 = (float)((int)fVar5 + -0xf);
    } else {
        fVar4 = 3.36312e-43;
        if ((int)fVar5 + 1 < 0xf1)
            fVar4 = (float)((int)fVar5 + 1);
        fVar9 = 1.0;
    }
    playerPuppet[1] = fVar4;*/
    f32 fVar4 = al::calcSpeedH(actor);
    /*
    fVar8 = (float)(int)playerPuppet[1];
    fVar6 = fVar8 / -240.0 + 1.0;
    fVar5 = fVar6;
    if (1.0 < fVar6)
        fVar5 = 1.0;
    fVar7 = 0.25;
    if (0.25 <= fVar6)
        fVar7 = fVar5;
    *playerPuppet = fVar4;
    fVar5 = 0.0;
    if ((int)playerPuppet[1] < 0x28) {
        fVar6 = (fVar8 / 40.0 + -1.0) * 3.1415927;
        fVar5 = sinf(fVar6);
        fVar4 = *playerPuppet;
        fVar5 = (fVar6 * fVar5) / 1.8;
    }
    fVar8 = fVar4 + -5.0;
    bVar2 = (valueC & 1) == 0;
    fVar6 = 272.0;
    if (bVar2)
        fVar6 = 160.0;
    if (fVar8 <= 0.0)
        fVar8 = 0.0;
    fVar7 = fVar9 * fVar7 * 0.35 * (fVar5 * 1.45 + 1.0);
    fVar9 = fVar7 * 0.6;
    if (bVar2)
        fVar9 = fVar7;
    fVar8 = (fVar5 * 0.08 + 1.0) * (fVar8 / 70.0 + 1.0) * 168.0;
    fVar5 = fVar8 * 1.7;
    if (bVar2)
        fVar5 = fVar8;
    */
    al::holdSeWithParam(actor, "CurveLv", 123 * valueA, "回転角(Degree)");
    al::holdSeWithParam(actor, "CurveLv", 123 * valueA, "回転角(Degree)");
    al::holdSeWithParam(actor, "CurveLv", 123 * valueA, "回転角(Degree)");
    al::holdSeWithParam(actor, "CurveLv", 123 * valueA, "回転角(Degree)");
    al::holdSeWithParam(actor, "CurveLv", 123 * valueA, "回転角(Degree)");
    al::holdSeWithParam(actor, "CurveLv", 123 * valueA, "回転角(Degree)");
    al::holdSeWithParam(actor, "CurveLv", 123 * valueA, "回転角(Degree)");
    al::holdSeWithParam(actor, "CurveLv", 123 * valueA, "回転角(Degree)");
    /*
    fVar8 = 0.01;
    fVar4 = 0.0;
    if (valueA <= 0.0) {
        fVar8 = 0.0;
        fVar4 = 0.01;
    }
    alPadRumbleFunction::startPadRumbleDirectValue(actor, fVar6, fVar5, fVar9, fVar9,
                                                   (0.7 - fVar4 * valueA) * 0.7,
                                                   (fVar8 * valueA + 0.7) * 0.7, -1);*/
    f32 fVar5 = rs::isPuppetHoldActionButton(playerPuppet) ? 9.0f : 0.0f;

    al::holdSeWithParam(actor,
                        rs::isPuppetHoldActionButton(playerPuppet) ? "MoveStartLv" : "MoveEndLv",
                        fVar5 + *valueB, "");
    al::holdSeWithParam(actor, "MoveLv", fVar5 + *valueB, "");
    al::holdSeWithParam(actor, "IdleLv", fVar5 + *valueB, "");
    return;
}
void funN(Motorcycle *actor)
{
  if (al::getVelocity(actor).y <= 0.0f) {
    sead::Vector3f seomePose={0.0f,0.0f,0.0f};
    const sead::Vector3f& trans = al::getTrans(actor);
    const sead::Vector3f& front2 = al::getFront(actor);
    sead::Vector3f frontTirePos={0.0f,0.0f,0.0f};
    sead::Vector3f backTirePos={0.0f,0.0f,0.0f};
    al::calcJointPos(&frontTirePos,actor,"FrontTire");
    al::calcJointPos(&backTirePos,actor,"BackTire");
    f32 tireDistance=frontTirePos.cross(backTirePos).length();
    /*fVar6 = *pfVar4;
    fVar8 = pfVar4[1];
    fVar9 = pfVar4[2];
    fVar10 = pVVar3->x;
    fVar11 = pVVar3->y;
    fVar7 = pVVar3->z;
    pVVar3 = al::getGravity(actor);
    local_60.z = (fVar7 - fVar5 * fVar9) - pVVar3->z * 150.0;
    local_60.y = (fVar11 - fVar5 * fVar8) - pVVar3->y * 150.0;
    local_60.x = (fVar10 - fVar5 * fVar6) - pVVar3->x * 150.0;
    pIVar1 = (IUseCollision *)0x0;
    if (actor != (LiveActor *)0x0) {
      pIVar1 = (IUseCollision *)&actor->collisionDirector;
    }
    pVVar3 = al::getGravity(actor);
    local_70.y = pVVar3->y * 150.0;
    local_70.x = pVVar3->x * 150.0;
    local_70.z = pVVar3->z * 150.0;
    bVar2 = alCollisionUtil::getFirstPolyOnArrow
                      (pIVar1,&local_80,nullptr,&local_60,&local_70,
                       nullptr,nullptr);
    if (bVar2) {*/
      //sead::Vector3f front = al::getTrans(actor)-local_80;
        sead::Vector3f front;
      al::normalize(&front);
      al::setFront(actor,front);
      sead::Vector3f* gravity = al::getGravityPtr(actor);
      al::verticalizeVec(gravity,front,*gravity);
      al::normalize(al::getGravityPtr(actor));
    //}
  }
}

void funE(Motorcycle *actor,IUsePlayerPuppet *playerPuppet,float *valueA,
                     f32* valueB,bool valueC)
{
    sead::Vector2f stick={0.0f,0.0f};
  funI(&stick,actor,playerPuppet);
  f32 fVar12 = stick.x * 12.5f;
  f32 fVar9 = (float)al::diffNearAngleDegree(0.0f,rs::getPuppetPoseRotZDegreeLeft(playerPuppet));
  f32 fVar10 = (float)al::diffNearAngleDegree(0.0f,rs::getPuppetPoseRotZDegreeRight(playerPuppet));
  f32 fVar8 = fVar9;
  if (fVar9 <= 0.0f) {
    fVar8 = -fVar9;
  }
  f32 fVar11 = fVar10;
  if (fVar10 <= 0.0f) {
    fVar11 = -fVar10;
  }
  if (fVar11 <= fVar8) {
    fVar10 = fVar9;
  }
  fVar8 = al::lerpValue(*valueA,fVar12 + al::normalizeAbs(fVar10,45.0f,135.0f) * -7.5f,0.1f);
  /*valueA = fVar8;
  local_68.x = 0.0;
  local_68.y = 0.0;
  funI(&local_68,actor,playerPuppet);
  pVVar4 = (Vector3 *)al::getFrontPtr(actor);
  al::rotateVectorDegreeY(pVVar4,local_68.x * -0.5);
  pVVar4 = (Vector3 *)al::getFrontPtr(actor);
  al::normalize(pVVar4);
  pVVar4 = al::getVelocity(actor);
  fVar8 = pVVar4->x;
  pVVar4 = al::getVelocity(actor);
  fVar10 = pVVar4->z;
  pVVar4 = al::getVelocity(actor);
  fVar9 = pVVar4->y;
  uVar5 = rs::isPuppetHoldActionButton(playerPuppet);
  if ((uVar5 & 1) == 0) {
    fVar10 = fVar10 * 0.95;
    fVar8 = fVar8 * 0.95;
  }
  puVar6 = (uint *)al::getFront(actor);
  uVar3 = *puVar6;
  lVar7 = al::getFront(actor);
  local_78.z = *(float *)(lVar7 + 8);
  local_78.y = 0.0;
  local_78.x = (float)uVar3;
  al::tryNormalizeOrZero(&local_78);
  fVar10 = fVar10 * fVar10 + fVar8 * fVar8 + 0.0;
  fVar8 = SQRT(fVar10);
  if (NAN(fVar8)) {
    fVar8 = sqrtf(fVar10);
  }
  local_88.y = fVar9 + fVar8 * local_78.y;
  local_88.z = fVar8 * local_78.z + 0.0;
  local_88.x = fVar8 * local_78.x + 0.0;
  al::setVelocity(actor,&local_88);*/
  al::addVelocityY(actor,-2.0f);
  funK(*valueA,valueB,actor,playerPuppet,1);
  if (!funL(actor)) {

    if (!rs::isCollidedWallVelocity(actor,actor) || !funM(actor)) {
      funN(actor);
      if (!funO(actor) && !al::isInWater(actor)&&valueC )
      {
        al::startHitReaction(actor,"着水");
      }
    }
  }
  return;
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
    al::startMclAnim(this, mIsOnLight ? "FrontOnTailOff" : "FrontOffTailOff");
    al::offCollide(this);
    al::calcQuat(&quat, this);
    vector = al::getTrans(this);

    mPlayerAnimator = new MotorcyclePlayerAnimator();
    /*puVar7 = operator.new(8);
    *puVar7 = 0;
    *(undefined4*)(puVar7 + 4) = 0;
    field6_0x130 = puVar7;*/
    mParams = new MotorcycleParams();
    /*puVar9 = operator.new(8);
    *puVar9 = 0;
    field11_0x148 = puVar9;*/
    mParkingParams = new ParkingParams();
    al::initJointControllerKeeper(this, 6);
    al::initJointLocalZRotator(this, &floatA, "AllRoot");
    al::initJointLocalXRotator(this, &floatB, "Handle");
    al::initJointLocalYRotator(this, &floatB, "FrontWheel");
    al::initJointLocalYRotator(this, &floatB, "Cowl");
    al::initJointLocalXRotator(this, &floatC, "AllRoot");
    al::initJointLocalXRotator(this, &floatJump, "JointRoot");
    CollisionShapeKeeper* collision = new CollisionShapeKeeper(7, 0x40, 0x10);
    collision->createShapeArrow("FrontCenter", forceField, {0.0f, -70.0f, 0.0f}, 20.0f, 0);
    collision->createShapeArrow("FrontLeft", forceField2, {0.0f, -70.0f, 0.0f}, 20.0f, 1);
    collision->createShapeArrow("FrontRight", forceField3, {0.0f, -70.0f, 0.0f}, 20.0f, 2);

    collision->createShapeSphereSupportGround("FrontFace", 60.0f, {0.0f, 80.0f, -35.0f},
                                              sead::Vector3f::ey, 20.0f);
    collision->createShapeSphereSupportGround("BackFace", 60.0f, {0.0f, 80.0f, -120.0f},
                                              sead::Vector3f::ey, 20.0f);
    collision->createShapeSphereSupportGround("CenterFace", 60.0f, {0.0f, 80.0f, -60.0f},
                                              sead::Vector3f::ey, 20.0f);

    collision->createShapeSphereIgnoreGround("Head", 40.0, {0.0f, 155.0f, -80.0f});
    collision->updateShape();

    PlayerCollider* collider = new PlayerCollider(getCollisionDirector(), getBaseMtx(),
                                                  al::getTransPtr(this), &verticalUp, false);
    collider->setCollisionShapeKeeper(collision);
    collider->onInvalidate();
    mPlayerCollider = collider;
    rs::createAndSetColliderFilterSpecialPurpose(this, "MoveLimit");

    /*this_04 = operator.new(0x30);
    pIVar2 = (IUsePlayerCollision*)0x0;
    if (this != (Motorcycle*)0x0)
        pIVar2 = (IUsePlayerCollision*)mPlayerCollision;
    PlayerColliderCameraTarget::PlayerColliderCameraTarget((PlayerColliderCameraTarget*)this_04,
                                                           this, pIVar2);
    this_04->vtable = (undefined*)&PTR_getTargetName_7101d16928;
    mCameraTargetBase = this_04;
    pCVar13 = (CameraSubTargetBase*)al::createTransCameraSubTarget("バイク進行方向", VectorA);
    mCameraSubTargetBase = pCVar13;
    pCVar14 = operator.new(0x30);
    pCVar14->mTurnSpeedRate1 = 0.3;
    pCVar14->mTurnSpeedRate2 = 0.1;
    pCVar14->mTurnBrakeEndDistance = -1.0;
    pCVar14->mTurnBrakeStartDistance = -1.0;
    pCVar14->mTurnStopStartDistance = -1.0;
    pCVar14->mTurnStopEndDistance = -1.0;
    pCVar14->_18 = 0x1e;
    pCVar14->mIsTurnV = false;
    pCVar14->mIsResetAfterTurnV = false;
    pCVar14->mMinTurnDegreeV = 0.0;
    pCVar14->mMaxTurnDegreeV = 0.0;
    pCVar14->mValidTurnDegreeRangeH = -1.0;
    pCVar14->mValidFaceDegreeRangeH = -1.0;
    mCameraSubTargetTurnParam = pCVar14;
    al::initCameraSubTargetTurnParam(pCVar13, pCVar14);
    pCVar15 = (CollisionPartsConnector*)al::tryCreateCollisionPartsConnector(this, info, quat);
    mCollisionPartsConnector = pCVar15;
    pBVar16 = (BindKeepDemoInfo*)rs::initBindKeepDemoInfo();
    mBindKeepDemoInfo = pBVar16;
    al::tryGetArg(mIsStickWorldPose, info, "IsStickWorldPose");
    iVar5 = al::calcLinkChildNum(info, "CoursePoint");
    *(int*)field21_0x198 = iVar5;
    if (0 < iVar5) {
        auVar3._8_8_ = 0;
        auVar3._0_8_ = (long)iVar5;
        uVar18 = (long)iVar5 * 0xc;
        if (SUB168(auVar3 * ZEXT816(0xc), 8) != 0)
            uVar18 = 0xffffffffffffffff;
        pVVar6 = operator.new[](uVar18);
        field20_0x190 = pVVar6;
        al::getChildLinkT(pVVar6, info, "CoursePoint", 0);
        if (1 < *(int*)field21_0x198) {
            lVar19 = 1;
            lVar20 = 0xc;
            do {
                al::getChildLinkT((Vector3*)(field20_0x190 + lVar20), info, "CoursePoint",
                                  (int)lVar19);
                lVar19 = lVar19 + 1;
                lVar20 = lVar20 + 0xc;
            } while (lVar19 < *(int*)field21_0x198);
        }
    }*/
    al::setEffectNamedMtxPtr(this, "WaterSurface", &mtx);
    al::setSeUserSyncParamPtr(this, kk3, "スピード");
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

    if (!funA(this) && al::isInWater(this) && !_248)
        al::startHitReaction(this, "着水");
}

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

static inline bool doTheImportanStuff(Motorcycle* actor, IUsePlayerPuppet** puppy,
                                      MotorcyclePlayerAnimator* animator,
                                      al::CameraTargetBase* camera,
                                      al::CameraSubTargetBase* subCamera) {
    if (!rs::isActiveBindKeepDemo(actor->getBindKeepDemoInfo()) && rs::isTriggerGetOff(*puppy)) {
        if (!al::isInAreaObj(actor, "MotorcycleInvalidGetOffArea")) {
            funD(actor, puppy, actor->get_23c(), animator, camera, subCamera);
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
    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mCameraTargetBase,
                           mCameraSubTargetBase))
        return;

    floatA = al::lerpValue(floatA, 0.0f, 0.025f);
    alPadRumbleFunction::startPadRumbleDirectValue(this, 160.0f, 168.0f, 0.035f, 0.035f, 0.7f, 0.7f,
                                                   -1);
    funB(floatA, this);
    al::addVelocityToGravity(this, 2.0f);
    al::scaleVelocity(this, 0.95f);

    if (rs::isCollidedGround(this))
        al::limitVelocityDirSign(this, al::getGravity(this), 3.0f);

    if (rs::isPuppetHoldActionButton(mPlayerPuppet)) {
        al::setNerve(this, &NrvMotorcycle.RideRunStart);
        return;
    }

    if (!funF(this, mPlayerPuppet))
        funU(this, mPlayerPuppet, mParkingParams, &floatA);
}

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

void Motorcycle::exeRideRunFall() {
    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mCameraTargetBase,
                           mCameraSubTargetBase))
        return;

    mParams->normal = {0.0f, 1.0f, 0.0f};
    sead::Vector2f stick = {0.0f, 0.0f};
    funI(&stick, this, mPlayerPuppet);
    f32 fVar13 = stick.x * 12.5f;
    f32 fVar10 = al::diffNearAngleDegree(0.0f, rs::getPuppetPoseRotZDegreeLeft(mPlayerPuppet));
    f32 fVar11 = al::diffNearAngleDegree(0.0f, rs::getPuppetPoseRotZDegreeRight(mPlayerPuppet));
    fVar10 = sead::Mathf::abs(fVar10);
    fVar11 = sead::Mathf::abs(fVar11);

    floatA = al::lerpValue(floatA, fVar13 + al::normalizeAbs(fVar11, 45.0f, 135.0f) * -7.5f, 0.1f);

    funJ(al::isInWater(this) ? 1.0f : 2.0f, this, mPlayerPuppet);
    funK(floatA, kk3, this, mPlayerPuppet, 1);
    if (!funL(this) && (!rs::isCollidedWallVelocity(this, this) || !funM(this))) {
        funN(this);
        if (!funO(this) && al::isInWater(this) && !_248) {
            al::startHitReaction(this, "着水");
            return;
        }
    }
    return;
}

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


    if (doTheImportanStuff(this, &mPlayerPuppet, mPlayerAnimator, mCameraTargetBase,
                           mCameraSubTargetBase))
        return;
    mParams->normal.set(0.0f, 1.0f, 0.0f);
     funE(this,mPlayerPuppet,&floatA,kk3,_248);
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

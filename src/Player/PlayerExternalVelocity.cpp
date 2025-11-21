#include "Player/PlayerExternalVelocity.h"

#include "Library/Collision/CollisionPartsKeeperUtil.h"
#include "Library/Controller/PadRumbleFunction.h"
#include "Library/LiveActor/ActorCollisionFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/Math/MathUtil.h"

#include "Util/AreaUtil.h"
#include "Util/ExternalForceKeeper.h"
#include "Util/PlayerCollisionUtil.h"
#include "Util/SensorMsgFunction.h"

PlayerExternalVelocity::PlayerExternalVelocity(const al::LiveActor* actor,
                                               const IUsePlayerCollision* collision,
                                               const sead::Vector3f* trans)
    : mActor(actor), mCollision(collision), mTrans(trans) {
    mExternalForceKeeper = new ExternalForceKeeper();
}

bool PlayerExternalVelocity::receiveMsgPlayer(const al::SensorMsg* msg, al::HitSensor* self,
                                              al::HitSensor* other) {
    sead::Vector3f snapForce = {0.0f, 0.0f, 0.0f};
    if (rs::tryGetObjSnapForce(&snapForce, msg)) {
        mSnapForce = snapForce;
        return true;
    }

    return receiveMsgCap(msg, self, other);
}

bool PlayerExternalVelocity::receiveMsgCap(const al::SensorMsg* msg, al::HitSensor* self,
                                           al::HitSensor* other) {
    return mExternalForceKeeper->receiveMsg(msg, self, other);
}

void PlayerExternalVelocity::requestApplyLastGroundInertia() {
    i7c = 1;
}

void PlayerExternalVelocity::cancelAndFeedbackLastGroundInertia(al::LiveActor* actor, f32 fa,
                                                                bool fe) {
    sead::Vector3f oldv8 = v8;
    v20 -= v8;
    v8 = {0.0f, 0.0f, 0.0f};
    i7c = 0;

    al::verticalizeVec(&oldv8, al::getGravity(mActor), oldv8);

    sead::Vector3f dirv8 = {0.0f, 0.0f, 0.0f};
    if (!al::tryNormalizeOrZero(&dirv8, oldv8))
        return;

    if (fe) {
        sead::Vector3f frontDir = {0.0f, 0.0f, 0.0f};
        al::calcFrontDir(&frontDir, mActor);
        al::verticalizeVec(&frontDir, al::getGravity(mActor), frontDir);

        if (al::tryNormalizeOrZero(&frontDir))
            fa *= sead::Mathf::clampMin(frontDir.dot(dirv8), 0.0f);
    }

    al::addVelocity(actor, fa * oldv8);
}

void PlayerExternalVelocity::update() {
    v20 = {0.0f, 0.0f, 0.0f};
    bool isOnGround = false;
    if (mCollision)
        isOnGround = rs::isOnGround(mActor, mCollision);
    else
        isOnGround = al::isExistActorCollider(mActor) && al::isOnGround(mActor, 0);

    f32 zz = isOnGround ? v64.y : v64.z;
    f32 mm = isOnGround ? v58.x : v58.y;

    mExternalForceKeeper->calcForce(&mExternalForce);
    mExternalForceKeeper->reset();

    v08 = mm * v08 + zz * mExternalForce;
    if (v08.length() < 0.1f)
        v08 = {0.0f, 0.0f, 0.0f};

    updatePadRumbleExternalForce();

    v14 *= isOnGround ? v58.z : v64.x;

    sead::Vector3f mister = {0.0f, 0.0f, 0.0f};

    rs::calcExtForceAreaVelocity(&mister, mActor, al::getTrans(mActor), v14,
                                 al::getVelocity(mActor));

    v14 += mister;
    if (v14.length() < 0.1f)
        v14 = {0.0f, 0.0f, 0.0f};

    updateLastGroundForce(isOnGround);
    v20 = v8;
    v20 += v08 + v14;
}

void PlayerExternalVelocity::updatePadRumbleExternalForce() {
    if (!mTrans)
        return;

    f32 distance = v08.length();
    if (al::isNearZero(distance)) {
        if (isRumbleRunning) {
            isRumbleRunning = false;
            alPadRumbleFunction::stopPadRumbleLoop(mActor, "【ループ】ジー（強）", mTrans);
        }
        return;
    }

    distance = al::easeIn(al::calcRate01(distance, 0.0f, 50.0f));
    if (!isRumbleRunning) {
        isRumbleRunning = true;
        alPadRumbleFunction::startPadRumbleLoopNo3D(mActor, "【ループ】ジー（強）", mTrans);
    }

    f32 volume = distance * 0.35f;
    f32 pitch = sead::Mathf::clampMin(distance * -0.25f + 1.0f, 0.1f);

    alPadRumbleFunction::changePadRumbleLoopVolmue(mActor, "【ループ】ジー（強）", mTrans, volume,
                                                   volume);
    alPadRumbleFunction::changePadRumbleLoopPitch(mActor, "【ループ】ジー（強）", mTrans, pitch,
                                                  pitch);
}

void PlayerExternalVelocity::updateLastGroundForce(bool isOnGround) {
    if (!mCollision)
        return;

    v8 = {0.0f, 0.0f, 0.0f};
    if (isOnGround) {
        al::calcForceMovePowerExceptNormal(
            &mMovePowerForce, rs::getCollidedGroundCollisionParts(mCollision),
            rs::getCollidedGroundPos(mCollision), rs::getCollidedGroundNormal(mCollision));
        return;
    }

    if (i7c < 1)
        mMovePowerForce = {0.0f, 0.0f, 0.0f};
    else
        v8.set(mMovePowerForce);

    i7c = al::converge(i7c, 0, 1);
}

void PlayerExternalVelocity::reset() {
    mExternalForceKeeper->reset();
    mExternalForce = {0.0f, 0.0f, 0.0f};
    mSnapForce = {0.0f, 0.0f, 0.0f};
    v14 = {0.0f, 0.0f, 0.0f};
    v20 = {0.0f, 0.0f, 0.0f};
    v08 = {0.0f, 0.0f, 0.0f};
}

bool PlayerExternalVelocity::isExistForce() const {
    return !al::isNearZero(v14) || !al::isNearZero(mExternalForce) || !al::isNearZero(v08);
}

bool PlayerExternalVelocity::isExistSnapForce() const {
    return !al::isNearZero(mSnapForce);
}

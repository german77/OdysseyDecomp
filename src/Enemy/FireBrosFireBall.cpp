#include "Enemy/FireBrosFireBall.h"

#include "Library/Effect/EffectSystemInfo.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorAreaFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorCollisionFunction.h"
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

#include "Util/PlayerUtil.h"
#include "Util/SensorMsgFunction.h"
/*
namespace {
NERVE_IMPL(FireBrosFireBall, Appear)
NERVE_IMPL(FireBrosFireBall, Move)
NERVE_IMPL(FireBrosFireBall, Attach)

NERVES_MAKE_STRUCT(FireBrosFireBall, Move, Appear)
NERVES_MAKE_NOSTRUCT(FireBrosFireBall, Attach)
}  // namespace

FireBrosFireBall::FireBrosFireBall(const char* name, const al::LiveActor* bros)
    : BrosWeaponBase(name, bros) {}

void FireBrosFireBall::init(const al::ActorInitInfo& init) {
    al::initActorWithArchiveName(this, init, "FireBrosFireBall", nullptr);
    al::invalidateClipping(this);
    makeActorDead();
    al::initNerve(this, &NrvFireBrosFireBall.Move, 0);
}

//Not matching
void FireBrosFireBall::shoot(const sead::Vector3f& trans, const sead::Quatf& quat,
                             const sead::Vector3f& direction, bool isHack, int vall, bool isFast) {
    mIsHack = isHack;
    if (mIsHack) {
        al::validateHitSensor(this, "AttackHack");
        al::invalidateHitSensor(this, "Attack");
    } else {
        al::validateHitSensor(this, "Attack");
        al::invalidateHitSensor(this, "AttackHack");
    }

    al::setQuat(this, quat);
    al::setTrans(this, trans);

    sead::Vector3f frontDir;
    al::calcFrontDir(&frontDir, this);
    al::setNerve(this, &NrvFireBrosFireBall.Appear);
    al::startAction(this, "Appear");
    al::onCollide(this);

    sead::Vector3f directive;
    if (!isFast) {
        sead::Vector3f up = sead::Vector3f{0.0f, -25.0f, 0.0f};
        sead::Vector3f velocity = direction;
        velocity.y = 0.0f;
        directive = frontDir * 37.0f + up + velocity;
    } else if (isHack)
        directive = frontDir * 37.0f;
    else {
        sead::Vector3f up = sead::Vector3f{0.0f, -37.0f, 0.0f};
        sead::Vector3f velocity = direction;
        velocity.y = 0.0f;
        directive = frontDir * 37.0f + up + velocity;
    }
    al::setVelocity(this, directive);

    mIsFast = isFast;
    mIsBouncing = false;
    mAttackDelay = 0;
    isKeek = false;
    appear();
}

void FireBrosFireBall::killEnemy() {
    al::startHitReaction(this, "時間消滅");
    kill();
}

void bounce(al::LiveActor* actor, const sead::Vector3f& hitNormal) {
    sead::Vector3f wallNormal = hitNormal;
    sead::Vector3f up = {0.0f, 1.0f, 0.0f};
    al::verticalizeVec(&wallNormal, up, wallNormal);

    if (!al::tryNormalizeOrZero(&wallNormal))
        return;

    f32 velocity = 2.0f * wallNormal.dot(al::getVelocity(actor));
    if (velocity < 0.0f)
        al::getVelocityPtr(actor)->add(-velocity * wallNormal);
}

void FireBrosFireBall::move() {
    al::updateMaterialCodeWater(this);
    if (al::isGreaterEqualStep(this, 120)) {
        al::startHitReaction(this, "時間消滅");
        kill();
        return;
    }

    sead::Vector3f* velocity = al::getVelocityPtr(this);

    if (al::isOnGround(this, 0)) {
        const sead::Vector3f& groundNormal = al::getCollidedGroundNormal(this);
        if (sead::Mathf::abs(al::calcAngleDegree(groundNormal, sead::Vector3f::ey)) > 45.0f) {
            bounce(this, groundNormal);
        } else {
            mIsBouncing = true;
            al::startHitReaction(this, "バウンド");
            velocity->y = !mIsFast ? 27.0f : 35.0f;
            al::startHitReaction(this, "跳ね返し");
        }
    }

    if (al::isCollidedWallFace(this) && al::tryGetCollidedWallSensor(this))
        bounce(this, al::getCollidedWallNormal(this));

    if (!mIsBouncing && mIsFast && al::isGreaterEqualStep(this, 30))
        velocity->y -= 2.0f;

    if (al::isInWaterArea(this)) {
        al::startHitReaction(this, "消滅");
        kill();
    }

    if (mIsBouncing) {
        f32 currentUpVel = velocity->y;
        f32 gravity = !mIsFast ? -2.0f : -4.0f;
        velocity->y = (currentUpVel + gravity) * 0.98f;
    }

    al::tryAddRippleSmall(this);
}

void FireBrosFireBall::attach(const sead::Matrix34f* mtx, const sead::Vector3f& vecA,
                              const sead::Vector3f& vecB, const char* name) {
    BrosWeaponBase::attach(mtx, vecA, vecB, nullptr);
    al::setNerve(this, &Attach);
}

void FireBrosFireBall::attackSensor(al::HitSensor* self, al::HitSensor* other) {
    if (mIsHack) {
        if (al::isSensorPlayerAttack(self)) {
            if (al::getSensorHost(other) == getBrosActor() && !isKeek)
                return;
            if (al::sendMsgPlayerFireBallAttack(other, self)) {
                kill();
                return;
            }
            if (rs::sendMsgWeaponItemGet(other, self))
                return;
        }
    }

    if (!mIsHack && al::isSensorEnemyAttack(self) && al::getSensorHost(other) != getBrosActor() &&
        al::sendMsgEnemyAttackFire(other, self, nullptr)) {
        kill();
    }
}

inline void tryUpdateEffectMaterialCode(al::LiveActor* actor, const char* materialCode) {
    al::tryUpdateEffectMaterialCode(actor, materialCode);
}

//Not matching
bool FireBrosFireBall::receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                                  al::HitSensor* self) {
    if (al::isMsgVanish(message) || al::isMsgEnemyAttackFire(message) ||
        rs::isMsgSeedAttack(message) || rs::isMsgSeedAttackHold(message) ||
        rs::isMsgHammerBrosHammerHackAttack(message) ||
        rs::isMsgHammerBrosHammerEnemyAttack(message) || rs::isMsgTankBullet(message)) {
        if (al::isMsgEnemyAttackFire(message)) {
            const char* materialCode;
            if (al::tryGetEnemyAttackFireMaterialCode(&materialCode, message))
                tryUpdateEffectMaterialCode(this, materialCode);
        }
        al::startHitReaction(this, "消滅");
        kill();
        return true;
    }

    if (rs::isMsgPlayerDisregardTargetMarker(message))
        return true;

    if (al::isMsgPlayerInvincibleAttack(message)) {
        al::startHitReaction(this, "消滅");
        return true;
    }

    if (!rs::isMsgCapAttack(message) || mAttackDelay > 0)
        return false;

    sead::Vector3f dir = al::getSensorPos(self) - al::getSensorPos(other);
    dir.y = 0.0f;
    al::tryNormalizeOrDirZ(&dir, dir);

    sead::Vector3f vel = al::getVelocity(this);
    f32 vely = vel.y;
    vel.y = 0.0f;

    f32 dist = vel.length();

    sead::Vector3f dir3 = dir;
    sead::Vector3f dir2 = rs::getPlayerPos(this) - al::getTrans(this);
    dir2.y = 0.0f;

    if (al::calcAngleDegree(dir2, dir3) < 90.0f)
        dir3.negate();

    al::tryNormalizeOrDirZ(&dir3, dir3);
    dir3 *= dist;
    dir3.y = vely;

    al::setVelocity(this, dir3);
    mIsHack = true;
    isKeek = true;
    mAttackDelay = 60;
    al::validateHitSensor(this, "AttackHack");
    al::invalidateHitSensor(this, "Attack");
    al::startHitReaction(this, "帽子反射");
    return false;
}

void FireBrosFireBall::control() {
    if (al::isInDeathArea(this) || al::isCollidedFloorCode(this, "DamageFire") ||
        al::isCollidedFloorCode(this, "Needle") || al::isCollidedFloorCode(this, "Poison")) {
        al::startHitReaction(this, "消滅");
        kill();
        return;
    }

    if (mAttackDelay > 0)
        mAttackDelay--;

    if (al::isNerve(this, &NrvFireBrosFireBall.Move) ||
        al::isNerve(this, &NrvFireBrosFireBall.Appear)) {
        al::HitSensor* sensor = al::tryGetCollidedSensor(this);
        if (sensor)
            rs::sendMsgFireBrosFireBallCollide(sensor, al::getHitSensor(this, "Attack"));
    }
}

void FireBrosFireBall::exeAppear() {
    if (al::isFirstStep(this))
        al::startAction(this, "Appear");

    move();

    if (al::isActionEnd(this))
        al::setNerve(this, &NrvFireBrosFireBall.Move);
}

void FireBrosFireBall::exeMove() {
    if (al::isFirstStep(this))
        al::startAction(this, "Shot");
    move();
}

void FireBrosFireBall::exeAttach() {
    sead::Matrix34f attachMtx;
    BrosWeaponBase::calcAttachMtx(&attachMtx, getPoseMtx(), getTrans(), getRotation());
    al::updatePoseMtx(this, &attachMtx);
}*/

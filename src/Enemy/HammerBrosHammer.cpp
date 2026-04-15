#include "Enemy/HammerBrosHammer.h"

#include "Library/Collision/CollisionPartsKeeperUtil.h"
#include "Library/Collision/CollisionPartsTriangle.h"
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
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Shadow/ActorShadowUtil.h"

#include "Util/AreaUtil.h"
#include "Util/SensorMsgFunction.h"

namespace {
NERVE_IMPL(HammerBrosHammer, Break)
NERVE_IMPL(HammerBrosHammer, Move)
NERVE_IMPL(HammerBrosHammer, Attach)

NERVES_MAKE_STRUCT(HammerBrosHammer, Move, Break)
NERVES_MAKE_NOSTRUCT(HammerBrosHammer, Attach)
}  // namespace

HammerBrosHammer::HammerBrosHammer(const char* name, const al::LiveActor* bros,
                                   const char* archiveName, bool isnan)
    : BrosWeaponBase(name, bros), mArchiveName(archiveName), vboold(isnan) {}

void HammerBrosHammer::init(const al::ActorInitInfo& init) {
    al::initActorWithArchiveName(this, init, mArchiveName, nullptr);
    al::initNerve(this, &NrvHammerBrosHammer.Move, 0);
    al::setEffectNamedMtxPtr(this, "LavaSurface", &mLavaSurfaceMtx);
    al::invalidateHitSensors(this);
    al::offCollide(this);
    if (!vboold)
        al::invalidateHitSensor(this, "Search");
    makeActorDead();
}

void HammerBrosHammer::shoot(const sead::Vector3f& trans, const sead::Quatf& quat,
                             const sead::Vector3f& force, bool isHack, s32 unknown, bool isFast) {
    al::showModelIfHide(this);
    mIsHack = isHack;
    if (isHack)
        al::validateHitSensor(this, "AttackHack");
    else
        al::validateHitSensor(this, "Attack");

    if (mIsHack && vboold) {
        mcounter = 100;
        al::invalidateHitSensor(this, "Search");
    } else {
        al::validateHitSensor(this, "Search");
    }

    al::setQuat(this, quat);
    al::invalidateClipping(this);
    al::onCollide(this);

    sead::Vector3f frontDir = sead::Vector3f::zero;
    sead::Vector3f upDir = sead::Vector3f::zero;
    al::resetPosition(this, trans);
    al::calcFrontDir(&frontDir, this);
    al::calcUpDir(&upDir, this);


    sead::Vector3f frontDir2 = frontDir;
    sead::Vector3f upDir2 = upDir;
    al::Triangle triangle;
    sead::Vector3f hitPos = sead::Vector3f::zero;
    bool isColliding = alCollisionUtil::getFirstPolyOnArrow(
        this, &hitPos, &triangle, al::getTrans(this), frontDir * 100.0f, nullptr, nullptr);
    if (isHack) {
        sead::Vector3f crossDir;
        crossDir.setCross(upDir2, frontDir2);


        f32 randomVal = al::getRandom(0.0f, 20.0f);
        f32 fVar16 = 60.0f;
        f32 baseRand = 25.0f;
        if (!rs::isInLowGravityArea(this)) {
            baseRand = 20.0f;
            fVar16 = 40.0f;
        }

        if (!isFast)
            fVar16 = baseRand;

        fVar16 += (randomVal - 10.0f);

        sead::Vector3f upVelocity;
        f32 sideRandom;

        if (rs::isInLowGravityArea(this)) {
            sideRandom = al::getRandom(0.0f, 5.0f) - 2.5f;
            upVelocity = upDir * 40.0f;
        } else {
            f32 scale = sead::Mathi::clampMax(unknown, 5) * 8.0f;
            sideRandom = al::getRandom(0.0f, 23.0f) - 11.5f;
            upVelocity = upDir * scale;
        }

        sead::Vector3f forced = force;
        forced.y = 0.0f;

        al::setVelocity(this, (crossDir * sideRandom) + (frontDir* fVar16) + upVelocity + forced);
        al::faceToVelocity(this);

        sead::Vector3f brosVel = al::getVelocity(getBrosActor());
        brosVel.y = 0.0f;
        al::addVelocity(this, brosVel);

        if (isColliding)
            al::getTransPtr(this)->y += 80.0f;
    } else {
        f32 scale = rs::isInLowGravityArea(this) ? 20.0f : 25.0f;
        al::setVelocity(this, frontDir * scale + upDir * 50.0f);
    }
    al::showShadow(this);
    appear();
    al::setNerve(this, &NrvHammerBrosHammer.Move);
}

void HammerBrosHammer::killCollide(al::HitSensor* sensor, const sead::Vector3f& trans,
                                   bool isHack) {
    al::setTrans(this, trans);
    appear();
    al::HitSensor* attackSensor = al::getHitSensor(this, "Attack");
    if (isHack)
        rs::sendMsgHammerBrosHammerHackAttack(sensor, attackSensor);
    else
        rs::sendMsgHammerBrosHammerEnemyAttack(sensor, attackSensor);

    al::startHitReaction(this, "コリジョン衝突");
    al::setNerve(this, &NrvHammerBrosHammer.Break);
    al::hideModelIfShow(this);
}

void HammerBrosHammer::killEnemy() {
    al::setNerve(this, &NrvHammerBrosHammer.Break);
    al::hideModelIfShow(this);
}

void HammerBrosHammer::attach(const sead::Matrix34f* poseMtx, const sead::Vector3f& trans,
                              const sead::Vector3f& rotation, const char* actionName) {
    BrosWeaponBase::attach(poseMtx, trans, rotation, actionName);
    al::showModelIfHide(this);
    al::hideShadow(this);
    al::invalidateHitSensor(this, "Attack");
    al::invalidateHitSensor(this, "AttackHack");
    al::setNerve(this, &Attach);
}

bool HammerBrosHammer::isBreak() const {
    return al::isNerve(this, &NrvHammerBrosHammer.Break);
}

s32 HammerBrosHammer::getBreakStep() const {
    if (HammerBrosHammer::isBreak())
        return al::getNerveStep(this);
    return -1;
}

void HammerBrosHammer::attackSensor(al::HitSensor* self, al::HitSensor* other) {}

bool HammerBrosHammer::receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                                  al::HitSensor* self) {
    if (rs::isMsgPlayerDisregardTargetMarker(message))
        return true;

    if (HammerBrosHammer::isBreak())
        return false;

    if (rs::isMsgCapAttack(message)) {
        al::startHitReaction(this, "コリジョン衝突");
        al::setNerve(this, &NrvHammerBrosHammer.Break);
        al::hideModelIfShow(this);
        return false;
    }

    if ((rs::isMsgHammerBrosHammerHackAttack(message) && !mIsHack) ||
        rs::isMsgHammerBrosHammerEnemyAttack(message)) {
        rs::requestHitReactionToAttacker(message, self, other);
        al::startHitReaction(this, "コリジョン衝突");
        al::setNerve(this, &NrvHammerBrosHammer.Break);
        al::hideModelIfShow(this);
        return true;
    }

    return false;
}

void HammerBrosHammer::exeAttach() {
    sead::Matrix34f attachMtx;
    BrosWeaponBase::calcAttachMtx(&attachMtx, getPoseMtx(), getTrans(), getRotation());
    al::updatePoseMtx(this, &attachMtx);
}

void HammerBrosHammer::exeMove() {
    if (al::isFirstStep(this))
        al::startAction(this, "Spin");

    al::HitSensor* attackSensor = al::getHitSensor(this, "Attack");

    if (al::isCollidedGround(this)) {
        al::HitSensor* groundSensor = al::tryGetCollidedGroundSensor(this);
        if (groundSensor)
            al::sendMsgPlayerTouch(groundSensor, attackSensor);

        if (al::isCollidedGroundFloorCode(this, "DamageFire")) {
            mLavaSurfaceMtx.setTranslation(al::getCollidedGroundPos(this));
            al::startHitReaction(this, "ダメージ床(火)");
        }
    }

    al::HitSensor* sensor = al::tryGetCollidedSensor(this);
    if (sensor) {
        bool isHandled;
        if (mIsHack)
            isHandled = rs::sendMsgHammerBrosHammerHackAttack(sensor, attackSensor);
        else
            isHandled = rs::sendMsgHammerBrosHammerEnemyAttack(sensor, attackSensor);

        if (!isHandled)
            al::startHitReaction(this, "コリジョン衝突");

        al::setNerve(this, &NrvHammerBrosHammer.Break);
        al::hideModelIfShow(this);
        return;
    }

    if (al::isInDeathArea(this)) {
        al::startHitReaction(this, "コリジョン衝突");
        al::setNerve(this, &NrvHammerBrosHammer.Break);
        al::hideModelIfShow(this);
        return;
    }

    if (al::isGreaterEqualStep(this, 300)) {
        kill();
        return;
    }

    if (mcounter > 0)
        mcounter--;

    sead::Quatf quat = al::getQuat(this);
    al::rotateQuatXDirDegree(&quat, quat, 20.0f);
    al::setQuat(this, quat);

    al::scaleVelocity(this, rs::isInLowGravityArea(this) ? 0.985f : 0.98f);
    al::addVelocityToGravity(this, rs::isInLowGravityArea(this) ? 1.0f : 2.0f);
}

void HammerBrosHammer::exeBreak() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "Wait");
        al::setVelocityZero(this);
        al::invalidateHitSensors(this);
        al::hideModelIfShow(this);
    }

    if (al::isGreaterEqualStep(this, 50))
        kill();
}

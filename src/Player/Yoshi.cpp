#include "Player/Yoshi.h"

#include <prim/seadSafeString.h>

#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nature/WaterSurfaceFinder.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"

#include "Item/YoshiFruitWatcher.h"
#include "Player/PlayerAnimator.h"
#include "Player/PlayerColliderYoshi.h"
#include "Player/PlayerConst.h"
#include "Player/PlayerEffect.h"
#include "Player/PlayerFunction.h"
#include "Player/PlayerJumpMessageRequest.h"
#include "Player/PlayerModelChangerYoshi.h"
#include "Player/PlayerModelHolder.h"
#include "Player/PlayerPushReceiver.h"
#include "Player/PlayerTrigger.h"
#include "Player/YoshiJointControlKeeper.h"
#include "Player/YoshiStateEgg.h"
#include "Player/YoshiStateHack.h"
#include "Player/YoshiStateNpc.h"
#include "Util/NpcEventFlowUtil.h"
#include "Util/ObjUtil.h"
#include "Util/PlayerCollisionUtil.h"
#include "Util/PlayerHackFunction.h"
#include "Util/SensorMsgFunction.h"
#include "Util/YoshiUtil.h"

namespace {
NERVE_IMPL(Yoshi, Egg)
NERVE_IMPL(Yoshi, Npc)
NERVE_IMPL(Yoshi, Hack)

NERVES_MAKE_STRUCT(Yoshi, Egg, Npc, Hack)
}  // namespace

static inline void reset(Yoshi* actor, PlayerModelChangerYoshi* modelChanger,
                         PlayerColliderYoshi* collider, sead::Matrix34f* mtx) {
    modelChanger->killModel();
    al::updatePoseMtx(actor, mtx);
    al::resetPosition(actor);
    rs::resetCollision(collider);
    al::setNerve(actor, &NrvYoshi.Egg);
}

Yoshi::Yoshi(const char* name) : al::LiveActor(name) {}

void Yoshi::init(const al::ActorInitInfo& info) {
    al::initActorWithArchiveName(this, info, "PlayerActorYoshi", nullptr);
    al::onCollide(this);
    PlayerModelHolder* holder = new PlayerModelHolder(1);
    al::LiveActor* actor = new al::LiveActor("ヨッシーモデル");
    PlayerFunction::initYoshiModelActor(actor, info, "Yoshi");
    holder->registerModel(actor, "Normal");
    holder->set_18("Yoshi");

    holder->changeModel("Normal");
    mModelHolder = holder;

    mPlayerConst = PlayerFunction::createYoshiConst(this, "PlayerActorYoshi", nullptr);
    PlayerColliderYoshi* collider = new PlayerColliderYoshi(this);
    collider->init();
    mCollider = collider;

    mModelChanger = new PlayerModelChangerYoshi(this, mModelHolder);
    mAnimator =
        new PlayerAnimator(mModelHolder, PlayerFunction::createPlayerDitherAnimator(this, 200.0f));
    mEffect = new PlayerEffect(this, mModelHolder, nullptr);
    mTrigger = new PlayerTrigger();
    mJumpMessageRequest = new PlayerJumpMessageRequest();
    mPushReceiver = new PlayerPushReceiver(this);

    mWaterSurfaceFinder = new al::WaterSurfaceFinder(this);
    mCapTargetInfo =
        rs::createCapTargetInfoWithPlayerCollider(mModelHolder->getActor(), mCollider, nullptr);
    al::initNerve(this, &NrvYoshi.Egg, 3);
    mStateEgg =
        new YoshiStateEgg(info, this, mCollider, mPlayerConst, mWaterSurfaceFinder, mModelChanger);

    mStateNpc = new YoshiStateNpc(
        this, mPlayerConst, mCollider, mTrigger, mWaterSurfaceFinder, info, mAnimator,
        rs::initEventFlowForSystem(mModelHolder->getActor(), info, nullptr, nullptr, nullptr));
    mStateHack = new YoshiStateHack(info, this, mModelHolder->getActor(), mCapTargetInfo,
                                    mPlayerConst, mWaterSurfaceFinder, mTrigger, mModelChanger,
                                    mAnimator, mCollider, mEffect, mJumpMessageRequest);
    al::initNerveState(this, mStateEgg, &NrvYoshi.Egg, "卵");
    al::initNerveState(this, mStateNpc, &NrvYoshi.Npc, "Npc");
    al::initNerveState(this, mStateHack, &NrvYoshi.Hack, "キャプチャー");
    mHitSensor = al::getHitSensor(this, "Body");
    mJointControlKeeper = new YoshiJointControlKeeper(mModelHolder->getActor(), mCollider);
    makeActorAlive();
    al::makeMtxRT(&mLocalMtx, this);
}

void Yoshi::initAfterPlacement() {
    rs::noticeCurrentHackYoshi(this);
}

void Yoshi::movement() {
    mTrigger->clearPreMovementTrigger();
    mTrigger->clearReceiveSensorTrigger();
    mTrigger->clearActionTrigger();
    mTrigger->clearAttackSensorTrigger();
    mAnimator->updateAnimFrame();
    mStateHack->updatePrevMovement();
    movement();
    mModelChanger->syncHost();
    mPushReceiver->clear();
    mStateHack->updateAfterMovement();
    /*
     pIVar10 = (IUsePlayerCollision *)mCollider;
     pPVar2 = mPlayerConst;
     pPVar3 = mModelHolder;
     pWVar12 = mWaterSurfaceFinder;
     pIVar1 = (IUsePlayerHeightCheck *)0x0;
     if (pIVar10 != (IUsePlayerCollision *)0x0) {
       pIVar1 = (IUsePlayerHeightCheck *)(pIVar10 + 0x10);
     }
     uVar6 = rs::isCollidedGround(pIVar10);
     if ((uVar6 & 1) == 0) {
       uVar6 = rs::isCollidedWall(pIVar10);
       if ((uVar6 & 1) == 0) {
         uVar6 = al::isInFire(this);
         pcVar7 = "LavaWhite";
         if ((uVar6 & 1) == 0) {
           pcVar7 = "NoCollide";
         }
       }
       else {
         pcVar7 = (char *)rs::getMaterialCodeWall(pIVar10);
       }
     }
     else {
       pcVar7 = (char *)rs::getMaterialCodeGround(pIVar10);
     }
     fVar14 = (float)(**(code **)(*(long *)pPVar2 + 0xc78))(pPVar2);
     bVar4 = rs::isInPuddleHeight(pWVar12,pIVar1,fVar14);
     bVar5 = al::isInAreaObj(this,"WetArea");
     bVar8 = (bool)(bVar5 & (bVar4 ^ 0xff) & 1);
     al::updateMaterialCodeWet(this,bVar8);
     al::updateMaterialCodePuddle(this,(bool)(bVar4 & 1));
     al::setMaterialCode(this,pcVar7);
     pLVar13 = *(LiveActor **)(*(long *)(pPVar3 + 0x10) + 0x98);
     al::updateMaterialCodeWet(pLVar13,bVar8);
     al::updateMaterialCodePuddle(pLVar13,(bool)(bVar4 & 1));
     al::setMaterialCode(pLVar13,pcVar7);
     velocity.x = 0.0;
     velocity.y = 0.0;
     velocity.z = 0.0;
     this_00 = mStateNpc;*/

    sead::Vector3f at = {0.0f, 0.0f, 0.0f};
    if (al::isNerve(this, &NrvYoshi.Npc))
        if (mStateNpc->tryGetLookAtPlayerPos(&at)) {
            mJointControlKeeper->set_30(true);
            mJointControlKeeper->set_34(at);
        } else
            mJointControlKeeper->set_30(false);
    else if (al::isNerve(this, &NrvYoshi.Hack))
        mStateHack->tryGetLookAtTonguePos(&at);
    else
        mJointControlKeeper->set_30(false);

    mJointControlKeeper->set_28(mStateHack->isActiveHeadCorrection());

    f32 pose = 0.0f;
    f32 rate = 0.0f;
    mStateHack->calcGroundPoseRate(&pose, &rate);
    mJointControlKeeper->set_40(pose, rate);
}

void Yoshi::attackSensor(al::HitSensor* self, al::HitSensor* other) {
    if ((!al::isNerve(this, &NrvYoshi.Hack) || !mStateHack->attackSensor(self, other)) &&
        al::isNerve(this, &NrvYoshi.Npc) && rs::isSensorTypeYoshiEnableSendPush(self) &&
        !al::sendMsgPush(other, self)) {
        rs::sendMsgPushToPlayer(other, self);
    }
}

bool Yoshi::receiveMsg(const al::SensorMsg* message, al::HitSensor* other, al::HitSensor* self) {
    if (al::isMsgPlayerDisregard(message)) {
        if (al::isNerve(this, &NrvYoshi.Hack))
            return true;
        if (al::isNerve(this, &NrvYoshi.Npc))
            return !al::isSensorName(self, "Head");
    }

    PlayerColliderYoshi* collider = mCollider;
    if (rs::isSensorTypeYoshiMsgReceivable(self) &&
        (al::isNerve(this, &NrvYoshi.Npc) ?
             (!rs::isCollidedGround(collider) || rs::isJustLand(collider)) :
             al::isNerve(this, &NrvYoshi.Hack))) {
        f32 power = al::isNerve(this, &NrvYoshi.Hack) ? mPlayerConst->getPushPower() :
                                                        mPlayerConst->getPushPower() * 5.0f;

        if (mPushReceiver->receivePushMsgHacker(message, other, self, power, true))
            return true;
    } else if (al::isNerve(this, &NrvYoshi.Npc)) {
        if (mPushReceiver->receivePushMsgYoshiNpc(message, other, self,
                                                  mPlayerConst->getPushPower() * 5.0f))
            return true;
    }

    if (al::isNerve(this, &NrvYoshi.Hack)) {
        bool state = false;
        if (mStateHack->receiveMsgEndState(&state, message, other, self)) {
            if (state) {
                reset(this, mModelChanger, mCollider, &mLocalMtx);
                return true;
            }
            mTrigger->set(PlayerTrigger::ECollisionTrigger_val10);
            al::setNerve(this, &NrvYoshi.Npc);
            return true;
        }
    }

    if (al::isNerve(this, &NrvYoshi.Npc) && al::isGreaterEqualStep(this, 15) &&
        mStateHack->receiveMsgSetNerveState(message, other, self)) {
        al::setNerve(this, &NrvYoshi.Hack);
        return true;
    }

    if (al::isNerve(this, &NrvYoshi.Npc) &&
        mStateNpc->receiveMsgReturnEggAndInitPosition(message, other, self)) {
        reset(this, mModelChanger, mCollider, &mLocalMtx);
        return true;
    }

    if (al::isNerve(this, &NrvYoshi.Hack))
        return mStateHack->receiveMsg(message, other, self);

    if (al::isNerve(this, &NrvYoshi.Npc))
        return mStateNpc->receiveMsg(message, other, self);

    return false;
}

void Yoshi::control() {
    mAnimator->updateModel();
    if (mAnimator->isSubAnimPlaying() && mAnimator->isSubAnimEnd())
        mAnimator->endSubAnim();
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

void Yoshi::updateCollider() {
    al::updatePoseTrans(this, al::getTrans(this));

    YoshiStateHack* stateHack = mStateHack;
    if (al::isNerve(this, &NrvYoshi.Hack)) {
        if (stateHack->isCollisionShapeTongueJump())
            mCollider->tryChangeCollisionTongueJump();
        else
            mCollider->tryChangeCollisionNormal();
    } else {
        mCollider->tryChangeCollisionNormal();
    }

    sead::Vector3f velocity = al::getVelocity(this);
    f32 force = 0.0f;
    sead::Vector3f pullDirection = {0.0f,0.0f,0.0f};

    if (mStateHack->tryCalcTonguePullForce(&force, &pullDirection)) {
        if (!rs::isOnGround(this, mCollider)) {
            f32 leng = velocity.length();
            f32 dot = velocity.dot(pullDirection);

            if (!al::isNearZero(dot, 0.001f)) {
                sead::Vector3f normalVel;
                al::normalize(&normalVel, velocity);
                f32 ndot = sead::Mathf::abs(normalVel.dot(pullDirection));
                f32 velAlongPull = leng * ndot;
                dot = sead::Mathf::clamp(dot, force, velAlongPull);
            }

            // Matches fmin s0, s10, s0 and fmaxnm
            dot = sead::Mathf::max(dot, sead::Mathf::min(force, 40.0f));

            f32 newLeng = sead::Mathf::sqrt(sead::Mathf::max(0.0f, (leng * leng) - (dot * dot)));
            al::verticalizeVec(&velocity, pullDirection, velocity);
            normalize2(&velocity, newLeng);
            velocity += (pullDirection * dot);
        } else {
            al::getGravity(this);
            sead::Vector3f gravity =rs::getCollidedGroundNormal(mCollider);
            sead::Vector3f groundNormal = gravity;

            if (!al::isParallelDirection(gravity, pullDirection, 0.01f)) {
                al::verticalizeVec(&groundNormal, gravity, pullDirection);
                al::normalize(&groundNormal);
            }

            f32 dotPull = groundNormal.dot(pullDirection);
            sead::Vector3f vecH = {0.0f, 0.0f, 0.0f};
            sead::Vector3f vecY = {0.0f, 0.0f, 0.0f};
            al::separateVectorHV(&vecH, &vecY, gravity, velocity);

            f32 lenH = vecH.length();
            f32 dotLimit = force * dotPull;
            f32 lengNV = groundNormal.dot(vecH);

            if (!al::isNearZero(lengNV, 0.001f)) {
                if (lengNV >= 0.0f) {
                    al::verticalizeVec(&velocity, groundNormal, vecH);
                    lengNV = sead::Mathf::max(lengNV, sead::Mathf::min(dotLimit, 40.0f));
                    velocity += (groundNormal * lengNV);
                    normalize2(&velocity, lenH);
                } else {
                    sead::Vector3f normalVecH;
                    al::normalize(&normalVecH, vecH);
                    f32 ndot = sead::Mathf::abs(normalVecH.dot(groundNormal));
                    f32 velAlongPull = lenH * ndot;
                    f32 limit = sead::Mathf::clampMax(velAlongPull, 40.0f);
                    lengNV = sead::Mathf::clamp(lengNV, dotLimit, velAlongPull);

                    f32 v53 = sead::Mathf::sqrt(sead::Mathf::max(0.0f, (lenH * lenH) - (lengNV * lengNV)));
                    al::verticalizeVec(&velocity, groundNormal, vecH);
                    normalize2(&velocity, v53);
                    velocity += (groundNormal * lengNV);
                }
            } else {
                al::verticalizeVec(&velocity, groundNormal, vecH);
                velocity += (groundNormal * sead::Mathf::min(dotLimit, 40.0f));
                normalize2(&velocity, lenH);
            }
            velocity += vecY;
        }
    }

    if (mTrigger->isOnCollisionExpandCheck())
        rs::resetCollisionExpandCheck(mCollider);

    sead::Vector3f pushedVel = {0.0f, 0.0f, 0.0f};
    mPushReceiver->calcPushedVelocity(&pushedVel, velocity);

    if (mStateHack->isEnableUpdateCollider())
        rs::updateCollider(this, mCollider, pushedVel);

    al::updatePoseTrans(this, al::getTrans(this));
    mWaterSurfaceFinder->update(al::getTrans(this), -al::getGravity(this), 200.0f);
    mEffect->updateWaterSurfaceMtx(mWaterSurfaceFinder);
    sendCollisionMsg();
}

void Yoshi::updateCollisionShape() {
    YoshiStateHack* stateHack = mStateHack;
    if (!al::isNerve(this, &NrvYoshi.Hack)) {
        mCollider->tryChangeCollisionNormal();
        return;
    }

    if (stateHack->isCollisionShapeTongueJump()) {
        mCollider->tryChangeCollisionTongueJump();
        return;
    }

    mCollider->tryChangeCollisionNormal();
}

void Yoshi::sendCollisionMsg() {
    mJumpMessageRequest->clear();
    mTrigger->clearCollisionTrigger();
    if (rs::sendPlayerCollisionUpperPunchMsg(this, mHitSensor, mCollider, mModelChanger))
        mTrigger->set(PlayerTrigger::ECollisionTrigger_val0);

    rs::sendPlayerCollisionTouchMsg(this, mHitSensor, mCollider);
    if (!mStateHack->reactionCollidedCollisionCode()) {
        if (!mStateNpc->reactionCollidedCollisionCode() &&
            !mStateEgg->reactionCollidedCollisionCode()) {
            return;
        }
        reset(this, mModelChanger, mCollider, &mLocalMtx);
        return;
    }
    mTrigger->set(PlayerTrigger::ECollisionTrigger_val10);
    al::setNerve(this, &NrvYoshi.Npc);
}

void Yoshi::appearEgg() {
    al::setNerve(this, &NrvYoshi.Egg);
    rs::resetCollision(mCollider);
    appear();
    al::invalidateClipping(this);
}

void Yoshi::startFruitShineGetDemo() {
    mStateHack->startFruitShineGetDemo();
}

void Yoshi::exeEgg() {
    if (al::updateNerveState(this))
        al::setNerve(this, &NrvYoshi.Npc);
}

void Yoshi::exeNpc() {
    if (al::updateNerveState(this))
        reset(this, mModelChanger, mCollider, &mLocalMtx);
}

void Yoshi::exeHack() {
    al::updateNerveState(this);
}

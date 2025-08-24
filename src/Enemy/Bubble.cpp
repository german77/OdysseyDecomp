#include "Enemy/Bubble.h"

#include "Library/Bgm/BgmLineFunction.h"
#include "Library/Camera/CameraUtil.h"
#include "Library/Collision/CollisionParts.h"
#include "Library/Collision/CollisionPartsKeeperUtil.h"
#include "Library/Effect/EffectSystemInfo.h"
#include "Library/Joint/JointSpringControllerHolder.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorAnimFunction.h"
#include "Library/LiveActor/ActorAreaFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorCollisionFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/Nature/NatureUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Rail/RailUtil.h"
#include "Library/Shadow/ActorShadowUtil.h"
#include "Library/Stage/StageSwitchUtil.h"

#include "Enemy/BubbleStateInLauncher.h"
#include "Util/Hack.h"
#include "Util/InputInterruptTutorialUtil.h"

namespace {
NERVE_IMPL(Bubble, StandBy)
NERVE_IMPL(Bubble, Delay)
NERVE_IMPL(Bubble, RailMove)
NERVE_IMPL(Bubble, Ready)
NERVE_IMPL(Bubble, Up)
NERVE_IMPL(Bubble, Turn)
NERVE_IMPL(Bubble, Down)
NERVE_IMPL(Bubble, Sink)
NERVE_IMPL(Bubble, Wait)
NERVE_IMPL(Bubble, Die)
NERVE_IMPL(Bubble, WaitHack)
NERVE_IMPL(Bubble, WaitHackStart)
NERVE_IMPL(Bubble, WaitHackFall)
NERVE_IMPL(Bubble, HackFall)
NERVE_IMPL(Bubble, HackMove)
NERVE_IMPL(Bubble, HackJump)
NERVE_IMPL_(Bubble, HackJumpHigh, HackJump)
NERVE_IMPL_(Bubble, HackCancelJump, HackJump)
NERVE_IMPL(Bubble, HackLand)
NERVE_IMPL(Bubble, HackInLauncher)
NERVE_IMPL(Bubble, HackResetPos)
NERVE_IMPL(Bubble, HackDemo)
NERVE_IMPL(Bubble, Revive)

NERVES_MAKE_STRUCT(Bubble, StandBy, HackInLauncher, Revive, WaitHackFall, Delay, Up, Turn, Down,
                   Die, HackJump, HackJumpHigh, HackCancelJump, HackFall, HackResetPos, HackDemo,
                   HackMove, WaitHack, WaitHackStart, Ready, Wait, Sink, HackLand, RailMove)
}  // namespace

Bubble::Bubble(const char* name) : al::LiveActor{name} {}

void Bubble::init(const al::ActorInitInfo& info) {}

void Bubble::initAfterPlacement() {
    checkEffectPos();
    if (al::isNoCollide(this) && !mIsWaveCheckOn)
        al::tryUpdateEffectMaterialCode(this, mMaterialCode);
}

void Bubble::attackSensor(al::HitSensor* self, al::HitSensor* other) {}

bool Bubble::receiveMsg(const al::SensorMsg* message, al::HitSensor* other, al::HitSensor* self) {}

void Bubble::control() {}

void Bubble::updateCollider() {}

void Bubble::appear() {
    al::LiveActor::appear();
    al::setNerve(this, &NrvBubble.StandBy);
    al::startVisAnim(this, "CapOff");

    if (!al::isValidSwitchStart(this))
        start();
}

void Bubble::calcAnim() {
    al::LiveActor::calcAnim();

    sead::Matrix34f mtx;
    mtx.setInverse(*getBaseMtx());
    sead::Vector3f offset = mShadowMaskOffset;
    offset.rotate(mtx);
    al::setShadowMaskOffset(this, offset, "body");
}

void Bubble::recalcClippingInfo() {}

void Bubble::start() {
    if (!al::isNerve(this, &NrvBubble.StandBy))
        return;

    al::setTrans(this, vecA);
    al::hideModelIfShow(this);
    valCount = 0;

    if (mDelayFrameNum == 0)
        al::setNerve(this, &NrvBubble.Ready);
    else
        al::setNerve(this, &NrvBubble.Delay);
}

void Bubble::killAll() {
    al::setNerve(this, &NrvBubble.Die);
}

bool Bubble::isIgnoreTriangle(const al::Triangle& triangle) const {
    return al::isFloorCode(triangle, "Fence");
}

void Bubble::checkEffectPos() {}

bool Bubble::isValidCollisionOrWaveCheck() const {
    if (!al::isNoCollide(this))
        return true;
    return mIsWaveCheckOn;
}

bool Bubble::isHack() const {
    return (al::isNerve(this, &NrvBubble.WaitHackStart) &&
            rs::isHackStartDemoEnterMario(mPlayerHack)) ||
           al::isNerve(this, &NrvBubble.HackFall) || al::isNerve(this, &NrvBubble.HackMove) ||
           al::isNerve(this, &NrvBubble.HackJump) || al::isNerve(this, &NrvBubble.HackJumpHigh) ||
           al::isNerve(this, &NrvBubble.HackCancelJump) || al::isNerve(this, &NrvBubble.HackLand) ||
           al::isNerve(this, &NrvBubble.HackResetPos) || al::isNerve(this, &NrvBubble.HackDemo) ||
           (al::isNerve(this, &NrvBubble.HackInLauncher) && mPlayerHack);
}

void Bubble::revive() {
    if (al::isNerve(this, &NrvBubble.Revive))
        return;

    if (al::isExistRail(this)) {
        al::setRailPosToStart(this);
        if (!al::isRailGoingToEnd(this))
            al::reverseRail(this);
    }
    al::offCollide(this);
    al::validateClipping(this);
    onGroupClipping();
    al::hideModelIfShow(this);
    al::setVelocityZero(this);
    al::setNerve(this, &NrvBubble.Revive);
}

void Bubble::updateLavaWave() {}

void Bubble::updateScrollAnimRate() {
    if (al::isActionPlaying(this, mActionName))
        return;

    mActionName = al::getActionName(this);

    if (isHack() &&
        (al::isActionPlaying(this, "BubbleCannonJump") ||
         al::isActionPlaying(this, "HackHighJump") || al::isActionPlaying(this, "HackWaitSpeedy") ||
         al::isActionPlaying(this, "MoveSpeedy"))) {
        al::setMtsAnimFrameRate(this, 2.0f);
        return;
    }

    al::setMtsAnimFrameRate(this, 1.0f);
}

bool Bubble::isOnGround() const {
    if (al::isOnGround(this, 0))
        return true;
    return isB;
}

void Bubble::setupHack() {
    if (al::isNoCollide(this) && !mIsWaveCheckOn) {
        s32 uVar3;
        if (!al::isExistRail(this))
            uVar3 = 0;
        else
            uVar3 = !al::isRailGoingToEnd(this);
        f32 fVar6 = maxD.m[uVar3 * 3 + 7][2];
        fVar6 = fVar6 + al::getColliderRadius(this);
        if (al::getTrans(this).y < fVar6)
            al::getTransPtr(this)->y = fVar6;
    }
    al::invalidateClipping(this);
    offGroupClipping();
    vecPos = {0.0f, 0.0f, 0.0f};
    al::onCollide(this);
    valC = 5;
    al::setVelocityZero(this);
    isF = false;
    mJointSpringControllerHolder->onControlAll();
}

bool Bubble::isCurrentNerveEnableLockOnAndStartHack() const {}

void Bubble::offGroupClipping() {
    if (mIsGroupClipping) {
        al::offGroupClipping(this);
        mIsGroupClipping = false;
    }
}

void Bubble::startHackLocal(al::HitSensor* self, al::HitSensor* other) {
    setupHack();
    mPlayerHack = rs::startHack(self, other, nullptr);
    rs::startHackStartDemo(mPlayerHack, this);
    al::setCameraTarget(this, mCameraTargetBase);
    al::setNerve(this, &NrvBubble.WaitHackStart);
}

bool Bubble::isInvalidHackEscape() const {
    if (isHack()) {
        if (al::isInAreaObj(this, "HackInvalidEscapeArea"))
            return true;
        if (al::isNerve(this, &NrvBubble.HackResetPos))
            return true;
    }
    return false;
}

void Bubble::prepareEndHack() {
    al::startHitReaction(this, "憑依解除(通常)");
    rs::tryEndHackStartDemo(mPlayerHack, this);
    al::showModelIfHide(this);
}

void Bubble::endHackByCancel() {
    al::endBgmSituation(this, "HackBubble", false);
    if (mCameraTicket && al::isActiveCamera(mCameraTicket)) {
        al::endCamera(this, mCameraTicket, -1, false);
        mCameraTicket = nullptr;
    }
    endHackCommon();
    if (al::isNerve(this, &NrvBubble.HackInLauncher) && mBubbleStateInLauncher->isFlying())
        return;
    al::setNerve(this, &NrvBubble.WaitHackFall);
}

void Bubble::forceEndHack() {
    if (mPlayerHack) {
        al::startHitReaction(this, "憑依解除(着地強制)");
        endHackCommon();
    }
    al::setVelocityZero(this);
    revive();
}

void Bubble::updatePosOnWave() {
    if (!al::isNoCollide(this) || mIsWaveCheckOn)
        updateLavaWave();
}

void updateVelocityIfValidCollision() {}

bool Bubble::isOnDamageFire() const {
    if (isB)
        return true;
    return al::isCollidedGroundFloorCode(this, "DamageFire");
}

void Bubble::tryStartHitReactionUp() {}

void Bubble::updateShadowMaskLength() {
    f32 newLength =
        sead::Mathf::clamp(al::getTrans(this).y - vecA.y + 300.0f, 300.0f, mShadowMaskDropLength);
    al::setShadowMaskDropLength(this, newLength);
}

void Bubble::tryStartHitReactionDown() {}

void Bubble::shiftSink() {}

void Bubble::headUp() {}

void Bubble::shiftReadyOrMove() {
    if (!al::isExistRail(this) || !al::isNoCollide(this))
        al::setNerve(this, &NrvBubble.Ready);
    else
        al::setNerve(this, &NrvBubble.RailMove);
}

void Bubble::trySendMsgStartInSaucePan() {}

void Bubble::tryHitReactionThroughFence() {}

void Bubble::tryShiftLand() {}

bool Bubble::isHoldHackAction() const {}

void Bubble::tryBoundMoveWall() {}

void Bubble::updateHackOnGround() {}

void Bubble::constrainLavaDomain() {}

void Bubble::tryShiftFall() {}

bool Bubble::isTriggerHackSwing() const {
    if (isF)
        return false;
    return rs::isTriggerHackSwing(mPlayerHack);
}

void Bubble::faceToMoveVec() {}

bool Bubble::isTriggerHackJump() const {}

void Bubble::revertTargetQuatInHackJump(sead::Quatf*, sead::Quatf*) {}

void Bubble::calcHackerMoveVec(sead::Vector3f*, const sead::Vector3f&) const {}

void Bubble::makeDisplayQuatInHackJump(const sead::Quatf&, const sead::Quatf&, const sead::Quatf&,
                                       bool) {}

bool Bubble::isDropAttackCollision() const {
    if (al::getVelocity(this).y > 0.0f)
        return false;

    if (!al::isOnGround(this, 0))
        return false;

    const sead::Vector3f& groundPos = al::getCollidedGroundPos(this);
    const sead::Vector3f& trans = al::getTrans(this);
    return !(groundPos.y - trans.y >= 0.0f);
}

bool Bubble::isRiseAttackCollision() const {
    if (al::getVelocity(this).y < 0.0f || !al::isCollidedCeiling(this))
        return false;

    const sead::Vector3f& ceilingPos = al::getCollidedCeilingPos(this);
    const sead::Vector3f& trans = al::getTrans(this);
    return !(ceilingPos.y - trans.y <= 0.0f);
}

bool Bubble::isHoldHackJump() const {
    if (isF)
        return false;
    return rs::isHoldHackJump(mPlayerHack);
}

void Bubble::tryShiftContinuousJump() {}

bool Bubble::calcHackerMoveDir(sead::Vector3f* moveDir, const sead::Vector3f& inputDir) const {
    if (isF) {
        moveDir->set({0.0f, 0.0f, 0.0f});
        return false;
    }
    return rs::calcHackerMoveDir(moveDir, mPlayerHack, inputDir);
}

bool Bubble::isWaitingLaunch() const {
    if (!al::isNerve(this, &NrvBubble.HackInLauncher))
        return false;

    return mBubbleStateInLauncher->isWaiting();
}

void Bubble::launch(const sead::Vector3f& dir, float force, al::CameraTicket* cameraTicket) {
    if (!al::isNerve(this, &NrvBubble.HackInLauncher))
        return;

    mBubbleStateInLauncher->shoot(dir, force);
    if (cameraTicket)
        mCameraTicket = cameraTicket;
    rs::hideTutorial(this);
    mIsShowTutorial = true;
}

void Bubble::launchCancel(const sead::Vector3f&) {}

void Bubble::showHackCap() {
    rs::showHackCap(mPlayerHack);
}

void Bubble::hideHackCap() {
    rs::hideHackCap(mPlayerHack);
}

bool Bubble::isTriggerJump() const {
    if (!mPlayerHack)
        return false;
    if (isF)
        return false;
    if (rs::isTriggerHackPreInputJump(mPlayerHack))
        return true;

    return isTriggerHackSwing();
}

bool Bubble::isTriggerAction() const {
    if (!mPlayerHack)
        return false;
    return isTriggerHackAction();
}

bool Bubble::isTriggerHackAction() const {
    if (isF)
        return false;
    return rs::isTriggerHackAction(mPlayerHack);
}

bool Bubble::isTriggerCancel() const {
    if (!mPlayerHack)
        return false;

    return HackFunction::isTriggerCancelBubbleLauncher(mPlayerHack);
}

void Bubble::startJumpAnim(float) {}

void Bubble::resetAndAppear(const sead::Vector3f&, const sead::Quatf&, float) {}

void Bubble::endHackCommon() {}

void Bubble::calcLaunchPos(sead::Vector3f*, const sead::Vector3f&, float, float) const {}

void Bubble::onGroupClipping() {
    if (!mIsGroupClipping) {
        al::onGroupClipping(this);
        mIsGroupClipping = true;
    }
}

bool Bubble::isOnGroundNoVelocity() const {
    if (al::isOnGroundNoVelocity(this, 0))
        return true;

    return isB;
}

void Bubble::updateCollisionPartsMove() {
    al::CollisionParts* collisionParts = al::tryGetCollidedGroundCollisionParts(this);
    if (!collisionParts) {
        if (isB || valFall > 9)
            vecCC = {0.0f, 0.0f, 0.0f};
        return;
    }
    sead::Vector3f delta = al::getTrans(this);
    delta.rotate(collisionParts->getBaseInvMtx());
    delta.rotate(collisionParts->getBaseMtx());
    vecCC = delta - al::getTrans(this);
}

void Bubble::accelStick() {}

bool Bubble::addHackActorAccelStick(sead::Vector3f* stickAccel, float scale,
                                    const sead::Vector3f& dir) {
    if (isF) {
        stickAccel->set({0.0f, 0.0f, 0.0f});
        return false;
    }
    return rs::addHackActorAccelStick(this, mPlayerHack, stickAccel, scale, dir);
}

bool Bubble::isGroundOverTheWave(bool checkHeight, const sead::Vector3f& wavePos) const {
    if (al::isOnGround(this, 0)) {
        if (!checkHeight)
            return true;

        if (wavePos.y <
            al::getTrans(this).y - al::getColliderRadius(this) + al::getColliderOffsetY(this)) {
            return true;
        }
    }

    return false;
}

bool Bubble::isEnableSnapWaveSurface() const {
    return isB && !al::isNerve(this, &NrvBubble.HackJump) &&
           !al::isNerve(this, &NrvBubble.HackJumpHigh) &&
           !al::isNerve(this, &NrvBubble.HackCancelJump) && !al::isNerve(this, &NrvBubble.Down);
}

void Bubble::exeStandBy() {}

void Bubble::exeDelay() {
    if (al::isGreaterEqualStep(this, mDelayFrameNum))
        al::setNerve(this, &NrvBubble.Ready);
}

void Bubble::exeRailMove() {
    if (al::isFirstStep(this))
        al::startAction(this, "Move");

    al::tryAddRippleSmall(this);
    f32 nerveRate = al::calcNerveRate(this, mRailMoveFrame);
    if (!al::isRailGoingToEnd(this))
        nerveRate = 1.0 - nerveRate;
    al::setSyncRailToCoord(this, bloatRail * nerveRate);

    if (!al::isNoCollide(this) || mIsWaveCheckOn)
        updateLavaWave();

    if (al::isGreaterEqualStep(this, mRailMoveFrame)) {
        al::setQuat(this, al::isRailGoingToEnd(this) ? quat1 : quatA);
        al::reverseRail(this);
        al::setNerve(this, &NrvBubble.Ready);
    }
}

void Bubble::exeReady() {}

void Bubble::exeUp() {}

void Bubble::exeTurn() {
    al::isFirstStep(this);
    if (al::isGreaterEqualStep(this, 4))
        al::setNerve(this, &NrvBubble.Down);
}

void Bubble::exeDown() {}

void Bubble::exeSink() {}

void Bubble::exeWait() {}

void Bubble::exeDie() {
    al::startHitReaction(this, "死亡");
    kill();
}

void Bubble::exeWaitHack() {
    setVelocityZero(this);
}

void Bubble::exeWaitHackStart() {}

void Bubble::endWaitHackStart() {
    mJointSpringControllerHolder->onControlAll();
}

void Bubble::exeWaitHackFall() {}

void Bubble::exeHackFall() {}

void Bubble::exeHackMove() {}

void Bubble::endHackMove() {}

void Bubble::exeHackJump() {}

void Bubble::endHackJump() {
    al::setQuat(this, quatD);
}

void Bubble::exeHackLand() {}

void Bubble::exeHackInLauncher() {}

void Bubble::endHackInLauncher() {
    if (mCameraTicket) {
        al::endCamera(this, mCameraTicket, -1, false);
        mCameraTicket = nullptr;
    }

    if (mIsShowTutorial) {
        rs::showTutorial(this);
        mIsShowTutorial = false;
    }
}

void Bubble::exeHackResetPos() {}

void Bubble::exeHackDemo() {}

void Bubble::exeRevive() {}

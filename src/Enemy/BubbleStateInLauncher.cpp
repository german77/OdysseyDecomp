#include "Enemy/BubbleStateInLauncher.h"

#include "Library/Bgm/BgmLineFunction.h"
#include "Library/Bgm/BgmParamsChanger.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"

#include "Enemy/Bubble.h"
#include "Util/InputInterruptTutorialUtil.h"

namespace {
NERVE_IMPL(BubbleStateInLauncher, Enter)
NERVE_IMPL(BubbleStateInLauncher, Wait)
NERVE_IMPL(BubbleStateInLauncher, Shoot)

NERVES_MAKE_NOSTRUCT(BubbleStateInLauncher, Enter, Wait, Shoot)
}  // namespace

BubbleStateInLauncher::BubbleStateInLauncher(Bubble* bubble)
    : al::HostStateBase<Bubble>("バブルキャノン状態", bubble) {
    initNerve(&Enter, 0);
    mIBgmParamsChanger = new sead::PtrArray<al::IBgmParamsChanger>();
    mIBgmParamsChanger->allocBuffer(5, nullptr);
}

void BubbleStateInLauncher::appear() {
    NerveStateBase::appear();
    al::setNerve(this, &Enter);
}

void BubbleStateInLauncher::kill() {
    NerveStateBase::kill();
    if (isWaiting())
        rs::resetHackTutorialText(getHost());
    if (mIBgmParamsChanger)
        al::endBgmSituation(getHost(), mIBgmParamsChanger, "BubbleLauncherShoot", true);
}

bool BubbleStateInLauncher::isWaiting() const {
    return al::isNerve(this, &Wait);
}

bool BubbleStateInLauncher::isFlying() const {
    return al::isNerve(this, &Shoot);
}

void BubbleStateInLauncher::shoot(const sead::Vector3f& direction, f32 force) {
    mShootDirection.set(direction);
    mShootForce = force;
    al::showModelIfHide(getHost());
    getHost()->showHackCap();
    rs::resetHackTutorialText(getHost());
    al::setNerve(this, &Shoot);
}

bool calcLaunchPosImpl(sead::Vector3f* outPos, sead::Vector3f* outDir, f32* outForce, float angle,
                       const al::LiveActor* actor, const sead::Vector3f& endPos) {
    outPos->set(al::getTrans(actor));
    sead::Vector3f shootDirection = endPos - *outPos;
    f32 yAxis = sead::Mathf::tan(sead::Mathf::deg2rad(angle));
    f32 length = sead::Mathf::sqrt(sead::Mathf::square(shootDirection.x) +
                                   sead::Mathf::square(shootDirection.z));
    f32 yDir = 2 * (yAxis * length - shootDirection.y);
    if (yDir <= 0.0f)
        return false;

    yDir = 0.8f / yDir;
    shootDirection.y = 0.0f;
    al::tryNormalizeOrZero(&shootDirection);
    f32 forceH = length * sead::Mathf::sqrt(yDir);
    f32 shootDirectionV = yAxis * forceH;

    outDir->set(shootDirection * forceH + shootDirectionV * sead::Vector3f::ey);
    *outForce = length / forceH;
    return true;
}

void BubbleStateInLauncher::calcLaunchPos(sead::Vector3f* outPos, const sead::Vector3f& endPos,
                                          f32 angle, s32 frame) const {
    sead::Vector3f pos;
    sead::Vector3f dir;
    f32 force;
    if (!calcLaunchPosImpl(&pos, &dir, &force, angle, getHost(), endPos)) {
        outPos->set(al::getTrans(getHost()));
        return;
    }

    s32 ratered = (s32)force == force ? 0 : -!(force >= 0.0f);
    ratered += (s32)force;

    if (frame < 0) {
        s32 iVar2 = 0;
        if (ratered != 0)
            iVar2 = ((frame >> 0x1f ^ frame) - (frame >> 0x1f)) / ratered;
        frame += (iVar2 + 1) * ratered;
    }

    s32 rated = 0;
    if(ratered != 0){
        rated=frame / ratered;
    }
    f32 rate = frame - rated * ratered;

    outPos->set(pos + rate * dir);
    outPos->y += rate * rate * -0.4f;
}

void BubbleStateInLauncher::exeEnter() {
    al::setVelocityZero(getHost());
    al::hideModelIfShow(getHost());
    getHost()->hideHackCap();
    rs::changeHackTutorialText(getHost(), "Launcher");
    al::setNerve(this, &Wait);
}

void BubbleStateInLauncher::exeWait() {
    if (al::isFirstStep(this))
        al::startBgmSituation(getHost(), "HackBubble", false);
}

void BubbleStateInLauncher::exeShoot() {
    if (al::isFirstStep(this)) {
        al::startAction(getHost(), "BubbleCannonJump");
        al::endBgmSituation(getHost(), "HackBubble", false);
        al::startBgmSituation(getHost(), mIBgmParamsChanger, "BubbleLauncherShoot", true, false);
        for (s32 i = 0; i < mIBgmParamsChanger->size(); i++)
            mIBgmParamsChanger->at(i)->changeVolume(0.1f, 180);

        if (!calcLaunchPosImpl(&mTrans, &mDirection, &mForce, mShootForce, getHost(),
                               mShootDirection)) {
            kill();
            return;
        }
        al::faceToTarget(getHost(), mShootDirection);
    }
    f32 nextStep = al::getNerveStep(this) + 1;

    sead::Vector3f velocity= mTrans+nextStep * mDirection - al::getTrans(getHost());
    velocity.y += nextStep * nextStep * -0.4f;
    al::setVelocity(getHost(), velocity);

    sead::Vector3f actorVelocity = al::getVelocity(getHost());
    if (al::tryNormalizeOrZero(&actorVelocity)) {
        sead::Vector3f frontDir;
        al::calcFrontDir(&frontDir, getHost());
        sead::Quatf quat;
        al::makeQuatRotationRate(&quat, frontDir, actorVelocity, 1.0f);

        al::setQuat(getHost(), quat*al::getQuat(getHost()));
    }

    if (actorVelocity.y < 0.0f && 0.0 <= mFindMe)
        for (s32 i = 0; i < mIBgmParamsChanger->size(); i++)
            mIBgmParamsChanger->at(i)->changeVolume(1.0f, 120);

    mFindMe = actorVelocity.y;
    // Remind me on how to get the fractional part
    s32 step = (s32)mForce == mForce ? 0 : -!(mForce >= 0.0f);
    step += (s32)mForce;
    if (al::isGreaterEqualStep(this, step + 5))
        kill();
}

#pragma once

#include <basis/seadTypes.h>
#include <math/seadMatrix.h>
#include <math/seadQuat.h>
#include <math/seadVector.h>

#include "Library/LiveActor/LiveActor.h"

namespace al {
struct ActorInitInfo;
class CameraTicket;
class HitSensor;
class SensorMsg;
class Triangle;
class CameraTargetBase;
class JointSpringControllerHolder;
}  // namespace al

class CapTargetInfo;
class IUsePlayerHack;
class DisregardReceiver;
class PlayerHackStartShaderCtrl;
class AnimScaleController;
class CollisionPartsFilterSpecialPurpose;
class BubbleStateInLauncher;

class Bubble : public al::LiveActor {
public:
    Bubble(const char*);

    void init(const al::ActorInitInfo& info) override;
    void initAfterPlacement() override;
    void attackSensor(al::HitSensor* self, al::HitSensor* other) override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;
    void control() override;
    void updateCollider() override;
    void appear() override;
    void calcAnim() override;

    void recalcClippingInfo();
    void start();
    void killAll();
    bool isIgnoreTriangle(const al::Triangle&) const;
    void checkEffectPos();
    bool isValidCollisionOrWaveCheck() const;
    bool isHack() const;
    void revive();
    void updateLavaWave();
    void updateScrollAnimRate();
    bool isOnGround() const;
    void setupHack();
    bool isCurrentNerveEnableLockOnAndStartHack() const;
    void offGroupClipping();
    void startHackLocal(al::HitSensor*, al::HitSensor*);
    bool isInvalidHackEscape() const;
    void prepareEndHack();
    void endHackByCancel();
    void forceEndHack();
    void updatePosOnWave();
    void updateVelocityIfValidCollision();
    bool isOnDamageFire() const;
    void tryStartHitReactionUp();
    void updateShadowMaskLength();
    void tryStartHitReactionDown();
    void shiftSink();
    void headUp();
    void shiftReadyOrMove();
    void trySendMsgStartInSaucePan();
    void tryHitReactionThroughFence();
    void tryShiftLand();
    bool isHoldHackAction() const;
    void tryBoundMoveWall();
    void updateHackOnGround();
    void constrainLavaDomain();
    void tryShiftFall();
    bool isTriggerHackSwing() const;
    void faceToMoveVec();
    bool isTriggerHackJump() const;
    void revertTargetQuatInHackJump(sead::Quatf*, sead::Quatf*);
    void calcHackerMoveVec(sead::Vector3f*, const sead::Vector3f&) const;
    void makeDisplayQuatInHackJump(const sead::Quatf&, const sead::Quatf&, const sead::Quatf&,
                                   bool);
    bool isDropAttackCollision() const;
    bool isRiseAttackCollision() const;
    bool isHoldHackJump() const;
    void tryShiftContinuousJump();
    bool calcHackerMoveDir(sead::Vector3f*, const sead::Vector3f&) const;
    bool isWaitingLaunch() const;
    void launch(const sead::Vector3f&, float, al::CameraTicket*);
    void launchCancel(const sead::Vector3f&);
    void showHackCap();
    void hideHackCap();
    bool isTriggerJump() const;
    bool isTriggerAction() const;
    bool isTriggerHackAction() const;
    bool isTriggerCancel() const;
    void startJumpAnim(float);
    void resetAndAppear(const sead::Vector3f&, const sead::Quatf&, float);
    void endHackCommon();
    void calcLaunchPos(sead::Vector3f*, const sead::Vector3f&, float, float) const;
    void onGroupClipping();
    bool isOnGroundNoVelocity() const;
    void updateCollisionPartsMove();
    void accelStick();
    bool addHackActorAccelStick(sead::Vector3f*, float, const sead::Vector3f&);
    bool isGroundOverTheWave(bool, const sead::Vector3f&) const;
    bool isEnableSnapWaveSurface() const;

    void exeStandBy();
    void exeDelay();
    void exeRailMove();
    void exeReady();
    void exeUp();
    void exeTurn();
    void exeDown();
    void exeSink();
    void exeWait();
    void exeDie();
    void exeWaitHack();
    void exeWaitHackStart();
    void endWaitHackStart();
    void exeWaitHackFall();
    void exeHackFall();
    void exeHackMove();
    void endHackMove();
    void exeHackJump();
    void endHackJump();
    void exeHackLand();
    void exeHackInLauncher();
    void endHackInLauncher();
    void exeHackResetPos();
    void exeHackDemo();
    void exeRevive();

private:
    sead::Vector3f vecA;
    f32 bloatA;
    s32 valIndex;
    s32 valD;
    f32 bloatRail;
    s32 mRailMoveFrame;
    s32 mWaitFrameNum;
    s32 mDelayFrameNum;
    sead::Vector3f mClippingPos;
    s32 valCount;
    s32 valLeCount;
    sead::Vector3f vecPos;
    IUsePlayerHack* mPlayerHack;
    CapTargetInfo* mCapTargetInfo;
    s32 valFall;
    sead::Quatf quatA;
    sead::Vector3f vecIni;
    sead::Vector3f vIniPos;
    bool isB;
    sead::Vector3f vecCC;
    BubbleStateInLauncher* mBubbleStateInLauncher;
    al::CameraTicket* mCameraTicket;
    al::CameraTargetBase* mCameraTargetBase;
    sead::Matrix34f matrixA;
    sead::Vector3f vecStick;
    s32 valJump;
    sead::Quatf quatD;
    sead::Vector3f vecLand;
    f32 bloatQuatZ;
    s32 counterA;
    s32 counterB;
    sead::Vector3f vecC;
    al::LiveActor* mSubActor;
    bool mIsClipped;
    s32 valC;
    sead::Vector3f vvectrans;
    sead::Matrix34f maxD;
    bool mIsWaveCheckOn;
    sead::Quatf quat1;
    sead::Vector3f mShadowMaskOffset;
    f32 mShadowMaskDropLength;
    s32 valSatus;
    sead::Vector3f vecD;
    s32 valCountA;
    CollisionPartsFilterSpecialPurpose* mCollisionPartsFilter;
    bool isL;
    sead::Vector3f vecB;
    sead::Vector3f vecArr;
    bool isD;
    sead::Matrix34f mtxB;
    const char* mMaterialCode;
    sead::Vector3f vecResPos;
    bool isF;
    bool isMsg;
    DisregardReceiver* mDisregardReceiver;
    PlayerHackStartShaderCtrl* mPlayerHackStartShaderCtrl;
    al::JointSpringControllerHolder* mJointSpringControllerHolder;
    const char* mActionName;
    AnimScaleController* mAnimScaleController;
    bool isU;
    bool mIsShowTutorial;
    bool mIsGroupClipping;
};

static_assert(sizeof(Bubble) == 0x358);

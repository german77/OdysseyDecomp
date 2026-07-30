#pragma once

#include <math/seadVector.h>

#include "Library/Nerve/NerveStateBase.h"

namespace al {
struct ActorInitInfo;
class HitSensor;
class LiveActor;
class SensorMsg;
class WaterSurfaceFinder;
}  // namespace al

class CapTargetInfo;
class HackerDepthShadowMapCtrl;
class IUsePlayerCollision;
class IUsePlayerHack;
class PlayerAnimator;
class PlayerConst;
class PlayerEffect;
class PlayerHackStartShaderCtrl;
struct PlayerJumpMessageRequest;
class PlayerModelChangerYoshi;
class PlayerTrigger;
class YoshiStateHackPlay;

class YoshiStateHack : public al::ActorStateBase {
public:
    YoshiStateHack(const al::ActorInitInfo&, al::LiveActor*, al::LiveActor*, const CapTargetInfo*,
                   const PlayerConst*, const al::WaterSurfaceFinder*, PlayerTrigger*,
                   PlayerModelChangerYoshi*, PlayerAnimator*, IUsePlayerCollision*, PlayerEffect*,
                   PlayerJumpMessageRequest*);

    void appear();
    void updatePrevMovement();
    void updateAfterMovement();
    bool isEnableUpdateCollider() const;
    bool isActiveHeadCorrection() const;
    bool isCollisionShapeTongueJump() const;
    void tryGetLookAtTonguePos(sead::Vector3f*) const;
    bool tryCalcTonguePullForce(f32*, sead::Vector3f*) const;
    void calcGroundPoseRate(f32*, f32*) const;
    void startFruitShineGetDemo();
    void exeLockOn();
    void exeDemo();
    void exeDemoHackStart();
    void exeHack();
    bool attackSensor(al::HitSensor* self, al::HitSensor* other);
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other, al::HitSensor* self);
    bool receiveMsgSetNerveState(const al::SensorMsg* message, al::HitSensor* other,
                                 al::HitSensor* self);
    bool receiveMsgEndState(bool*, const al::SensorMsg* message, al::HitSensor* other,
                            al::HitSensor* self);
    void prepareEndHack();
    bool reactionCollidedCollisionCode();

private:
    al::LiveActor* mActor;
    PlayerConst* mPlayerConst;
    al::WaterSurfaceFinder* mWaterSurfaceFinder;
    IUsePlayerCollision* mPlayerCollision;
    CapTargetInfo* mCapTargetInfo;
    PlayerModelChangerYoshi* mPlayerModelChangerYoshi;
    PlayerAnimator* mPlayerAnimator;
    IUsePlayerHack* mPlayerHack;
    YoshiStateHackPlay* mYoshiStateHackPlay;
    PlayerHackStartShaderCtrl* mPlayerHackStartShaderCtrl;
    HackerDepthShadowMapCtrl* mHackerDepthShadowMapCtrl;
};

static_assert(sizeof(YoshiStateHack) == 0x78);

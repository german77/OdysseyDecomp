#pragma once

#include <math/seadVector.h>

#include "Library/Nerve/NerveStateBase.h"

namespace al {
struct ActorInitInfo;
class EventFlowExecutor;
class HitSensor;
class SensorMsg;
class WaterSurfaceFinder;
}  // namespace al

class IUsePlayerCollision;
class PlayerAnimator;
class PlayerConst;
class PlayerTrigger;

class YoshiStateNpc : public al::ActorStateBase {
public:
    YoshiStateNpc(al::LiveActor*, const PlayerConst*, const IUsePlayerCollision*,
                  const PlayerTrigger*, const al::WaterSurfaceFinder*, const al::ActorInitInfo&,
                  PlayerAnimator*, al::EventFlowExecutor*);

    void appear();
    void control();
    void exeAppear();
    void exeWait();
    void exeTurn();
    void exeReaction();
    void exeHackEnd();
    bool tryGetLookAtPlayerPos(sead::Vector3f*) const;
    bool reactionCollidedCollisionCode();
    bool receiveMsg(const al::SensorMsg*, al::HitSensor*, al::HitSensor*);
    bool receiveMsgReturnEggAndInitPosition(const al::SensorMsg*, al::HitSensor*, al::HitSensor*);

private:
    PlayerConst* mPlayerConst;
    IUsePlayerCollision* mIUsePlayerCollision;
    PlayerTrigger* mPlayerTrigger;
    al::WaterSurfaceFinder* mWaterSurfaceFinder;
    al::EventFlowExecutor* mEventFlowExecutor;
    PlayerAnimator* mPlayerAnimator;
    sead::Vector3f _50;
    sead::Vector3f _5c;
    sead::Vector3f _68;
    sead::Vector3f _74;
    bool _80;
    bool mIsUseReturnTimer;
    s32 _84;
};

static_assert(sizeof(YoshiStateNpc) == 0x88);

#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al {
struct ActorInitInfo;
class WaterSurfaceFinder;
}  // namespace al

class IUsePlayerCollision;
class PlayerConst;
class PlayerModelChangerYoshi;
class YoshiEgg;

class YoshiStateEgg : public al::ActorStateBase {
public:
    YoshiStateEgg(const al::ActorInitInfo&, al::LiveActor*, const IUsePlayerCollision*,
                  const PlayerConst*, const al::WaterSurfaceFinder*, PlayerModelChangerYoshi*);
    void appear();
    void kill();
    void exeAppear();

    void exeWait();
    bool reactionCollidedCollisionCode();

private:
    IUsePlayerCollision* mPlayerCollision;
    PlayerConst* mPlayerConst;
    al::WaterSurfaceFinder* mWaterSurfaceFinder;
    YoshiEgg* mYoshiEgg;
    PlayerModelChangerYoshi* mPlayerModelChangerYoshi;
    bool _48;
};

static_assert(sizeof(YoshiStateEgg) == 0x50);

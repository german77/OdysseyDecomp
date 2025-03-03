#pragma once

#include <basis/seadTypes.h>
#include <math/seadVector.h>

#include "Library/LiveActor/LiveActor.h"

namespace al {
class ActorInitInfo;
class HitSensor;
class SensorMsg;
}  // namespace al

class Coin;

class CoinRail : public al::LiveActor {
public:
    CoinRail(const char* name);

    void init(const al::ActorInitInfo& initInfo) override;
    void appear() override;
    void kill() override;
    void makeActorDead() override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;

    bool isGot() const;
    void exeMove();
    void exeCloseMove();

private:
    Coin** mCoinArray = nullptr;
    f32* mCoinPosition = nullptr;
    s32 mCoinNum = 0;
    f32 mMoveVelocity = 0.0f;
    s32 someSortOfIndex = 0;
    s32 anotherKindOfIndex = 0;
    sead::Vector3f mRailClippingInfo;
    sead::Vector3f mDisplayOffset = sead::Vector3f::zero;
};

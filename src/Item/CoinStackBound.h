#pragma once

#include <basis/seadTypes.h>
#include <math/seadQuat.h>
#include <math/seadVector.h>

#include "Library/LiveActor/LiveActor.h"

namespace al {
class ActorInitInfo;
class FlashingTimer;
class HitSensor;
class SensorMsg;
class WaterSurfaceFinder;
}  // namespace al

class CoinStackBound : public al::LiveActor {
public:
    CoinStackBound(char const* name);

    void init(const al::ActorInitInfo& initInfo) override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;
    void appear() override;
    void processTimer();

    void exeAlive();
    void exeAppear();
    void exeFall();
    void exeBounce();
    void exeVanish();
    void exeCollected();

private:
    void* _108;
    s32 _110;
    sead::Vector3f vector1 = sead::Vector3f::zero;
    sead::Vector3f vector2 = sead::Vector3f::zero;
    sead::Vector3f vector3 = sead::Vector3f::zero;
    sead::Quatf quat = sead::Quatf::unit;
    s32 counter = 0;
    float lefloat = 17.5f;
    bool anotherbool = false;
    float rotateAngle = 30.0f;
    al::FlashingTimer* mFlashingTimer = nullptr;
    bool timerStuf = false;
    sead::Matrix34f matrix;
    bool isOnGound = false;
    al::WaterSurfaceFinder* mWaterSurfaceFinder = nullptr;
    bool nibool = true;
};

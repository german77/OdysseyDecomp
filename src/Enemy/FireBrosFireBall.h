#pragma once

#include <basis/seadTypes.h>
#include <math/seadVector.h>

#include "Enemy/Bros.h"

namespace al {
struct ActorInitInfo;
class HitSensor;
class SensorMsg;
}  // namespace al

class FireBrosFireBall : public BrosWeaponBase {
public:
    FireBrosFireBall(const char* name, const al::LiveActor* bros);

    void init(const al::ActorInitInfo& init) override;
    void shoot(const sead::Vector3f&, const sead::Quatf&, const sead::Vector3f&, bool, int,
               bool) override;
    void killEnemy() override;
    void move();
    void attach(const sead::Matrix34f*, const sead::Vector3f&, const sead::Vector3f&,
                const char*) override;
    void attackSensor(al::HitSensor* self, al::HitSensor* other) override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;
    void control() override;

    void killCollide(al::HitSensor*, const sead::Vector3f&, bool) override {}

    void exeAppear();
    void exeMove();
    void exeAttach();

private:
    bool mIsHack = false;
    bool mIsFast = false;
    bool mIsBouncing = false;
    s32 mAttackDelay = 0;
    bool isKeek = false;
};

static_assert(sizeof(FireBrosFireBall) == 0x140);

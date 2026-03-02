#pragma once

#include <basis/seadTypes.h>
#include <math/seadVector.h>
#include <math/seadQuat.h>
#include <math/seadMatrix.h>

#include "Library/LiveActor/LiveActor.h"

#include "Player/IUsePlayerCollision.h"

namespace al {
class CollisionPartsConnector;

}  // namespace al
class IUsePlayerPuppet;
class PlayerCollider;
class MotorcyclePlayerAnimator;
class BindKeepDemoInfo;
class CameraTargetBase;
class CameraSubTargetBase;
class CameraSubTargetTurnParam;

class Motorcycle : public al::LiveActor, public IUsePlayerCollision {
public:
    Motorcycle(const char* name);

    void init(const al::ActorInitInfo& info) override;
    void initAfterPlacement() override;
    void attackSensor(al::HitSensor* self, al::HitSensor* other) override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;
    void movement() override;
    void kill() override;
    void updateCollider() override;
    void calcAnim() override;
    PlayerCollider* getPlayerCollider() const override;

    void exeWait();
    void exeCreep();
    void exeFall();
    void exeJump();
    void endJump();
    void exeReaction();
    void exeReset();
    void exeRideStart();
    void exeRideStartOn();
    void exeRideWait();
    void exeRideWaitJump();
    void endRideWaitJump();
    void exeRideWaitLand();
    void exeRideRunStart();
    void exeRideRun();
    void exeRideRunCollide();
    void exeRideRunFall();
    void exeRideRunWheelie();
    void endRideRunWheelie();
    void exeRideRunLand();
    void exeRideRunJump();
    void endRideRunJump();
    void exeRideRunBoundStart();
    void exeRideRunBound();
    void exeRideRunClash();
    void exeRideParkingSnap();
    void exeRideParkingStart();
    void exeRideParking();
    void exeRideParkingAfter();

private:
    IUsePlayerPuppet* mPlayerPuppet;
    PlayerCollider* mPlayerCollider;
    void* kk;
    MotorcyclePlayerAnimator* mPlayerAnimator;
    void* kk2;
    f32 floatA;
    f32 floatB;
    f32 floatC;
    f32 floatJump;
    void* kk3;
    void* _150;
    BindKeepDemoInfo* mBindKeepDemoInfo;
    al::CollisionPartsConnector* mCollisionPartsConnector;
    CameraTargetBase* mCameraTargetBase;
    CameraSubTargetBase* mCameraSubTargetBase;
    CameraSubTargetTurnParam* mCameraSubTargetTurnParam;
    sead::Vector3f vectorA;
    f32 floatD;

    void* _190;

    void* _198;
    sead::Quatf quat;
    sead::Vector3f vector;

    char filler_[0x10];
    sead::Vector3f vector3;
    sead::Vector3f vector2;
    sead::Vector3f vector5;
    sead::Vector3f vector4;

    void* _1fc;
    sead::Matrix34f mtx;
    int _filler;
    s32 valA;
    char filler[0xc];
    bool _248;
    bool _249;
};

static_assert(sizeof(Motorcycle) == 0x250);

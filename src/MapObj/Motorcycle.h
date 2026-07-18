#pragma once

#include <basis/seadTypes.h>
#include <container/seadPtrArray.h>
#include <math/seadMatrix.h>
#include <math/seadQuat.h>
#include <math/seadVector.h>

#include "Library/LiveActor/LiveActor.h"

#include "Player/IUsePlayerCollision.h"

namespace al {
class CollisionPartsConnector;
class CameraTargetBase;
class CameraSubTargetBase;

}  // namespace al
class IUsePlayerPuppet;
class PlayerCollider;
class MotorcyclePlayerAnimator;
class BindKeepDemoInfo;
class CameraSubTargetTurnParam;

struct MotorcycleParams {
    bool bool_0;
    bool bool_1;
    bool bool_2;
    bool bool_3;
    bool bool_4;
    bool bool_5;
    sead::Vector3f vector_8;
    sead::FixedPtrArray<sead::Vector3f, 194> array2;
    sead::FixedPtrArray<sead::Vector3f, 194> array;
    s32 val_c58;
    sead::Vector3f vector_5c;
    sead::Vector3f normal;
};

static_assert(sizeof(MotorcycleParams) == 0xc78);

struct ParkingParams {
    al::LiveActor* actor;
    f32 floatA;
    f32 floatB;
    f32 floatC;
    f32 floatJump;
    sead::Quatf quatA;
    sead::Quatf quatB;
    sead::Vector3f vectorA;
    sead::Vector3f vectorB;
};

static_assert(sizeof(ParkingParams) == 0x50);

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

    PlayerCollider* getPlayerCollider() const override { return mPlayerCollider; }

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

    MotorcycleParams* getParams() const { return mParams; }

    bool isStickWorldPose()const{return mIsStickWorldPose;}
private:
    bool isRideRun_();

    IUsePlayerPuppet* mPlayerPuppet;
    PlayerCollider* mPlayerCollider;
    MotorcycleParams* mParams;
    MotorcyclePlayerAnimator* mPlayerAnimator;
    void* kk2;
    f32 floatA;
    f32 floatB;
    f32 floatC;
    f32 floatJump;
    void* kk3;
    ParkingParams* mParkingParams;
    BindKeepDemoInfo* mBindKeepDemoInfo;
    al::CollisionPartsConnector* mCollisionPartsConnector;
    al::CameraTargetBase* mCameraTargetBase;
    al::CameraSubTargetBase* mCameraSubTargetBase;
    CameraSubTargetTurnParam* mCameraSubTargetTurnParam;
    sead::Vector3f vectorA;
    f32 floatD;

    void* _190;

    void* _198;
    sead::Quatf quat;
    sead::Vector3f vector;
    sead::Quatf mStartQuat;
    sead::Vector3f mStartTrans;
    sead::Vector3f vector2;
    sead::Vector3f vector5;
    sead::Vector3f vector4;

    s32 _23css;
    s32 _23cxs;
    sead::Matrix34f mtx;
    int _filler;
    s32 valA;
    s32 _23c;
    s32 _240;
    bool mIsOnLight;
    bool _245;
    bool mIsStickWorldPose;
    bool _247;
    bool _248;
    bool _249;
};

static_assert(sizeof(Motorcycle) == 0x250);

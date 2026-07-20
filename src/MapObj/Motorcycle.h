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
    bool isOnGround;
    bool isOnJump;
    bool bool_3;
    bool bool_4;
    bool bool_5;
    bool bool_6;
    sead::Vector3f floorNormalAvg;                                // check
    sead::FixedPtrArray<sead::Vector3f, 194> frontContactPoints;  // check
    sead::FixedPtrArray<sead::Vector3f, 194> backContactPoints;   // check
    s32 framesInAir;
    sead::Vector3f lastGroundPos;
    sead::Vector3f groundNormal;
};

static_assert(sizeof(MotorcycleParams) == 0xc78);

struct ParkingParams {
    al::LiveActor* actor;
    f32 steerAngle;
    f32 handleAngle;
    f32 mLeanAngle;
    f32 mJumpAngle;
    sead::Quatf quatA;
    sead::Quatf quatB;
    sead::Vector3f vectorA;
    sead::Vector3f vectorB;
};

static_assert(sizeof(ParkingParams) == 0x50);

struct AccelerationState {
    bool isAccelerating;
    f32 accelRate;
};

static_assert(sizeof(AccelerationState) == 0x8);

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

    bool isStickWorldPose() const { return mIsStickWorldPose; }

    BindKeepDemoInfo* getBindKeepDemoInfo() const { return mBindKeepDemoInfo; }

    s32* get_23c() { return &_23c; }

    IUsePlayerPuppet** getPuppy() { return &mPlayerPuppet; }


private:
    bool isRideRun_();

    IUsePlayerPuppet* mPlayerPuppet = nullptr;
    PlayerCollider* mPlayerCollider = nullptr;
    MotorcycleParams* mParams = nullptr;
    MotorcyclePlayerAnimator* mPlayerAnimator = nullptr;
    AccelerationState* mAccelerationState = nullptr;  // 130
    f32 steerAngle = 0.0f;
    f32 handleAngle = 0.0f;
    f32 mLeanAngle = 0.0f;
    f32 mJumpAngle = 0.0f;
    f32* kk3 = nullptr;  // 148
    ParkingParams* mParkingParams = nullptr;
    BindKeepDemoInfo* mBindKeepDemoInfo = nullptr;
    al::CollisionPartsConnector* mCollisionPartsConnector = nullptr;
    al::CameraTargetBase* mCameraTargetBase = nullptr;
    al::CameraSubTargetBase* mCameraSubTargetBase = nullptr;
    CameraSubTargetTurnParam* mCameraSubTargetTurnParam = nullptr;
    sead::Vector3f vectorA = {0.0f, 0.0f, 0.0f};
    f32 floatD = 0.0f;

    void* _190 = nullptr;

    s32 _198 = 0;
    s32 _19c = 0;
    sead::Quatf mQuat = sead::Quatf::unit;
    sead::Vector3f mTrans= {0.0f, 0.0f, 0.0f};
    sead::Quatf mStartQuat = sead::Quatf::unit;
    sead::Vector3f mStartTrans = {0.0f, 0.0f, 0.0f};
    sead::Vector3f vector2 = {0.0f, 0.0f, 0.0f};
    sead::Vector3f vector5 = {0.0f, 0.0f, 0.0f};
    sead::Vector3f vector4 = {0.0f, 0.0f, 0.0f};

    f32 _23css = 0;
    f32 _23cxs = 0;
    sead::Matrix34f mWaterSurfaceMtx = sead::Matrix34f::ident;
    int _234 = 0;
    s32 valA = -1;
    s32 _23c = -1;
    s32 _240 = 3;
    bool mIsOnLight = false;
    bool _245 = false;
    bool mIsStickWorldPose = false;
    bool _247 = false;
    bool _248 = false;
    bool _249 = false;
};

static_assert(sizeof(Motorcycle) == 0x250);

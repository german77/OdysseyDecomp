#pragma once

#include <basis/seadTypes.h>
#include <container/seadObjArray.h>
#include <container/seadPtrArray.h>
#include <math/seadMatrix.h>
#include <math/seadQuat.h>
#include <math/seadVector.h>

#include "Library/LiveActor/LiveActor.h"

#include "Camera/PlayerColliderCameraTarget.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Player/IUsePlayerCollision.h"

namespace al {
class CollisionPartsConnector;
class CameraTargetBase;
class TransCameraSubTarget;
class CameraSubTargetTurnParam;

}  // namespace al
class IUsePlayerPuppet;
class PlayerCollider;
class MotorcyclePlayerAnimator;
class BindKeepDemoInfo;
class MotorcycleColliderCameraTarget;

struct MotorcycleParams {
    bool isInBack(const sead::Vector3f pos) {
        for (s32 i = 0; i < backContactPoints.size(); i++)
            if (pos.dot(-*backContactPoints[i]) < 0.0f)
                return true;
        return false;
    }

    bool isInFront(const sead::Vector3f pos) {
        for (s32 i = 0; i < frontContactPoints.size(); i++)
            if (pos.dot(*frontContactPoints[i]) < 0.0f)
                return true;
        return false;
    }

    bool isOnGround = false;
    bool isOnJump = false;
    bool bool_3 = false;
    bool bool_4 = false;
    bool bool_5 = false;
    bool bool_6 = false;
    sead::Vector3f groundNormalAvg = {0.0f, 0.0f, 0.0f};          // check
    sead::FixedObjArray<sead::Vector3f, 64> frontContactPoints;  // check
    sead::FixedObjArray<sead::Vector3f, 64> backContactPoints;   // check
    s32 framesInAir = 0;
    sead::Vector3f lastGroundPos = {0.0f, 0.0f, 0.0f};
    sead::Vector3f groundNormal = {0.0f, 1.0f, 0.0f};
};

static_assert(sizeof(MotorcycleParams) == 0xc78);

struct ParkingParams {
    al::LiveActor* actor = nullptr;
    f32 steerAngle = 0.0f;
    f32 handleAngle = 0.0f;
    f32 leanAngle = 0.0f;
    f32 jumpAngle = 0.0f;
    sead::Quatf quatA = sead::Quatf::unit;
    sead::Quatf quatB = sead::Quatf::unit;
    sead::Vector3f vectorA = {0.0f, 0.0f, 0.0f};
    sead::Vector3f vectorB = {0.0f, 0.0f, 0.0f};
};

static_assert(sizeof(ParkingParams) == 0x50);

struct AccelerationState {
    bool isAccelerating = false;
    f32 accelRate = 0.0f;
};

static_assert(sizeof(AccelerationState) == 0x8);

struct SeRumbleState {
    void reset() {
        volume = 0.0f;
        rumble = 0;
    }

    f32 volume = 0.0f;
    s32 rumble = 0;
};

static_assert(sizeof(SeRumbleState) == 0x8);

// TODO: Find what kind of object has this structure
struct UnknownStruct {
    char filler[0x10];
    al::LiveActor* actor;
};

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
    f32 mSteerAngle = 0.0f;
    f32 mHandleAngle = 0.0f;
    f32 mLeanAngle = 0.0f;
    f32 mJumpAngle = 0.0f;
    SeRumbleState* mSeRumbleState = nullptr;  // 148
    ParkingParams* mParkingParams = nullptr;
    BindKeepDemoInfo* mBindKeepDemoInfo = nullptr;
    al::CollisionPartsConnector* mCollisionPartsConnector = nullptr;
    MotorcycleColliderCameraTarget* mColliderCameraTarget = nullptr;
    al::TransCameraSubTarget* mTransCameraSubTarget = nullptr;
    al::CameraSubTargetTurnParam* mCameraSubTargetTurnParam = nullptr;
    sead::Vector3f vectorA = {0.0f, 0.0f, 0.0f};
    f32 floatD = 0.0f;

    sead::Vector3f* mCoursePoints = nullptr;

    s32 mCoursePointSize = 0;
    s32 _19c = 0;
    sead::Quatf mQuat = sead::Quatf::unit;
    sead::Vector3f mTrans = {0.0f, 0.0f, 0.0f};
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
    bool mIsAccelerating = false;
    bool mIsStickWorldPose = false;
    bool _247 = false;
    bool _248 = false;
    bool _249 = false;
};

static_assert(sizeof(Motorcycle) == 0x250);

const sead::Vector3f colliderTrans = {0.0f, 0.0f, -85.0f};

class MotorcycleColliderCameraTarget : public PlayerColliderCameraTarget {
public:
    MotorcycleColliderCameraTarget(const Motorcycle* actor)
        : PlayerColliderCameraTarget(actor, actor) {}

    void calcTrans(sead::Vector3f* offset) const override {
        al::calcTransLocalOffset(offset, getActor(), colliderTrans);
    }
};

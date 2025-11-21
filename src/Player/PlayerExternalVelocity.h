#pragma once

#include <basis/seadTypes.h>
#include <math/seadVector.h>

namespace al {
class HitSensor;
class LiveActor;
class SensorMsg;
}  // namespace al

class ExternalForceKeeper;
class IUsePlayerCollision;

class PlayerExternalVelocity {
public:
    PlayerExternalVelocity(const al::LiveActor*, const IUsePlayerCollision*, const sead::Vector3f*);

    bool receiveMsgPlayer(const al::SensorMsg*, al::HitSensor*, al::HitSensor*);
    bool receiveMsgCap(const al::SensorMsg*, al::HitSensor*, al::HitSensor*);
    void requestApplyLastGroundInertia();
    void cancelAndFeedbackLastGroundInertia(al::LiveActor*, f32, bool);
    void update();
    void updatePadRumbleExternalForce();
    void updateLastGroundForce(bool isOnGround);
    void reset();
    bool isExistForce() const;
    bool isExistSnapForce() const;

private:
    ExternalForceKeeper* mExternalForceKeeper=nullptr;
    sead::Vector3f v08= {0.0f, 0.0f, 0.0f};
    sead::Vector3f v14= {0.0f, 0.0f, 0.0f};
    sead::Vector3f v20= {0.0f, 0.0f, 0.0f};
    const al::LiveActor* mActor=nullptr;
    const IUsePlayerCollision* mCollision=nullptr;
    const sead::Vector3f* mTrans=nullptr;
    bool isRumbleRunning=false;
    sead::Vector3f mExternalForce= {0.0f, 0.0f, 0.0f};
    sead::Vector3f v58= {0.95f, 0.955f, 0.955f};
    sead::Vector3f v64= {0.95f, 0.275f, 0.275f};
    sead::Vector3f mSnapForce= {0.0f, 0.0f, 0.0f};
    s32 i7c=0;
    sead::Vector3f mMovePowerForce= {0.0f, 0.0f, 0.0f};
    sead::Vector3f v8= {0.0f, 0.0f, 0.0f};
};

static_assert(sizeof(PlayerExternalVelocity) == 0x98);

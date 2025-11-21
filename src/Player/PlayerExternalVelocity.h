#pragma once

class PlayerExternalVelocity {
public:
    void PlayerExternalVelocity(const al::LiveActor*, const IUsePlayerCollision*,
                                const sead::Vector3<float>*);
    void receiveMsgPlayer(const al::SensorMsg*, al::HitSensor*, al::HitSensor*);
    void receiveMsgCap(const al::SensorMsg*, al::HitSensor*, al::HitSensor*);
    void requestApplyLastGroundInertia();
    void cancelAndFeedbackLastGroundInertia(al::LiveActor*, float, bool);
    void update();
    void updatePadRumbleExternalForce();
    void updateLastGroundForce(bool);
    void reset();
    bool isExistForce() const;
    bool isExistSnapForce() const;

private:
    al::ExternalForceKeeper mExternalForceKeeper;

};

static_assert(sizeof(Bird) == 0x98);

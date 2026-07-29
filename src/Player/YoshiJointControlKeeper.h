#pragma once

#include <math/seadVector.h>

namespace al {
class LiveActor;
}  // namespace al

class ActorJointControlGroundPose;
class IUsePlayerCollision;
class JointLookAtController;
class YoshiJointControlHeadCorrection;

class YoshiJointControlKeeper {
public:
    YoshiJointControlKeeper(const al::LiveActor*, const IUsePlayerCollision*);

    void update();
    void updateHeadCorrect();
    void updateLookAt();
    void updateGroundPose();

    void set_28(bool value) { _28 = value; }

    void set_30(bool value) { _30 = value; }

    void set_34(const sead::Vector3f& value) { _34 = value; }

    void set_40(f32 a, f32 b) {
        _40 = a;
        _44 = b;
        update();
    }

private:
    al::LiveActor* mActor;
    IUsePlayerCollision* mPlayerCollision;
    YoshiJointControlHeadCorrection* mControlHeadCorrection;
    JointLookAtController* mLookAtController;
    ActorJointControlGroundPose* mControlGroundPose;
    bool _28;
    f32 _2c;
    bool _30;
    sead::Vector3f _34;
    f32 _40;
    f32 _44;
    f32 _48;
    f32 _4c;
};

static_assert(sizeof(YoshiJointControlKeeper) == 0x50);

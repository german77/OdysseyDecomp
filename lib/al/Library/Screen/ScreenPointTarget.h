#pragma once

#include <basis/seadTypes.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>

namespace al {
class LiveActor;
class ParameterObj;
class ScreenPointCheckGroup;
class ParameterF32;
class ParameterStringRef;
class ParameterV3f;

class ScreenPointTarget {
public:
    ScreenPointTarget(LiveActor*, const char*, f32, const sead::Vector3f*, const char*,
                      const sead::Vector3f&);

    void setFollowMtxPtrByJointName(const LiveActor*);
    const char* getJointName() const;
    void update();
    void validate();
    void invalidate();
    void validateBySystem();
    void invalidateBySystem();
    f32 getTargetRadius() const;
    const char* getTargetName() const;
    void setTargetName(const char*);
    void setTargetRadius(f32);
    void setTargetFollowPosOffset(const sead::Vector3f&);
    void setJointName(const char*);

    ParameterObj* getParameterObj() const { return mParameterObj; }

    sead::Matrix34f* getJointMtx() const { return mJointMtx; }

    LiveActor* getActor() const { return mActor; }

    const sead::Vector3f& getTargetPos() const { return mTargetPos; }

private:
    bool mBa = false;
    bool mBb = true;
    ParameterObj* mParameterObj;
    ParameterStringRef* mParameterBase;
    ParameterF32* mParameterBase2;
    ParameterV3f* mParameterBase3;
    ParameterStringRef* mJointName;
    f32 bloat=1.0f;
    char filler2[0x4];
    const sead::Vector3f* _38;
    sead::Matrix34f* mJointMtx;
    sead::Vector3f _48;
    sead::Vector3f mTargetPos=sead::Vector3f::zero;
    LiveActor* mActor;
    ScreenPointCheckGroup* mCheckGroup=nullptr;
};

static_assert(sizeof(ScreenPointTarget) == 0x70);

}  // namespace al

#pragma once

#include <math/seadMatrix.h>
#include <math/seadVector.h>

namespace al {
class LiveActor;
}  // namespace al

class IUsePlayerCollision;

class CapTargetInfo {
public:
    CapTargetInfo();

    void init(const al::LiveActor* actor, const char* name);
    void setFollowLockOnMtx(const char* jointName, const sead::Vector3f& localTrans,
                            const sead::Vector3f& localRotate);
    void setLockOnStartAnimName(const char* animName);
    void setLockOnAnimName(const char* animName);
    void setHackName(const char* hackName);
    void makeLockOnMtx(sead::Matrix34f* outMtx) const;
    void calcLockOnFollowTargetScale(sead::Vector3f* targetScale) const;

    void setPoseMatrix(sead::Matrix34f* mtx) { mPoseMatrix = mtx; }

    void setEscapeLocalOffset(const sead::Vector3f& offset) {
        mIsEscapeLocalOffset = true;
        mEscapeLocalOffset.x = offset.x;
        mEscapeLocalOffset.y = offset.y;
        mEscapeLocalOffset.z = offset.z;
    }

    void setPlayerCollision(IUsePlayerCollision* playerCollision) {
        mPlayerCollision = playerCollision;
    }

    void setLockOnScale(f32 lockOnScale) { mLockOnScale = lockOnScale; }

    void useLockOnFollowMtxScale(bool isUseLockOnFollowMtxScale) {
        mIsUseLockOnFollowMtxScale = isUseLockOnFollowMtxScale;
    }

    void useFollowScaleLocalOffset(bool isUseFollowScaleLocalOffset) {
        mIsUseFollowScaleLocalOffset = isUseFollowScaleLocalOffset;
    }

    void setLockOnOnly(bool isLockOnOnly) { mIsLockOnOnly = isLockOnOnly; }

    void setHackNameToCamera(bool isSetHackNameToCamera) {
        mIsSetHackNameToCamera = isSetHackNameToCamera;
    }

    void setInvalidHackThrow(bool isInvalidHackThrow) { mIsInvalidHackThrow = isInvalidHackThrow; }

    void setInvalidCapEye(bool isInvalidCapEye) { mIsInvalidCapEye = isInvalidCapEye; }

    void useDepthShadow(bool isUseDepthShadow) { mIsUseDepthShadow = isUseDepthShadow; }

private:
    const al::LiveActor* mActor = nullptr;
    const char* mHackName = nullptr;
    IUsePlayerCollision* mPlayerCollision = nullptr;
    sead::Matrix34f* mPoseMatrix = nullptr;
    const sead::Matrix34f* mJointMtx = nullptr;
    sead::Vector3f mLocalTrans = sead::Vector3f::zero;
    sead::Vector3f mLocalRotate = sead::Vector3f::zero;
    f32 mLockOnScale = 1.0f;
    bool mIsUseLockOnFollowMtxScale = false;
    bool mIsUseFollowScaleLocalOffset = false;
    const char* mLockOnStartAnimName = "Capture";
    const char* mLockOnAnimName = "Capture";
    bool mIsEscapeLocalOffset = false;
    sead::Vector3f mEscapeLocalOffset = {0.0f, 0.0f, 0.0f};
    const char* mName = nullptr;
    bool mIsExistModel = false;
    bool mIsLockOnOnly = false;
    bool _72 = false;
    bool mIsUseDepthShadow = false;
    bool _74 = false;
    bool mIsLockOn = true;
    bool mIsLockOnStart = false;
    bool mIsSetHackNameToCamera = false;
    bool _78 = false;
    bool _79 = false;
    bool mIsInvalidHackThrow = false;
    bool mIsInvalidCapEye = false;
    bool _7c = false;
    bool _7d = false;
    bool _7e = false;
    bool _7f = false;
};

namespace CapTargetInfoFunction {
void initIterCapTargetInfo(CapTargetInfo* capTargetInfo, IUsePlayerCollision*,
                           al::LiveActor const* actor, char const* name);
}  // namespace CapTargetInfoFunction

#pragma once

#include <basis/seadTypes.h>
#include <math/seadBoundBox.h>
#include <math/seadVector.h>

namespace sead {
class Thread;
}  // namespace sead

namespace al {
class ActorInitInfo;
class ClippingJudge;
class ClippingActorInfo;
class ClippingGroupHolder;
class LiveActor;

class ClippingActorHolder {
public:
    ClippingActorHolder(s32);

    void updateAsync(sead::Thread*, s64);
    void update(const al::ClippingJudge*);
    ClippingActorInfo* registerActor(al::LiveActor*);
    ClippingActorInfo* initGroupClipping(al::LiveActor*, const al::ActorInitInfo&);
    void endInit(al::ClippingGroupHolder*);
    void validateClipping(al::LiveActor*);
    void invalidateClipping(al::LiveActor*);
    void addToClippingTarget(al::LiveActor*);
    void removeFromClippingTarget(al::LiveActor*);
    void onGroupClipping(al::LiveActor*);
    void offGroupClipping(al::LiveActor*);
    f32 getClippingRadius(const al::LiveActor*);
    ClippingActorInfo* find(const al::LiveActor*) const;
    const sead::Vector3f& getClippingCenterPos(const al::LiveActor*);
    void setTypeToSphere(al::LiveActor*, f32, const sead::Vector3f*);
    void setTypeToObb(al::LiveActor*, const sead::BoundBox3f&);
    sead::BoundBox3f& getClippingObb(const al::LiveActor*);
    void setNearClipDistance(al::LiveActor*, f32);
    void setFarClipLevel20M(al::LiveActor*);
    void setFarClipLevelMax(al::LiveActor*);
    void updateFarClipLevel();

private:
    char _0[0x40];
};

}  // namespace al

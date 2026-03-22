#pragma once

#include <math/seadMatrix.h>
#include <math/seadVector.h>

#include "Library/HostIO/HioNode.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace al {
class LiveActor;
class PlacementId;
struct ActorInitInfo;
}  // namespace al

class FukankunZoomObj;
class Fukankun;

class FukankunZoomObjHolder : public al::HioNode, public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_FukankunZoomObjHolder;

    FukankunZoomObjHolder();

    void initAfterPlacementSceneObj(const al::ActorInitInfo& info) override;
    virtual void execute();
    FukankunZoomObj* tryGetFukankunZoomObj(const al::PlacementId&);
    void declareUseFukankunZoomTargetActor();
    void registerFukankunZoomObj(const al::LiveActor*, const al::PlacementId*);
    void registerFukankunZoomTargetActor(const al::LiveActor*, s32, const sead::Vector3f&,
                                         const char*);
    u32 getFukankunZoomTargetActorNum() const;
    al::LiveActor* tryGetFukankunZoomTargetActor(s32) const;
    bool tryGetFukankunZoomTargetActorIsNoZoomOn(s32) const;
    u32 tryGetFukankunZoomTargetActorZoomType(s32) const;
    u32 tryGetFukankunZoomTargetActorOffset(s32) const;
    const char* tryGetFukankunZoomTargetActorTargetJointName(s32) const;
    void declareWatchFukankunZoomTargetActor(const Fukankun*, s32);
    s32 getWatchCount(const al::LiveActor*) const;
    bool tryGetActiveFukankunLinkedShineMtx(const sead::Matrix34f**) const;

private:
    s8 filler[0x658 - sizeof(al::HioNode) - sizeof(al::ISceneObj)];
};

static_assert(sizeof(FukankunZoomObjHolder) == 0x658);

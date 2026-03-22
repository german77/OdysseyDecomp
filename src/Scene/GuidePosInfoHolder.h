#pragma once

#include <math/seadVector.h>

#include "Library/HostIO/HioNode.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace al {
class LiveActor;
class IUseSceneObjHolder;
}  // namespace al

class GuidePosInfoHolder : public al::HioNode, public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_GuidePosInfoHolder; 

    GuidePosInfoHolder();

    void calcGuidePos(sead::Vector3f* pos, const al::LiveActor* actor) const;
    void setForceGuidePosPtr(const sead::Vector3f* guidePosPtr);
    void resetForceGuidePosPtr();

private:
    const sead::Vector3f* mGuidePosPtr = nullptr;
};

static_assert(sizeof(GuidePosInfoHolder) == 0x10);

namespace rs {
void calcGuidePos(sead::Vector3f* pos, const al::LiveActor* actor);
void setRouteHeadGuidePosPtr(const al::IUseSceneObjHolder* holder, const sead::Vector3f* guidePosPtr);
void resetRouteHeadGuidePosPtr(const al::IUseSceneObjHolder* holder);
}  // namespace rs

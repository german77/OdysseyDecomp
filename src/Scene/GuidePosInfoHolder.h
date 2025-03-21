#pragma once

#include <math/seadVector.h>

namespace rs {
class IUseSceneObjHolder;
class LiveActor;
}  // namespace rs

namespace rs {
void calcGuidePos(sead::Vector3f*, const al::LiveActor*);
void setRouteHeadGuidePosPtr(const al::IUseSceneObjHolder*, const sead::Vector3f*);
void resetRouteHeadGuidePosPtr(const al::IUseSceneObjHolder*);
}  // namespace rs

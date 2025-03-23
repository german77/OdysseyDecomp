#pragma once

#include <basis/seadTypes.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>

#include "Library/Camera/CameraTargetBase.h"

namespace al {
class CameraPoser;
struct CameraStartInfo;

class ActorMatrixCameraTarget : public CameraTargetBase {
public:
    ActorMatrixCameraTarget(LiveActor const*, sead::Matrix34f const*);

    void calcTrans(sead::Vector3f*) const;
    void calcSide(sead::Vector3f*) const;
    void calcUp(sead::Vector3f*) const;
    void calcFront(sead::Vector3f*) const;
    void calcVelocity(sead::Vector3f*) const;
};

// static_assert(sizeof(CameraVerticalAbsorber) == 0x1B0);

}  // namespace al

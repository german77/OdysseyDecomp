#pragma once

#include <math/seadVector.h>

#include "Library/Camera/ICameraInput.h"

namespace al {
class LiveActor;
}

class ProjectCameraInput : public al::ICameraInput {
public:
    ProjectCameraInput(const al::LiveActor* player);

    void calcInputStick(sead::Vector2f*) const override;
    bool isTriggerReset() const override;
    bool isHoldZoom() const override;
    bool tryCalcSnapShotMoveStick(sead::Vector2f*) const override;
    bool isHoldSnapShotZoomIn() const override;
    bool isHoldSnapShotZoomOut() const override;
    bool isHoldSnapShotRollLeft() const override;
    bool isHoldSnapShotRollRight() const override;
    void calcGyroPose(sead::Vector3f*, sead::Vector3f*, sead::Vector3f*) const override;

private:
    al::LiveActor* mPlayer = nullptr;
};

static_assert(sizeof(ProjectCameraInput) == 0x10);

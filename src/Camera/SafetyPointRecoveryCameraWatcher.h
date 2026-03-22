#pragma once

#include "Library/Camera/IUseCamera.h"
#include "Library/Execute/IUseExecutor.h"
#include "Library/Nerve/NerveExecutor.h"

namespace al {
class PlayerHolder;
class AreaObjGroup;
}  // namespace al

class SafetyPointRecoveryCameraWatcher : public al::NerveExecutor,
                                         public al::IUseCamera,
                                         public al::IUseExecutor {
public:
    SafetyPointRecoveryCameraWatcher(al::CameraDirector*, const al::PlayerHolder*,
                                     const al::AreaObjGroup*, const al::AreaObjGroup*);
    ~SafetyPointRecoveryCameraWatcher() override;

    void execute() override;
    void exeDeactive();
    void exeActive();
    void exeActiveGoalCamera();
    void exeActiveObjectCamera();

    al::CameraDirector* getCameraDirector() const override;

private:
    s8 filler[0x88 - sizeof(al::NerveExecutor) - sizeof(al::IUseCamera) - sizeof(al::IUseExecutor)];
};

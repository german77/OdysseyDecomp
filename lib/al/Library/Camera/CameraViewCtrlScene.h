#pragma once

#include <basis/seadTypes.h>

namespace al {

class CameraRequestParamHolder;
class CameraSwitchRequester;
class SpecialCameraHolder;

class SceneCameraViewCtrl {
public:
    SceneCameraViewCtrl();

    const CameraSwitchRequester* getSwitchRequester() const { return mSwitchRequester; }

    void resetViewName() { mViewName = nullptr; }

private:
    CameraSwitchRequester* mSwitchRequester = nullptr;
    const char* mViewName = "Start";
    void* _10 = nullptr;
};

static_assert(sizeof(SceneCameraViewCtrl) == 0x18);

class SceneCameraCtrl {
public:
    SceneCameraCtrl();
    void init(s32 viewNum);

    SceneCameraViewCtrl* getSceneViewAt(s32 idx) const { return &mViewArray[idx]; }

    CameraRequestParamHolder* getRequestParamHolder() const { return mRequestParamHolder; }

private:
    s32 mViewNum = 0;
    SceneCameraViewCtrl* mViewArray = nullptr;
    CameraRequestParamHolder* mRequestParamHolder = nullptr;
    SpecialCameraHolder* mSpecialCameraHolder = nullptr;
};

static_assert(sizeof(SceneCameraCtrl) == 0x20);

}  // namespace al

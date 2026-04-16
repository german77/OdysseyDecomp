#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al {
class LiveActor;
class Scene;
class SimpleLayoutAppearWaitEnd;
}  // namespace al

class ScenarioStartCameraHolder;
class StageSceneStateSkipDemo;

class StageSceneStateScenarioCamera : public al::HostStateBase<al::Scene> {
public:
    StageSceneStateScenarioCamera(const char*, al::Scene*, const char*, s32, al::LiveActor*);
    void setStateSkipDemo(StageSceneStateSkipDemo*);

    void setScenarioStartCameraHolder(ScenarioStartCameraHolder* holder) {
        mScenarioStartCameraHolder = holder;
    }

    void setScenarioStartLayout(al::SimpleLayoutAppearWaitEnd* layout) {
        mScenarioStartLayout = layout;
    }

    void appear() override;
    void kill() override;
    bool tryStart();
    bool isExistEnableCamera() const;
    void exeCamera();
    void exeSkip();
    ~StageSceneStateScenarioCamera() override;

private:
    al::SimpleLayoutAppearWaitEnd* mScenarioStartLayout = nullptr;
    ScenarioStartCameraHolder* mScenarioStartCameraHolder = nullptr;
    unsigned char _padding2[0x50 - 0x30];
};

static_assert(sizeof(StageSceneStateScenarioCamera) == 0x50);

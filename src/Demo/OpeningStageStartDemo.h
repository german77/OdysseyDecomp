#pragma once

#include "Demo/StageEventDemoBase.h"

namespace al {
class ActorInitInfo;
class CameraTicket;
}  // namespace al

class OpeningStageStartDemo : public StageEventDemoBase {
public:
    OpeningStageStartDemo(const char*);

    void init(const al::ActorInitInfo&) override;

    virtual void startDemo();
    virtual void startCamera(al::CameraTicket*, s32);
    virtual void endDemo();
    virtual bool isDemoLastStep() const;
    virtual bool isEndDemo() const;
    virtual bool isDrawLayout() const;
    virtual bool isStageStartDemo() const;

    void exeWait();
    void exeBeforeCameraAuto();
    void exeCameraAuto();
    void exeCameraTutorial();
    void endCamera(s32);
    void exeWaitShowStandUpGuide();
    void exeWaitStandUp();
    void exeStandUp();
};

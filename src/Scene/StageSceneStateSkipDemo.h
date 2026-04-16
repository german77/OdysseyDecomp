#pragma once

#include <basis/seadTypes.h>

#include "Library/Nerve/NerveStateBase.h"

namespace al {
class Scene;
class WindowConfirm;
class WipeHolder;
class DemoSyncedEventKeeper;
}  // namespace al

class PlayGuideSkip;
class SceneAudioSystemPauseController;
class StageSceneStateWorldMap;

class StageSceneStateSkipDemo : public al::HostStateBase<al::Scene> {
public:
    StageSceneStateSkipDemo(const char*, al::Scene*, al::WindowConfirm*, al::WipeHolder*,
                            PlayGuideSkip*, SceneAudioSystemPauseController*,
                            al::DemoSyncedEventKeeper*);
    void setWorldMapState(StageSceneStateWorldMap*);
    bool tryAppearSkipDemoLayout();
    bool isDisableSkipByWorldMap() const;
    bool tryStartSkipDemo();
    bool isDemoCancelStageScene() const { return mIsDemoCancelStageScene; }
    void tryEndForNoSkip();
    bool isCancelSkip() const;
    bool isConfirmingOfSkip() const;
    void appear() override;
    void kill() override;
    void exeConfirmSkip();
    void exeWaitConfirmClose();
    void exeSkip();
    void exeWaitSkipEnd();

private:
    void* _20 = nullptr;
    void* _28 = nullptr;
    void* _30 = nullptr;
    void* _38 = nullptr;
    void* _40 = nullptr;
    void* _48 = nullptr;
    void* _50 = nullptr;
    u32 _58 = 0;
    u32 _5c = 0;
    bool mIsDemoCancelStageScene = false;
};

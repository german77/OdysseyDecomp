#pragma once

#include <heap/seadFrameHeap.h>

#include "Library/Sequence/Sequence.h"

namespace al {
struct SequenceInitInfo;
class Scene;
class GamePadSystem;
class SimpleLayoutAppearWaitEnd;
class LayoutKit;
class ScreenCaptureExecutor;
class WipeHolder;
}  // namespace al

class GameDataHolder;
class SimpleAudioUser;

struct E3SequenceData {
    bool useSpecialControls = false;
    bool _1 = true;
    bool _2 = true;
    s32 hours = 10;
    s32 minutes = 0x78;
    s32 seconds = 0;
    s32 playTime = 0;
    s32 _14 = 0;
    s32 specialControlsDelay = 0;
};

class E3Sequence : public al::Sequence {
public:
    E3Sequence(const char* name);
    bool isDisposable() const override;
    void destroySceneHeap(bool removeCategory);
    void init(const al::SequenceInitInfo& initInfo) override;
    void initSystem();
    void deleteScene();
    void update() override;
    bool isEnableSave() const;
    void drawMain() const override;
    void updateDestroy();
    bool isAbleReset();
    al::Scene* getCurrentScene() const override;

    void exeLoadStationedResource();
    void exeTitle();
    void exeStageSelect();
    void exeEndCard();
    void exeSoftReset();
    void exeInitSystem();
    void exeLoadWorldResource();
    void exeLoadWorldResourceWithBoot();
    void exeLoadStage();
    void exePlayStage();
    void exeDestroy();
    void exeDestroyToRebootEnd();
    void exeMiss();
    void exeMissCoinSub();
    void exeMissEnd();

private:
    al::Scene* mCurrentScene;
    GameDataHolder* mGameDataHolder;
    al::GamePadSystem* mGamePadSystem;
    al::LayoutKit* mLayoutKit;
    char filler1[0xa8];
    al::ScreenCaptureExecutor* mScreenCaptureExecutor;
    al::WipeHolder* mWipeHolder;
    char filler2[0x10];
    al::SimpleLayoutAppearWaitEnd* mCounterMiss;
    char filler3[0x8];
    al::SimpleLayoutAppearWaitEnd* mE3TitleMenu;
    al::SimpleLayoutAppearWaitEnd* mE3StageSelect;
    al::SimpleLayoutAppearWaitEnd* mE3Tank;

    char filler4[0x160];
    SimpleAudioUser* mSimpleAudioUser;
    char filler5[0x8];
    E3SequenceData* mSequenceData;
};

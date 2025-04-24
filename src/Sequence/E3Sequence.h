#pragma once

#include <heap/seadFrameHeap.h>

#include "Library/Sequence/Sequence.h"

namespace al {
struct SequenceInitInfo;
class Scene;
class GamePadSystem;
class SimpleLayoutAppearWaitEnd;
class LayoutKit;
class WipeHolder;
}  // namespace al

class GameDataHolder;
class SimpleAudioUser;

struct E3SequenceData {
    bool _0 = false;
    bool _1 = true;
    bool _2 = true;
    s32 _4 = 10;
    s32 _8 = 0x78;
    s32 _c = 0;
    s32 _10 = 0;
    s32 _14 = 0;
    s32 _18 = 0;
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
    al::WipeHolder* mWipeHolder;
    al::SimpleLayoutAppearWaitEnd* mCounterMiss;
    al::SimpleLayoutAppearWaitEnd* mE3StageSelect;
    al::SimpleLayoutAppearWaitEnd* mE3Tank;

    SimpleAudioUser* mSimpleAudioUser;
    E3SequenceData* mSequenceData;
};

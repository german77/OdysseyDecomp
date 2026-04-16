#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al {
class LayoutInitInfo;
class Scene;
}  // namespace al

class MapLayout;
class SceneAudioSystemPauseController;
class StageSceneStateCollectBgm;
class StageSceneStateStageMap;

class StageSceneStateCollectionList : public al::HostStateBase<al::Scene> {
public:
    StageSceneStateCollectionList(const char*, al::Scene*, const al::LayoutInitInfo&, MapLayout*,
                                  StageSceneStateCollectBgm*, SceneAudioSystemPauseController*);
    void updateText();
    void init() override;
    void appear() override;
    void kill() override;
    void control() override;
    void setOpenByAchievementNpc();
    void setOpenByMusicNpc();
    void setOpenInWorldMap(s32);
    void exeViewStageMap();
    void exeViewShineList();
    void changeLeft();
    void changeRight();
    void exeViewHackList();
    void exeViewSouvenirList();
    void exeViewBgmList();
    void exeChangeView();
    void exeClose();
    ~StageSceneStateCollectionList() override;

    StageSceneStateStageMap* getStateStageMap() { return mStateStageMap; }

private:
    s8 padding[0x18];
    StageSceneStateStageMap* mStateStageMap;
    s8 filler[0x60];
};

static_assert(sizeof(StageSceneStateCollectionList) == 0xa0);

#pragma once

#include <basis/seadTypes.h>

namespace al {
class Scene;
}  // namespace al

class GameDataHolderAccessor;
class MapIconInfo;
class MapLayout;

class StageSceneStateStageMap {
public:
    StageSceneStateStageMap(const char*, al::Scene*, MapLayout*);
    void init();
    void appear();
    void startRace();
    void endRace();
    void exeOpen();
    void setOpenInCollectionList();
    void exeOpenInCollectionList();
    void exeCheckpointWarpTutorialWait();
    void exeCheckpointWarpTutorial();
    void exeCheckpointWarpTutorialWaitDecide();
    void exeWait();
    void tryCloseMap();
    void exeScroll();
    void tryBindIcon(f32);
    void exeAdsorb();
    void exeWaitAdsorb();
    void tryCheckpointWarp(GameDataHolderAccessor, const MapIconInfo*);
    void exeIconSelectMove();
    void exeMoveIconAdsorbRange();
    bool isEndClose() const;
    bool isEndCheckpointWarp() const;
    s32 calcWarpCheckpointIndex() const;
    bool isWarpHome() const;
    bool isAdsorbWarpIcon() const;
    bool isEnableChangeOut() const;
    void closeMap();
    void changeOut(bool);
    void changeIn(bool);
    void exeEndClose();
    void exeChangeOut();
    void getBackgroundLayout();
    bool isEnableOpenList() const;
    void openList();
    void exeChangeIn();
    void exeEndCheckpointWarp();
    ~StageSceneStateStageMap();
};

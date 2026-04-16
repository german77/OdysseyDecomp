#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al {
class Scene;
}  // namespace al

class StageSceneStateCollectionList;
class StageSceneStatePauseMenu;
class StageSceneStateSnapShot;

class StageSceneStateRadicon : public al::HostStateBase<al::Scene> {
public:
    StageSceneStateRadicon(const char*, al::Scene*, StageSceneStateCollectionList*,
                           StageSceneStatePauseMenu*, StageSceneStateSnapShot*);
    void appear() override;
    void kill() override;
    void exeRadicon();
    void exePause();
    void exeMap();
    void exeSnapShot();
    bool isKill() const { return mIsKill; }
    ~StageSceneStateRadicon() override;

private:
    unsigned char _padding[0x18];
    bool mIsKill = false;
    unsigned char _padding2[0x7];
};

static_assert(sizeof(StageSceneStateRadicon) == 0x40);

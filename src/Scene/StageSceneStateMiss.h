#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al {
class LayoutInitInfo;
class Scene;
class SubCameraRenderer;
}  // namespace al

class GameDataHolder;
class StageSceneLayout;

class StageSceneStateMiss : public al::HostStateBase<al::Scene> {
public:
    StageSceneStateMiss(const char*, al::Scene*, const al::LayoutInitInfo&, GameDataHolder*,
                        StageSceneLayout*, al::SubCameraRenderer*);
    void appear() override;
    void kill() override;
    bool checkMiss() const;
    bool isResetCollectBgm() const { return mIsResetCollectBgm; }
    void exeMiss();
    bool isPlayerDeadWipeStart() const;
    void exeMissWipe();
    ~StageSceneStateMiss() override;

private:
    unsigned char _20[0x39 - 0x20];
    bool mIsResetCollectBgm;
    unsigned char _3a[0x48 - 0x3a];
};

static_assert(sizeof(StageSceneStateMiss) == 0x48);

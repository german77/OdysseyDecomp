#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al {
class Scene;
class WipeSimple;
}  // namespace al

class GameDataHolder;
class LocationNameCtrl;

class StageSceneStateWarp : public al::HostStateBase<al::Scene> {
public:
    StageSceneStateWarp(const char*, al::Scene*, al::WipeSimple*, GameDataHolder*,
                        LocationNameCtrl*);
    void appear() override;
    void kill() override;
    bool tryStartWarp();
    void tryValidateEndEntranceCamera();
    void exeStartWarp();
    void exeWaitWarp();
    void exeEndWarp();
    ~StageSceneStateWarp() override;
};

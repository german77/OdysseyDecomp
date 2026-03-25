#pragma once

#include <cstddef>
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

    StageSceneStateStageMap* getStateStageMap() { return mStateStageMap; }

public:
    s8 padding[0x18];
    StageSceneStateStageMap* mStateStageMap;
    s8 filler[0x60];
};

static_assert(sizeof(StageSceneStateCollectionList) == 0xa0);

#pragma once

#include <basis/seadTypes.h>

#include "Library/Nerve/NerveStateBase.h"

namespace al {
struct ActorInitInfo;
}

class GameDataHolder;
class MiniGameMenu;
class StageScene;
class StageSceneLayout;
class StageSceneStateWarp;
class TimeBalloonDirector;
class TimeBalloonNpc;
class TimeBalloonSequenceInfo;
class StageSceneStateStageMap;

class StageSceneStateTimeBalloon : public al::HostStateBase<StageScene> {
public:
    StageSceneStateTimeBalloon(StageScene*, TimeBalloonDirector*, TimeBalloonSequenceInfo*, GameDataHolder*, StageSceneLayout*, al::ActorInitInfo const&, TimeBalloonNpc*, bool, MiniGameMenu*, StageSceneStateStageMap*, StageSceneStateWarp*);
    virtual bool isAutoStart() const;
    virtual bool isOpenMapNerve() const;
    virtual bool isMissNerve() const;
    virtual bool isDemoNerve() const;
    virtual bool isPauseNerve() const;
    virtual bool isTimeBalloonNerve() const;

private:
    u8 _padding[0x138];
};

static_assert(sizeof(StageSceneStateTimeBalloon) == 0x158);

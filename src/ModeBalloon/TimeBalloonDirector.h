#pragma once

#include "Library/HostIO/HioNode.h"
#include "Library/Nerve/IUseNerve.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace al {
class LiveActor;
}

class StageSceneLayout;
class TimeBalloonNpc;
class TimeBalloonSequenceInfo;

class TimeBalloonDirector : public al::HioNode, public al::IUseNerve, public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_TimeBalloonDirector;

    void init(const al::ActorInitInfo&, TimeBalloonNpc*, TimeBalloonSequenceInfo*, bool, bool,
              StageSceneLayout*);
    void requestDownload();
    void execute();
    al::LiveActor* getAccessor() const;
};

#pragma once

#include <basis/seadTypes.h>

#include "Library/HostIO/HioNode.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

class TalkNpcSceneEventSwitcher : public al::HioNode, public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_TalkNpcSceneEventSwitcher;

    TalkNpcSceneEventSwitcher();

    const char* getSceneObjName() const override { return "TalkNpcSceneEventSwitcher"; }

private:
    s8 filler[0x28 - sizeof(al::HioNode) - sizeof(al::ISceneObj)];
};

static_assert(sizeof(TalkNpcSceneEventSwitcher) == 0x28);

#pragma once

#include "Library/HostIO/HioNode.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace al {
class LiveActor;
}

class TalkNpcParam;

class TalkNpcParamHolder : public al::HioNode, public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_TalkNpcParamHolder;

    TalkNpcParamHolder();

    TalkNpcParam* findOrCreateParam(const al::LiveActor* actor, const char* name);
    TalkNpcParam* tryFindParamLocal(const al::LiveActor* actor, const char* name) const;

    const char* getSceneObjName() const override { return "トークNPCパラメータ保持"; }

private:
    u32 mTalkParamCount = 0;
    TalkNpcParam** mTalkParams = nullptr;
};

static_assert(sizeof(TalkNpcParamHolder) == 0x18);

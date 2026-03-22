#pragma once

#include "Library/HostIO/HioNode.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace al {
struct ActorInitInfo;
}

class PaintObj;

class PaintObjHolder : public al::HioNode, public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_PaintObjHolder; 

    PaintObjHolder();
    ~PaintObjHolder() override;

    void initAfterPlacementSceneObj(const al::ActorInitInfo& info) override;
    virtual void execute();
    void registerPaintObj(PaintObj* paintObj);

private:
    s8 filler[0x218 - sizeof(al::HioNode) - sizeof(al::ISceneObj)];
};

static_assert(sizeof(PaintObjHolder) == 0x218);

namespace rs {
void registerPaintObjToHolder(PaintObj*);
}

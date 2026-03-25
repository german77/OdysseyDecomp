#pragma once

#include "Library/LiveActor/LiveActor.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

class GiantWanderBoss : public al::LiveActor, public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_GiantWanderBoss;

    static bool isOnSwitchAll(GiantWanderBoss*);
    void startDemoWorldEnter();

    const char* getSceneObjName() const override { return ""; }

    void initAfterPlacementSceneObj(const al::ActorInitInfo&) override {}

    void initSceneObj() override {}
};

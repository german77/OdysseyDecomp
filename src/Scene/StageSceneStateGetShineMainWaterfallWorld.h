#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al {
struct ActorInitInfo;
class Scene;
}  // namespace al

class WaterfallWorldFallDownBridge;
class StageSceneStateGetShineMain;
class StageSceneStateGetShineMainLast;
class StageSceneStateRecoverLife;
class StageSceneStateScenarioCamera;

class StageSceneStateGetShineMainWaterfallWorld : public al::HostStateBase<al::Scene> {
public:
    static StageSceneStateGetShineMainWaterfallWorld* tryCreate(al::Scene*,
                                                                const al::ActorInitInfo&);
    StageSceneStateGetShineMainWaterfallWorld(const char*, al::Scene*,
                                              WaterfallWorldFallDownBridge*);
    void init() override;
    void appear() override;
    void kill() override;
    void setStateGetShine(StageSceneStateGetShineMain*);
    void setStateScenarioCamera(StageSceneStateScenarioCamera*);
    void setStateRecoverLife(StageSceneStateRecoverLife*);
    void setStateGetShineLast(StageSceneStateGetShineMainLast*);

    void exeDemoGetShine();
    void exeDemoDownBridge();
    void exeDemoScenarioCamera();
    void exeDemoRecoverLife();
    void exeDemoCapManHeroTalkFirstMoonGet();
    void exeDemoEnd();

private:
    s8 filler[0x48 - sizeof(al::HostStateBase<al::Scene>)];
};

static_assert(sizeof(StageSceneStateGetShineMainWaterfallWorld) == 0x48);

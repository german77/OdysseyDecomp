#pragma once

#include "Library/Nerve/NerveStateBase.h"

class StageSceneStateBreakCageShine;
class StageSceneStateGetShineMain;
class StageSceneStateScenarioCamera;
class StageSceneStateRecoverLife;
class StageSceneStateGetShineMainLast;

namespace al {
class Scene;
struct ActorInitInfo;
}  // namespace al

class StageSceneStateGetShineMainWithCageShine : public al::HostStateBase<al::Scene> {
public:
    static StageSceneStateGetShineMainWithCageShine* tryCreate(al::Scene*,
                                                               const al::ActorInitInfo&);
    StageSceneStateGetShineMainWithCageShine(const char*, al::Scene*,
                                             StageSceneStateBreakCageShine*);

    void init() override;
    void appear() override;
    void kill() override;
    void setState(StageSceneStateGetShineMain*, StageSceneStateScenarioCamera*,
                  StageSceneStateRecoverLife*, StageSceneStateGetShineMainLast*);

    void exeDemoGetShine();
    void exeDemoBreakCageShine();
    void exeDemoScenarioCamera();
    void exeDemoRecoverLife();
    void exeDemoEnd();

private:
    s8 filler[0x48 - sizeof(al::HostStateBase<al::Scene>)];
};

static_assert(sizeof(StageSceneStateGetShineMainWithCageShine) == 0x48);

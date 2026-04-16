#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al {
struct ActorInitInfo;
class LayoutInitInfo;
class LiveActor;
class Scene;
struct SceneInitInfo;
}  // namespace al

class GameDataHolder;
class ProjectItemDirector;
class StageSceneLayout;
class StageSceneStateRecoverLife;

class StageSceneStateGetShine : public al::HostStateBase<al::Scene> {
public:
    StageSceneStateGetShine(const char*, al::Scene*, StageSceneLayout*, const al::SceneInitInfo&,
                            const al::ActorInitInfo&, const al::LayoutInitInfo&, al::LiveActor*,
                            al::LiveActor*, al::LiveActor*, GameDataHolder*, ProjectItemDirector*);
    void appear() override;
    void kill() override;
    bool isDrawForward() const;
    void setModeEndAndWait();
    void setModeHack();
    void setModeKeepBind();
    void setAchievementName(const char*);
    void setShopShine10();
    void setLifeRecoverState(StageSceneStateRecoverLife*);
    void exeDemoGetFirst();
    void exeDemoGet();
    void exeDemoShineCount();
    void exeDemoLifeRecover();
    void exeDemoEndWaitScreenFader();
    void exeDemoEnd();
    void exeDemoEndAndWait();
    bool isDemoEnd() const;
    bool isPowerStar() const;
    ~StageSceneStateGetShine() override;
};

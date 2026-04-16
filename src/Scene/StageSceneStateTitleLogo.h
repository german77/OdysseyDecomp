#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al {
struct ActorInitInfo;
class LayoutInitInfo;
class Scene;
class SimpleLayoutAppearWaitEnd;
class WipeHolder;
}  // namespace al

class GameDataHolder;

class StageSceneStateTitleLogo : public al::HostStateBase<al::Scene> {
public:
    static StageSceneStateTitleLogo* tryCreate(al::Scene*, const al::ActorInitInfo&,
                                               const al::LayoutInitInfo&, al::WipeHolder*,
                                               GameDataHolder*);
    StageSceneStateTitleLogo(const char*, al::Scene*, al::SimpleLayoutAppearWaitEnd*,
                             al::WipeHolder*, GameDataHolder*, const al::ActorInitInfo&);
    void appear() override;
    void kill() override;
    void exeWait();
    ~StageSceneStateTitleLogo() override;
};

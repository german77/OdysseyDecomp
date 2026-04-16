#pragma once

#include "Library/LiveActor/LiveActorGroup.h"
#include "Library/Nerve/NerveStateBase.h"

namespace al {
struct ActorInitInfo;
class LayoutInitInfo;
class Scene;
}  // namespace al

class CollectBgmPlayer;
class CollectBgmSpeaker;
class SceneAudioSystemPauseController;

class StageSceneStateCollectBgm : public al::HostStateBase<al::Scene> {
public:
    static StageSceneStateCollectBgm* tryCreate(al::Scene*, const al::ActorInitInfo&,
                                                const al::LayoutInitInfo&, CollectBgmPlayer*);
    StageSceneStateCollectBgm(const char*, al::Scene*, al::DeriveActorGroup<CollectBgmSpeaker>*,
                              const al::LayoutInitInfo&, CollectBgmPlayer*);
    void updateList();
    void appear() override;
    void kill() override;
    void changeIn(bool);
    void setSelectedIdxPlayingBgm();
    void changeOut(bool);
    void forceStopCollectBgm();
    void calcCursorPos(sead::Vector2f*) const;
    void exeAppear();
    void exeWait();
    void exeDecide();
    void exeReject();
    void exeChangeIn();
    void exeChangeOut();
    bool tryOpenCollectBgm();
    bool isEnableChangeCollectionListPage() const;
    ~StageSceneStateCollectBgm() override;
};

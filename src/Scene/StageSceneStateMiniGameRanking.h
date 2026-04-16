#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al {
struct ActorInitInfo;
class LayoutInitInfo;
class Nerve;
class Scene;
}  // namespace al

enum class RankingCategory;

class StageSceneStateMiniGameRanking : public al::HostStateBase<al::Scene> {
public:
    static StageSceneStateMiniGameRanking* tryCreate(al::Scene*, const al::ActorInitInfo&,
                                                     const al::LayoutInitInfo&);
    StageSceneStateMiniGameRanking(const char*, al::Scene*, const al::ActorInitInfo&,
                                   const al::LayoutInitInfo&);
    void tryCreateRankingNpcs(const al::ActorInitInfo&);
    ~StageSceneStateMiniGameRanking() override;
    void init() override;
    void appear() override;
    bool tryOpen();
    void exeUpload();
    void exeWaitEndConfirm();
    void exeConnectToInternet();
    void exeConnectToInternetNoHandleError();
    void exeConnectToInternetNoHandleErrorWait();
    void exeWaitEndConfirmToClose();
    void exeAppear();
    void exeViewRanking();
    void updateViewRanking(RankingCategory);
    void changeView(RankingCategory, bool);
    void updateDownloadRankingData(RankingCategory, const al::Nerve*);
    void getRankingType() const;
    void control() override;
    void exeChangeView();
    void exeDownloadRankingData();
    void exeClose();
    void exeHandleError();
};

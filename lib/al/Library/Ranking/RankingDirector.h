#pragma once

#include "Library/HostIO/HioNode.h"
#include "Library/Network/IUseNexNetworkStateHolder.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace nn::nex::RankingConstants {
class RankingMode;
}

namespace nn::nex {
class Credentials;
class RankingScoreData;
class RankingOrderParam;
}  // namespace nn::nex

namespace al {
class RankingDirector : public al::ISceneObj,
                        public al::IUseNexNetworkStateHolder,
                        public al::HioNode {
public:
    static constexpr s32 sSceneObjId = SceneObjID_alRankingDirector;

    RankingDirector();
    ~RankingDirector() override;

    NexNetworkStateHolder* getNexNetworkStateHolder() const override;
    void init(nn::nex::Credentials*);
    bool isAvailable() const;
    bool startUploadCommonData(const void*, u8);
    bool tryEndUploadCommonData();
    bool tryStartUploadScore(const nn::nex::RankingScoreData&);
    bool tryEndUploadScore();
    bool startDeleteScore(u32);
    bool startDeleteScore();
    bool tryEndDeleteScore();
    bool startGetRanking(u32, const nn::nex::RankingConstants::RankingMode&,
                         const nn::nex::RankingOrderParam&);
    bool startGetCachedTopXRanking(u32, const nn::nex::RankingOrderParam&);
    bool tryEndGetRanking(bool*);
    bool startGetApproxOrder(u32*, u32, u32, const nn::nex::RankingOrderParam&);
    bool tryEndGetApproxOrder();

    const char* getSceneObjName() const override { return "ランキングディレクタ"; }

private:
    s8 filler[0x48 - sizeof(al::ISceneObj) - sizeof(al::IUseNexNetworkStateHolder) -
              sizeof(al::HioNode)];
};

}  // namespace al

static_assert(sizeof(al::RankingDirector) == 0x48);

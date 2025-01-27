#include "Item/CoinCollectHolder.h"

#include "Library/Area/AreaObjUtil.h"
#include "Library/LiveActor/ActorFlagFunction.h"

#include "Item/CoinCollect.h"
#include "Item/CoinCollect2D.h"
#include "MapObj/CoinCollectHintObj.h"
#include "Scene/SceneObjFactory.h"

CoinCollectHolder::CoinCollectHolder() {
    mCollect.allocBuffer(100, nullptr);
    mCollect2D.allocBuffer(100, nullptr);
    mCollectHintObj.allocBuffer(10, nullptr);
}

const char* CoinCollectHolder::getSceneObjName() const {
    return "コレクトコイン保持者";
}

void CoinCollectHolder::registerCoinCollect(CoinCollect* coin) {
    mCollect.pushBack(coin);
}

void CoinCollectHolder::registerCoinCollect2D(CoinCollect2D* coin) {
    mCollect2D.pushBack(coin);
}

void CoinCollectHolder::registerHintObj(CoinCollectHintObj* hintObj) {
    mCollectHintObj.pushBack(hintObj);
}

CoinCollect* CoinCollectHolder::tryFindAliveCoinCollect(const sead::Vector3f&, bool) const {}

CoinCollect* CoinCollectHolder::tryFindAliveCoinCollect(const sead::Vector3f&, f32, f32,
                                                        bool) const {}

CoinCollect* CoinCollectHolder::tryFindDeadButHintEnableCoinCollect() const {
    for (s32 i = 0; i < mCollect.size(); i++) {
        CoinCollect* coin = mCollect[i];
        if (al::isAlive(coin))
            continue;

        sead::Vector3f playerPOs = coin->getTransForHint();
        auto* areaobj = al::tryFindAreaObj(coin, "InvalidateStageMapArea", playerPOs);
        return coin;
    }
    return nullptr;
}

CoinCollect2D* CoinCollectHolder::tryFindAliveCoinCollect2D(const sead::Vector3f&, bool) const {}

CoinCollect2D* CoinCollectHolder::tryFindAliveCoinCollect2D(const sead::Vector3f&, f32, f32,
                                                            bool) const {}

CoinCollectHintObj* CoinCollectHolder::tryFindExStageHintObjTrans(sead::Vector3f*, const char*) {}

namespace rs {
CoinCollectHolder* createCoinCollectHolder(const al::IUseSceneObjHolder* objHolder) {
    return (CoinCollectHolder*)al::createSceneObj(objHolder, SceneObjID_CoinCollectHolder);
}
}  // namespace rs

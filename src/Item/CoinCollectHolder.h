#pragma once

#include <basis/seadTypes.h>
#include <container/seadPtrArray.h>
#include <math/seadVector.h>

#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace al {
class IUseSceneObjHolder;
}  // namespace al

class CoinCollect;
class CoinCollect2D;
class CoinCollectHintObj;

class CoinCollectHolder : public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_CoinCollectHolder;

    CoinCollectHolder();

    const char* getSceneObjName() const override;

    void registerCoinCollect(CoinCollect*);
    void registerCoinCollect2D(CoinCollect2D*);
    void registerHintObj(CoinCollectHintObj*);

    CoinCollect* tryFindAliveCoinCollect(const sead::Vector3f&, bool) const;
    CoinCollect* tryFindAliveCoinCollect(const sead::Vector3f&, f32, f32, bool) const;
    CoinCollect* tryFindDeadButHintEnableCoinCollect() const;
    CoinCollect2D* tryFindAliveCoinCollect2D(const sead::Vector3f&, bool) const;
    CoinCollect2D* tryFindAliveCoinCollect2D(const sead::Vector3f&, f32, f32, bool) const;
    CoinCollectHintObj* tryFindExStageHintObjTrans(sead::Vector3f*, const char*);

private:
    sead::PtrArray<CoinCollect> mCollect;
    sead::PtrArray<CoinCollect2D> mCollect2D;
    sead::PtrArray<CoinCollectHintObj> mCollectHintObj;
};

namespace rs {
CoinCollectHolder* createCoinCollectHolder(const al::IUseSceneObjHolder* objHolder);
}  // namespace rs

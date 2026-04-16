#pragma once

#include "Library/LiveActor/LiveActorGroup.h"
#include "Library/Nerve/NerveStateBase.h"

namespace al {
struct ActorInitInfo;
}  // namespace al

class ShoppingWatcher;
class StageScene;
class StageSceneLayout;

class StageSceneStateShop : public al::HostStateBase<StageScene> {
public:
    StageSceneStateShop(const char*, StageScene*, StageSceneLayout*);
    void initAndSetting(al::DeriveActorGroup<ShoppingWatcher>*, const al::ActorInitInfo&);
    void appear() override;
    void kill() override;
    void control() override;
    void getShopNpc() const;
    void exeInShop();
    void exeBuyShineEnd();
    void exeBuyLifeMaxUpItem();
    bool isWear() const;
    bool isEndBuyShine() const;
    bool isEndBuyShine10() const;
    bool isEndBuyLifeMaxUpItem() const;
    void settingShopId(s32);
    void requestOpen();
    bool isDrawViewRenderer() const;
    ~StageSceneStateShop() override;

private:
    unsigned char _padding[0x50 - 0x20];
};

static_assert(sizeof(StageSceneStateShop) == 0x50);

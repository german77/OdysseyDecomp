#pragma once

#include "Library/Event/IEventFlowEventReceiver.h"
#include "Library/Event/IEventFlowQueryJudge.h"
#include "Library/LiveActor/LiveActor.h"

class Doshi;

class ShoppingWatcher : public al::LiveActor,
                        public al::IEventFlowEventReceiver,
                        public al::IEventFlowQueryJudge {
public:
    ShoppingWatcher(const char*, const char*, Doshi*);

    void init(const al::ActorInitInfo&) override;
    bool isAfterBuyWear() const;
    bool isAfterBuyShine() const;
    bool isAfterBuyLifeUpItem() const;
    void initAfterPlacement() override;
    void tryStartCameraAfterBuyItem();
    void control() override;
    void attackSensor(al::HitSensor*, al::HitSensor*) override;
    bool receiveMsg(const al::SensorMsg*, al::HitSensor*, al::HitSensor*) override;
    bool receiveEvent(const al::EventFlowEventData*) override;
    const char* judgeQuery(const char*) const override;

    void exeWait();
    void exeScare();
    void exeShop();
    void exeDemoWear();
    void tryEndCameraAfterBuyItem();
    void exeDemoGetShine();
    void exeDemoGetLifeUpItem();
    void exeEnd();
    void exeReaction();
    void exeTurnToInitFront();
    void exeTimeBalloonOrRace();

    bool isAliveNpcInShop() const;
    bool isWait() const;
    bool isShop() const;
    void endShop();
    void appearNpcInShop();
    void killNpcInShop();
    const sead::Vector3f& getDoshiPos();
    void calcDoshiFrontDir(sead::Vector3f*);
    bool isInsideTerritoryPlayer() const;
    void requestStartDoshiCamera();
    void requestEndDoshiCamera();

private:
    s8 filler[0x288 - sizeof(al::LiveActor) - sizeof(al::IEventFlowEventReceiver) -
              sizeof(al::IEventFlowQueryJudge)];
};

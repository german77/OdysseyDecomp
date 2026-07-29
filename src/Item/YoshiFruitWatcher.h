#pragma once

#include "Library/LiveActor/LiveActor.h"
#include "Library/Scene/ISceneObj.h"

namespace al {
struct ActorInitInfo;
}

class YoshiFruitShineHolder;
class SaveObjInfo;
class Yoshi;

class YoshiFruitWatcher : public al::LiveActor, public al::ISceneObj {
public:
    YoshiFruitWatcher();

    void initAfterPlacementSceneObj(const al::ActorInitInfo&) override;
    void registerShineHolder(YoshiFruitShineHolder*);
    void registerFruit(al::LiveActor*, SaveObjInfo*);
    void noticeCurrentHackYoshi(Yoshi*);
    void noticeGetFruit(al::LiveActor*, SaveObjInfo*);
    void saveGetFruit();
    void control() override;
    void exeWait();
    void exeGaugeAppear();
    void exeGaugeWait();
    void exeGaugeEnd();
    void exeDemoRequest();
    void exeDemoGauge();
    void exeDemoShine();
    const char* getSceneObjName() const override;

private:
};

namespace rs {

void registerFruit(al::LiveActor*, SaveObjInfo*);
void registerFruitShineHolder(YoshiFruitShineHolder*);
void noticeCurrentHackYoshi(Yoshi*);
void noticeGetFruit(al::LiveActor*, SaveObjInfo*);

}  // namespace rs

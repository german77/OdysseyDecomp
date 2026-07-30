#pragma once

#include <container/seadPtrArray.h>

#include "Library/LiveActor/LiveActor.h"
#include "Library/Scene/ISceneObj.h"

namespace al {
struct ActorInitInfo;
class LayoutActor;
}  // namespace al

class YoshiFruitShineHolder;
class SaveObjInfo;
class Yoshi;
class Shine;

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
    sead::PtrArray<LiveActor> _110;
    sead::PtrArray<LiveActor> _120;
    YoshiFruitShineHolder* mShineHolder;
    al::LiveActor* _138;
    s32 _140;
    Shine* mShine;
    al::LayoutActor* mLayoutActor;
};

static_assert(sizeof(YoshiFruitWatcher) == 0x158);

namespace rs {

void registerFruit(al::LiveActor*, SaveObjInfo*);
void registerFruitShineHolder(YoshiFruitShineHolder*);
void noticeCurrentHackYoshi(Yoshi*);
void noticeGetFruit(al::LiveActor*, SaveObjInfo*);

}  // namespace rs

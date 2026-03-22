#pragma once

#include <basis/seadTypes.h>

#include "Library/HostIO/HioNode.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace al {
class LiveActor;
}

class StageTalkDemoNpcCap;

class CapManHeroDemoDirector : public al::HioNode, public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_CapManHeroDemoDirector; 

    CapManHeroDemoDirector();
    ~CapManHeroDemoDirector() override;

    void init(const al::ActorInitInfo&);
    bool isEndDemo() const;
    bool isExistTalkDemoStageStart() const;
    bool isExistTalkDemoMoonRockFind() const;
    bool isExistTalkDemoAfterMoonRockBreakDemo() const;
    void preEventFromSceneFirstMoonGet(const char*);
    void startTalkDemoFirstMoonGet();
    void startTalkDemoCore(StageTalkDemoNpcCap*);
    void startTalkDemoStageStart();
    void startTalkDemoMoonRockFind();
    void startTalkDemoAfterMoonRockBreakDemo();

    const char* getSceneObjName() const override { return "キャップ会話ディレクター"; }

private:
    s8 filler[0x40 - sizeof(al::HioNode) - sizeof(al::ISceneObj)];
};

static_assert(sizeof(CapManHeroDemoDirector) == 0x40);

class CapManHeroDemoFunction {
public:
    void capManHeroControl(al::LiveActor*);
    void createDemoCapManHero(const char*, const al::ActorInitInfo&, const char*);
    void startCapManHeroCommonSettingAfterShowModel(al::LiveActor*);
};

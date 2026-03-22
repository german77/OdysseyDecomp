#pragma once

#include "Library/LiveActor/LiveActor.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

class CapMessagePlacement;
class CapMessageShowInfo;
class CapMessageGameDataChecker;

class CapMessageDirector : public al::LiveActor, public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_CapMessageDirector;

    CapMessageDirector();

    void initAfterPlacementSceneObj(const al::ActorInitInfo& info) override;
    void exeWait();
    bool tryStartShowPlacement();
    void exeDelayPlacement();
    CapMessagePlacement* findPlacement() const;
    void exeShowPlacement();
    void forceEndInner();
    void exeShowSystemLow();
    void exeShowSystem();
    void exeShowSystemContinue();
    void exeEnd();
    bool isShow(const char*) const;
    bool isDelay(const char*) const;
    bool isActive(const char*) const;
    void registerCapMessagePlacement(CapMessagePlacement*);
    bool tryShowMessageSystem(const CapMessageShowInfo*, const CapMessageGameDataChecker*);
    bool tryCheck(const CapMessageGameDataChecker*) const;
    bool tryShowMessageSystemLow(const CapMessageShowInfo*, const CapMessageGameDataChecker*);
    bool tryShowMessageSystemContinue(const CapMessageShowInfo*, const CapMessageGameDataChecker*);
    void endCapMessageSystemContinue();
    void invalidateAppearCapMessage();
    void validateAppearCapMessage();
    void forceEnd();

    const char* getSceneObjName() const override { return "帽子メッセージディレクター(新)"; }

private:
    s8 filler[0x138 - sizeof(al::LiveActor) - sizeof(al::ISceneObj)];
};

static_assert(sizeof(CapMessageDirector) == 0x138);

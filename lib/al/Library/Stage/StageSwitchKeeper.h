#pragma once

#include <basis/seadTypes.h>

namespace al {
class ExecuteDirector;
class PlacementId;
class StageSwitchAccesser;
class StageSwitchDirector;
class StageSwitchListener;
class PlacementInfo;
class IUseStageSwitch;
class FunctorBase;
class IUseName;

struct StageSwitchAccesserList {
    StageSwitchAccesserList();
    StageSwitchAccesserList(const StageSwitchAccesser* accessers);

    const StageSwitchAccesser* accessers = nullptr;
    s32 accesserSize = 0;
    s32 accesserCapacity = 0;
};

class StageSwitchKeeper {
public:
    StageSwitchKeeper();
    void init(StageSwitchDirector* director, const PlacementInfo& placementInfo);
    StageSwitchAccesser* tryGetStageSwitchAccesser(const char* linkName) const;

    void setUseName(IUseName* useName) { mUseName = useName; }

private:
    StageSwitchAccesser* mAccessers = nullptr;
    s32 mAccesserSize = 0;
    IUseName* mUseName = nullptr;
};

class StageSwitchDirector : public HioNode, public IUseExecutor {
public:
    StageSwitchDirector(ExecuteDirector*);

    void execute() override;

    void useSwitch(const StageSwitchAccesser*);
    void* findSwitchNoFromObjId(const PlacementId*);  // unknown return type
    void onSwitch(const StageSwitchAccesser*);
    void offSwitch(const StageSwitchAccesser*);
    bool isOnSwitch(const StageSwitchAccesser*);
    void addListener(StageSwitchListener*, StageSwitchAccesser*);

private:
    void* filler[3];
};

static_assert(sizeof(StageSwitchDirector) == 0x20);

bool tryOnStageSwitch(IUseStageSwitch*, const char*);
bool tryOffStageSwitch(IUseStageSwitch*, const char*);
bool tryOnSwitchDeadOn(IUseStageSwitch* stageSwitch);
bool listenStageSwitchOn(IUseStageSwitch* stageSwitchHolder, const char* eventName,
                         const FunctorBase& actionOnOn);
bool listenStageSwitchOnAppear(IUseStageSwitch* stageSwitchHolder,
                               const FunctorBase& actionOnAppear);
bool listenStageSwitchOff(IUseStageSwitch* stageSwitchHolder, const char* eventName,
                         const FunctorBase& actionOnOff);
bool listenStageSwitchOnOff(IUseStageSwitch* stageSwitchHolder, const char* eventName,
                            const FunctorBase& actionOnOn, const FunctorBase& actionOnOff);
bool listenStageSwitchOnOffStart(IUseStageSwitch* stageSwitchHolder, const FunctorBase& actionOnOn,
                                 const FunctorBase& actionOnOff);
bool listenStageSwitchOnOffAppear(IUseStageSwitch* stageSwitchHolder, const FunctorBase& actionOnOn,
                                  const FunctorBase& actionOnOff);
bool listenStageSwitchOnKill(IUseStageSwitch* stageSwitchHolder, const FunctorBase& actionOnOn);
bool listenStageSwitchOnOffKill(IUseStageSwitch* stageSwitchHolder, const FunctorBase& actionOnOn,
                                const FunctorBase& actionOnOff);
bool listenStageSwitchOnStart(IUseStageSwitch* stageSwitchHolder, const FunctorBase& actionOnOn);
bool listenStageSwitchOnStop(IUseStageSwitch* stageSwitchHolder, const FunctorBase& actionOnOn);
}  // namespace al

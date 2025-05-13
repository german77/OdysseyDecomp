#pragma once

#include <basis/seadTypes.h>

namespace al {
class StageResourceList;

class StageResourceKeeper {
public:
    StageResourceKeeper();

    void initAndLoadResource(const char* stageName, s32 scenarioNo);

    StageResourceList* getMapStageInfo() const { return mResourceList[0]; }
    StageResourceList* getDesignStageInfo() const { return mResourceList[1]; }
    StageResourceList* getSoundStageInfo() const { return mResourceList[2]; }

private:
    StageResourceList** mResourceList = nullptr;
    const char* mStageName = nullptr;
    s32 mScenarioNo = -1;
};

}  // namespace al

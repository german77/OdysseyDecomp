#pragma once

#include <basis/seadTypes.h>
#include <prim/seadSafeString.h>

namespace al {
class IUseSceneObjHolder;
class LayoutActor;
}  // namespace al

namespace rs {

const char16* getWorldCoinCollectPictureFont(al::LayoutActor*, s32);
const char16* getWorldCoinCollectPictureFont(al::LayoutActor*);
bool trySetPaneStageMessageIfExist(al::LayoutActor*, const char*, const char*, const char*);
bool trySetPaneSystemMessageIfExist(al::LayoutActor*, const char*, const char*, const char*);
bool tryGetMapMainScenarioLabel(sead::BufferedSafeStringBase<char>*,
                                sead::BufferedSafeStringBase<char>*, bool*,
                                const al::IUseSceneObjHolder*);

const char* getCheckpointLabelPrefix();
}  // namespace rs

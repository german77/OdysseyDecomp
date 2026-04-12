#pragma once

#include <basis/seadTypes.h>

namespace al {
class IUseSceneObjHolder;
}  // namespace al

namespace rs {
bool isExistShineChipWatcher(const al::IUseSceneObjHolder*);
bool isCompleteShineChip(const al::IUseSceneObjHolder*);
s32 getShineChipCount(const al::IUseSceneObjHolder*);
s32 getCurrentShineChipWatcherIndex(const al::IUseSceneObjHolder*);
bool isCurrentShineChipWatcherTypeEmpty(const al::IUseSceneObjHolder*);
bool isAppearedShineChipShine(const al::IUseSceneObjHolder*);
void addDemoActorShineChipWatcher(const al::IUseSceneObjHolder*);
bool isEnableStartShineChipCompleteDemo(const al::IUseSceneObjHolder*);
void tryStartAppearShineChipShine(const al::IUseSceneObjHolder*);
bool isInAreaCurrentShineChipWatcher(const al::IUseSceneObjHolder*);
}  // namespace rs


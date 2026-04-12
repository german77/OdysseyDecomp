#pragma once

#include <basis/seadTypes.h>

namespace al {
class AudioDirector;
class IUseAudioKeeper;
class LiveActor;
class Scene;
}  // namespace al

namespace rs {
void procForRestart(const al::AudioDirector*, const al::AudioDirector*, bool);
void endBgmSituationForSceneTransition(const al::IUseAudioKeeper*);
void tryRegistBgmResourceNameToBgmRhythmSyncDirector(const al::IUseAudioKeeper*);
void stopCollectBgm(al::Scene*, s32);
void stopCollectBgm(al::LiveActor*, s32);
}  // namespace rs


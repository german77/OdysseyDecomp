#include "Project/Audio/AudioEffectProcInfo.h"

namespace al {

AudioDemoSyncedProcInfo::AudioDemoSyncedProcInfo() = default;

AudioDemoSyncedProcInfo::AudioDemoSyncedProcInfo(const AudioDemoSyncedProcInfo& info)
    : name(info.name), isEnableEvent(info.isEnableEvent), isPartsProc(info.isPartsProc),
      list(info.list) {}

}  // namespace al

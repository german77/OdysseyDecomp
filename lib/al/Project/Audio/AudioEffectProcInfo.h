#pragma once

#include <container/seadPtrArray.h>

namespace al {

struct AudioDemoSyncedProcInfo {
    AudioDemoSyncedProcInfo();
    AudioDemoSyncedProcInfo(const AudioDemoSyncedProcInfo& info);

    const char* name = nullptr;
    bool isEnableEvent = false;
    bool isPartsProc = false;
    sead::PtrArray<const char>* list = nullptr;
};

static_assert(sizeof(AudioDemoSyncedProcInfo) == 0x18);

}  // namespace al

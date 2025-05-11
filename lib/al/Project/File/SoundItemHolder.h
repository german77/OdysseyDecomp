#pragma once

#include <basis/seadTypes.h>
#include <container/seadBuffer.h>

#include "Project/File/SoundItemEntry.h"
#include "Project/FileEntryBase.h"

namespace sead {
class ArchiveRes;
class FileDevice;
}  // namespace sead

namespace al {

class SoundItemHolder {
public:
    SoundItemHolder();
    SoundItemEntry* addNewLoadRequestEntry(u32, u32, IAudioResourceLoader*);
    SoundItemEntry* tryFindEntry(u32, IAudioResourceLoader*);
    void waitLoadDoneAll();
    void clearEntry();

private:
    sead::Buffer<SoundItemEntry> mSoundItemEntries;
    s32 mSize = 0;
};

}  // namespace al

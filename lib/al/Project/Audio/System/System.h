#pragma once

#include <basis/seadTypes.h>
#include <nn/atk.h>
#include <nn/atk/atk_SoundHeap.h>

#include "Library/HostIO/HioNode.h"

namespace alsd {

class AudioSoundHeapMulti : public nn::atk::SoundHeap, public sead::hostio::Node {
public:
    AudioSoundHeapMulti(unsigned long, sead::Heap*);

    bool isCrossingMemoryBoundary();
    void setSoundDataManagement(nn::atk::SoundDataManager&, nn::atk::SoundArchive&);
    void genMessage(sead::hostio::Context*);
    void listenPropertyEvent(const sead::hostio::PropertyEvent*);
    void dump();

private:
    void create_(u64, sead::Heap*);

    char filler[0x18];
};

static_assert(sizeof(AudioSoundHeapMulti) == 0x70);

class SoundHandle : public nn::atk::SoundHandle {
public:
    void stop(s32);
    void pause(s32);
    void unpause(s32);
    void setVolume(f32, s32);
    void setPitch(f32);
    void setPan(f32);
    bool isAttachedSound();
    s32 getSoundId();
};

static_assert(sizeof(SoundHandle) == 0x8);

}  // namespace alsd

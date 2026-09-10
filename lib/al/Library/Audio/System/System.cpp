#include "Library/Audio/System/System.h"

#include <nn/atk/atk_HardwareManager.h>
#include <nn/atk/atk_SoundPlayer.h>
#include <nn/atk/atk_SoundSystem.h>
#include <nn/audio.h>
#include <prim/seadScopedLock.h>

#include "Project/Audio/System/System.h"

namespace alsd {
AudioPlayerMulti::AudioPlayerMulti() {
    mAudioSoundDataMgrMulti = new AudioSoundDataMgrMulti();
}

void AudioPlayerMulti::initialize() {}

void AudioPlayerMulti::finalize() {
    if (!IsAvailable())
        return;

    stopAll(0);
    destroySoundHeap();

    if (mMemoryPool) {
        nn::audio::RequestDetachMemoryPool(mMemoryPool);
        while (nn::audio::IsMemoryPoolAttached(mMemoryPool))
            ;
        auto& hardwareManager = nn::atk::detail::driver::HardwareManager::GetInstance();
        nn::audio::ReleaseMemoryPool(hardwareManager.GetConfig(), mMemoryPool);
    }

    shutdownDataManagement();
    mAudioSoundDataMgrMulti->unmountSoundArchive();
}

void AudioPlayerMulti::calc() {
    if (!nn::atk::SoundSystem::IsInitialized())
        return;

    if (mIsMultithreaded) {
        sead::ScopedLock<sead::CriticalSection> lock(&mCriticalSection);
        Update();
        return;
    }

    Update();
}

bool AudioPlayerMulti::startSound(SoundHandle* handle, u32 id) {
    return StartSound(handle, id, nullptr).m_Code ==
           nn::atk::SoundStartable::StartResult::ResultCode_Success;
}

bool AudioPlayerMulti::startSound(SoundHandle* handle, const char* name) {
    return StartSound(handle, name, nullptr).m_Code ==
           nn::atk::SoundStartable::StartResult::ResultCode_Success;
}

bool AudioPlayerMulti::holdSound(SoundHandle* handle, u32 id) {
    return HoldSound(handle, id, nullptr).m_Code ==
           nn::atk::SoundStartable::StartResult::ResultCode_Success;
}

bool AudioPlayerMulti::holdSound(SoundHandle* handle, const char* name) {
    return HoldSound(handle, name, nullptr).m_Code ==
           nn::atk::SoundStartable::StartResult::ResultCode_Success;
}

s32 AudioPlayerMulti::getSoundCount() const {
    alsd::AudioFsSoundArchiveCafe* archive = mAudioSoundDataMgrMulti->getSoundArchive();
    // return archive ? archive->GetSoundCount() : 0;
}

const char* AudioPlayerMulti::getSoundName(u32 id) const {
    alsd::AudioFsSoundArchiveCafe* archive = mAudioSoundDataMgrMulti->getSoundArchive();
    // return archive ? archive->GetItemLabel(id) : nullptr;
}

s32 AudioPlayerMulti::getSoundId(const char* name) const {
    alsd::AudioFsSoundArchiveCafe* archive = mAudioSoundDataMgrMulti->getSoundArchive();
    // return archive ? archive->GetItemId(name) : -1;
}

void AudioPlayerMulti::stopAll(s32 id) {
    u32 playerCount = GetPlayerCount();
    for (u32 i = 0; i < playerCount; i++)
        GetSoundPlayer(0x4000000 + i)->StopAllSound(id);
}

void AudioPlayerMulti::destroySoundHeap() {
    mAudioSoundDataMgrMulti->connectSoundHeap(nullptr);
    if (mAudioSoundHeapMulti) {
        delete mAudioSoundHeapMulti;
        mAudioSoundHeapMulti = nullptr;
    }
}

void AudioPlayerMulti::shutdownDataManagement() {
    Finalize();
    if (mStreamBuffer) {
        delete[] mStreamBuffer;
        mStreamBuffer = nullptr;
    }
    if (mPlayerBuffer) {
        delete[] mPlayerBuffer;
        mPlayerBuffer = nullptr;
    }
}

void AudioPlayerMulti::pauseAll(s32) {}

void AudioPlayerMulti::unpauseAll(s32) {}

nn::atk::SoundStartable::StartResult
AudioPlayerMulti::detail_SetupSound(nn::atk::SoundHandle* handle, u32 a, bool b, const char* c,
                                    const nn::atk::SoundStartable::StartInfo* startInfo) {
    return nn::atk::SoundArchivePlayer::detail_SetupSound(handle, a, b, c, startInfo);
}

void AudioPlayerMulti::createSoundHeap(u64, sead::Heap*) {}

void AudioPlayerMulti::setupDataManagement(u32, u32, u32, sead::Heap*, s32) {}

void AudioPlayerMulti::setupDataManagement(const DataManagementSetupParam&) {}

bool AudioPlayerMulti::isPlayerBufferCrossingMemoryBoundary() const {}

bool AudioPlayerMulti::isStreamBufferCrossingMemoryBoundary() const {}

void AudioPlayerMulti::genMessage(sead::hostio::Context*) {}

void AudioPlayerMulti::listenPropertyEvent(const sead::hostio::PropertyEvent*) {}

bool AudioPlayerMulti::isNwEnabled_() const {
    return true;
}

void AudioPlayerMulti::setPauseAll_(s32, bool) {}

bool AudioPlayerMulti::setupDataManagementInner_(const nn::atk::SoundArchive&, u32, u32, u32,
                                                 sead::Heap*, s32) {}
}  // namespace alsd

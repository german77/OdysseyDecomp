#pragma once

namespace al {
class IUseAudioKeeper;
void createAudioEffect(const IUseAudioKeeper* keeper, const char* name);
void startAudioEffect(const IUseAudioKeeper* keeper, const char* name);
void startAudioEffectDefaultArea(const IUseAudioKeeper* keeper);
}  // namespace al

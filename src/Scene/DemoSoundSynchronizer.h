#pragma once

#include <basis/seadTypes.h>

namespace al {
class AudioDirector;
class ApplicationMessageReceiver;
class DemoSyncedEventKeeper;
}  // namespace al

class DemoSoundSynchronizer {
public:
    DemoSoundSynchronizer(const al::ApplicationMessageReceiver* receiver, al::AudioDirector* director);

    void update();
    void trySync(s32, al::DemoSyncedEventKeeper*);
    void endSync();
    void startSync();

private:
    u8 _padding[0x20];
};

static_assert(sizeof(DemoSoundSynchronizer) == 0x20);

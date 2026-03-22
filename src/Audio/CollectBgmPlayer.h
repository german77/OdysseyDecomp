#pragma once

#include <basis/seadTypes.h>
#include "Library/HostIO/HioNode.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace al {
class IUseAudioKeeper;
}

class CollectBgmPlayer : public al::HioNode, public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_CollectBgmPlayer;

    CollectBgmPlayer();

    void init(const al::IUseAudioKeeper*);
    void prepare();
    void start(const char*, const char* bgm);
    void stop(s32);
    bool isPlaying(const char*, const char* bgm) const;

    void reset() {
        _10 = nullptr;
        bgmName = nullptr;
    }

private:
    const al::IUseAudioKeeper* audioKeeper = nullptr;
    const char* _10 = nullptr;
    const char* bgmName = nullptr;
};

static_assert(sizeof(CollectBgmPlayer) == 0x20);

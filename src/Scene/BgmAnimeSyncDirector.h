#pragma once

#include "Library/Audio/IUseAudioKeeper.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

class BgmAnimeSyncDirector : public al::ISceneObj, public al::IUseAudioKeeper {
public:
    static constexpr s32 sSceneObjId = SceneObjID_BgmAnimeSyncDirector;

    BgmAnimeSyncDirector();

    void initAfterPlacementSceneObj(const al::ActorInitInfo&) override;
    s32 calcArchiveAndActionIndex(const char*, const char*);
    f32 calcNecessaryMeasuresNumScale(s32);

    al::AudioKeeper* getAudioKeeper() const override { return mAudioKeeper; }

private:
    al::AudioKeeper* mAudioKeeper = nullptr;
    s32 _18 = -1;
};

static_assert(sizeof(BgmAnimeSyncDirector) == 0x20);

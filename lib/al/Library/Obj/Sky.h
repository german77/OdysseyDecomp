#pragma once

#include "Library/LiveActor/LiveActor.h"

namespace al {

class Sky : public LiveActor {
public:
    Sky(const char* name);

    void init(const ActorInitInfo&) override;
    void initFromPreset(const ActorInitInfo&);
    void control() override;

    bool isOnlyCubeMap() const { return mIsOnlyCubeMap; }

private:
    s32 _108;
    f32* _110;
    s32 mTexturePatternId;
    bool mIsOnlyCubeMap;
};

static_assert(sizeof(Sky) == 0x120);

}  // namespace al

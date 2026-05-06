#pragma once

#include "Library/HostIO/HioNode.h"

namespace al {
class AnimInfoTable;

class AnimPlayerBase : public HioNode {
public:
    AnimPlayerBase();

    virtual void updateLast() { _10 = false; }

    virtual bool calcNeedUpdateAnimNext() = 0;

    AnimInfoTable* getAnimInfoTable() const { return mInfoTable; }

    void setAnimInfoTable(AnimInfoTable* table) { mInfoTable = table; }

    bool is10() const { return _10; };

    void set10(bool value) { _10 = value; }

    bool isAnimationPlaying() const { return mIsAnimationPlaying; };

    void stopAnimation() { mIsAnimationPlaying = false; }

    void startAnimation() { mIsAnimationPlaying = true; }

private:
    AnimInfoTable* mInfoTable = nullptr;
    bool _10 = false;
    bool mIsAnimationPlaying = false;
};

static_assert(sizeof(AnimPlayerBase) == 0x18);

}  // namespace al

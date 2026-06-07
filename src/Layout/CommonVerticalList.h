#pragma once

#include <math/seadVector.h>
#include <prim/seadSafeString.h>

#include "Library/Nerve/NerveExecutor.h"

namespace nn::ui2d {
class TextureInfo;
}  // namespace nn::ui2d

namespace al {
class LayoutActor;
class LayoutInitInfo;
class RollParts;
}  // namespace al

struct RollPartsData {
    char filler[0x18];
};

class CommonVerticalList : public al::NerveExecutor {
public:
    CommonVerticalList(al::LayoutActor*, const al::LayoutInitInfo&, bool);

    void initData(s32);
    void initDataNoResetSelected(s32);
    void initDataWithIdx(s32, s32, s32);
    void hideAll();
    void updateCursorPos();
    void addStringData(const sead::WFixedSafeString<512>*, const char*);
    void setEnableData(const bool*);
    void addGroupAnimData(const sead::FixedSafeString<64>*, const char*);
    void setImageData(nn::ui2d::TextureInfo**, const char*);
    void setSelectedIdx(s32, s32);
    void setRollPartsData(RollPartsData*);
    void setRollPartsSelected(s32, s32);
    s32 getRollPartsSelected(s32);
    al::RollParts* getSelectedParts() const;
    al::RollParts* getParts(s32) const;
    s32 getListPartsNum() const;
    void startLoopActionAll(const char*, const char*);
    void calcCursorPos(sead::Vector2f*) const;
    bool isActive() const;
    bool isDeactive() const;
    bool isDecideEnd() const;
    bool isRejectEnd() const;
    void update();
    void up();
    void down();
    void decide();
    void updateParts();
    void reject();
    void deactivate();
    void activate();
    void jumpTop();
    void jumpBottom();
    void pageUp();
    void pageDown();
    void rollRight();
    void rollLeft();
    void exeActive();
    void exeDeactive();
    void exeDecide();
    void exeDecideEnd();
    void exeReject();
    void exeRejectEnd();
    void calcAnimRate();
    void appearCursor();
    void hideCursor();
    void endCursor();

    s32 getSelectedId() const { return mSelectedId; }

    void set_c8(f32 value) { _c8 = value; }

    void set_cc(bool value) { _cc = value; }

    s32 getFirstId() const { return mFirstId; }

    s32 getLastId() const { return mLastId; }

private:
    char filler[0x2c];
    s32 mSelectedId;
    char filler2s[0x28];
    s32 mFirstId;
    char filler2[0x30];
    s32 mLastId;
    char filler44[0x2c];
    bool _cc;
    char filler3[0xc];
    f32 _c8;
};

static_assert(sizeof(CommonVerticalList) >= 0xd0);

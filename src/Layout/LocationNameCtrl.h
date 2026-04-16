#pragma once

#include <basis/seadTypes.h>

#include "Library/Area/IUseAreaObj.h"
#include "Library/Nerve/NerveStateBase.h"

namespace al {
class AreaObjDirector;
class LayoutInitInfo;
class PlayerHolder;
}  // namespace al

class GameDataHolder;

class LocationNameCtrl : public al::NerveStateBase, public al::IUseAreaObj {
public:
    LocationNameCtrl(al::AreaObjDirector*, GameDataHolder*, const al::LayoutInitInfo&,
                     const al::PlayerHolder*);
    ~LocationNameCtrl() override;
    al::AreaObjDirector* getAreaObjDirector() const override;
    void tryKill();
    void exeHide();
    void exeDelay();
    void exeShow();

private:
    u8 _padding[0x20];
};

static_assert(sizeof(LocationNameCtrl) == 0x40);

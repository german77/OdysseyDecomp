#include "Item/Coin2DCityDirector.h"

#include "Library/Bgm/BgmBeatCounter.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"

#include "Item/Coin2DCity.h"

namespace {
NERVE_IMPL(Coin2DCityDirector, Wait);

NERVES_MAKE_STRUCT(Coin2DCityDirector, Wait);
}  // namespace

Coin2DCityDirector::Coin2DCityDirector(const char* name) : al::LiveActor(name) {}

void Coin2DCityDirector::init(const al::ActorInitInfo& initInfo) {}

void Coin2DCityDirector::initAfterPlacement() {}

void Coin2DCityDirector::control() {
    return mBgmBeatCounter->update();
}

void Coin2DCityDirector::registerCoin(Coin2DCity*) {}

void Coin2DCityDirector::getCoin() {}

bool Coin2DCityDirector::isTriggerBeat() const {
    return mBgmBeatCounter->isAboveThreshold();
}

void Coin2DCityDirector::exeWait() {}

#include "Player/PlayerJudgeForceLand.h"

#include "Player/PlayerTrigger.h"
#include "Util/JudgeUtil.h"

PlayerJudgeForceLand::PlayerJudgeForceLand(const IJudge* judgeLongFall,
                                           const PlayerTrigger* trigger)
    : mJudgeLongFall(judgeLongFall), mTrigger(trigger) {}

bool PlayerJudgeForceLand::judge() const {
    return rs::isJudge(mJudgeLongFall) || mTrigger->isOn(PlayerTrigger::EActionTrigger_val11);
}

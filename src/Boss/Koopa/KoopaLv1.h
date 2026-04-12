#pragma once

#include "Library/LiveActor/LiveActor.h"

namespace al {
struct ActorInitInfo;
class HitSensor;
class SensorMsg;
}  // namespace al

class KoopaLv1 : public al::LiveActor {
public:
    KoopaLv1(const char*);
    void init(const al::ActorInitInfo&) override;
    void initAfterPlacement() override;
    void kill() override;
    void movement() override;
    void calcAnim() override;
    void attackSensor(al::HitSensor* self, al::HitSensor* other) override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;
    void exeDemoStart();
    void exeJump1();
    void exeThrowCap();
    void exeJump2();
    void exeHipDrop();
    void exeThrowBall();
    void exeHitPunch();
    void exeAttackTail();
    void exeRecover();
    void exeFireBeam();
    void exeDeadAndDemoEnd();
    void exeDemoClashBasement();
};

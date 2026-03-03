#pragma once

#include <basis/seadTypes.h>

#include "Library/Nerve/NerveExecutor.h"

class IUsePlayerPuppet;

class MotorcyclePlayerAnimator : public al::NerveExecutor {
public:
    MotorcyclePlayerAnimator();

    void update(f32, f32, f32);
    void startBind(IUsePlayerPuppet*);
    void startBindKeepDemo();
    void endBind();
    void startBindRideStartL();
    void startBindRideStartR();
    void startBindRideOn();
    void startBindWait();
    void startBindRideRunStart();
    void startBindRideClash();
    void startBindRideJump();
    void tryStartBindRideRunIfNotPlaying();
    void tryStartBindRideLandIfJump();

    void exeNone();
    void exeRideStartL();
    void exeRideStartR();
    void exeRideOn();
    void exeWait();
    void exeRideRunStart();
    void exeRideRun();
    void exeRideClash();
    void exeRideJump();
    void exeRideLand();
    void exeBindKeepDemo();

private:
    IUsePlayerPuppet* mPlayerPuppet;
    f32 floatA;
    f32 floatB;
    f32 floatC;
};

static_assert(sizeof(MotorcyclePlayerAnimator) == 0x28);

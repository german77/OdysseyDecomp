#pragma once

#include <basis/seadTypes.h>
#include <container/seadPtrArray.h>
#include <math/seadVector.h>

#include "Library/Nerve/NerveStateBase.h"

namespace al {
class IBgmParamsChanger;
}  // namespace al

class Bubble;

class BubbleStateInLauncher : public al::HostStateBase<Bubble> {
public:
    BubbleStateInLauncher(Bubble* bubble);

    void appear() override;
    void kill() override;

    bool isWaiting() const;
    bool isFlying() const;
    void shoot(const sead::Vector3f& direction, f32 force);
    void calcLaunchPos(sead::Vector3f*, const sead::Vector3f&, f32, s32) const;

    void exeEnter();
    void exeWait();
    void exeShoot();

private:
    sead::Vector3f mShootDirection = {0.0f, 0.0f, 0.0f};
    f32 mShootForce = 0.0f;
    sead::Vector3f mTrans = {0.0f, 0.0f, 0.0f};
    sead::Vector3f mDirection = {0.0f, 0.0f, 0.0f};
    f32 mForce=0.0f;
    f32 mFindMe=0.0f;
    sead::PtrArray<al::IBgmParamsChanger>* mIBgmParamsChanger = nullptr;
};

static_assert(sizeof(BubbleStateInLauncher) == 0x58);

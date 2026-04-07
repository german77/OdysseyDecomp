#pragma once

#include <math/seadQuat.h>

#include "Library/LiveActor/LiveActor.h"

namespace al {
struct ActorInitInfo;
class HitSensor;
class ParameterIo;
class ParameterObj;
class ParameterF32;
class ParameterV3f;
class RippleCtrl;
class SensorMsg;

class RippleFixMapParts : public LiveActor {
public:
    RippleFixMapParts(const char* name);

    void init(const ActorInitInfo& info) override;
    void initAfterPlacement() override;
    void appear() override;
    void control() override;
    void movement() override;
    void calcAnim() override;
    bool receiveMsg(const SensorMsg* message, HitSensor* other, HitSensor* self) override;

private:
    struct Parameters {
        ParameterIo* parameterIo;
        ParameterObj* parameterObj;
        ParameterF32* sinkStartDist;
        ParameterF32* sinkEndDist;
        ParameterV3f* sinkDistance;
    };

    static_assert(sizeof(Parameters) == 0x28);

    RippleCtrl* mRippleCtrl = nullptr;
    bool mIsCalcViewModel = false;
    bool mIsAddRippleByCamera = false;
    Parameters* mParameters = nullptr;
    sead::Vector3f mTrans = {0.0f, 0.0f, 0.0f};
};

static_assert(sizeof(RippleFixMapParts) == 0x130);
}  // namespace al

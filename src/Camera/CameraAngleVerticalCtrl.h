#pragma once

#include <math/seadVector.h>

#include "Library/Nerve/NerveExecutor.h"

namespace al {
class ByamlIter;
}

struct CameraAngleUpdateInfo {
    bool isA = false;
    bool isB = false;
    f32 valA = 23.0f;
    sead::Vector3f vecA = {0.0f, 0.0f, 0.0f};
    sead::Vector3f vecB = {0.0f, 0.0f, 0.0f};
    sead::Vector3f vecC = {0.0f, 0.0f, 0.0f};
    sead::Vector3f vecD = {0.0f, 0.0f, 0.0f};
    sead::Vector2f vecE = {0.0f, 0.0f};
    s32 valB = 0;
    f32 valC = 1.0f;
    bool isC = false;
    bool isD = false;
    bool isE = false;
    sead::Vector3f vecF = {0.0f, 0.0f, 0.0f};
    bool isChaseToSubTarget = false;
    f32 chaseA = 0.0f;
    f32 chaseB = 0.0f;
};

static_assert(sizeof(CameraAngleUpdateInfo) == 0x64);

struct CameraInfo {
    f32 defaultAngle = 23.0;
    f32 minAngle = -35.0f;
    f32 maxAngle = 85.0f;
    f32 waterNeutralAngle = 0.0f;
    bool isSetAngleRangeV = false;
    bool isValidAutoLook = false;
    bool isInvalidHackFlyerCtrl = false;
    bool isInvalidAutoLowAngleReset = false;
};

static_assert(sizeof(CameraInfo) == 0x14);

struct CameraInfo2 {
    bool isValid = false;
    f32 angle = 35.0f;
    f32 range = 85.0f;
};

static_assert(sizeof(CameraInfo2) == 0xc);

struct CameraInfo3 {
    bool isValid = false;
    f32 angle = 35.0f;
    f32 range = 85.0f;
};

static_assert(sizeof(CameraInfo3) == 0xc);

struct CameraInfo4 {
    u32 angle = 0;
    s32 range = -1;
};

static_assert(sizeof(CameraInfo4) == 0x8);

struct CameraInfo5 {
    int nullla=0;
    int nulllb=0;
    int nulllc=0;
    int nullld=0;
    bool whoah=false;
};

static_assert(sizeof(CameraInfo5) == 0x14);

class CameraAngleVerticalCtrl : public al::NerveExecutor {
public:
    CameraAngleVerticalCtrl();

    f32 getInitDefaultAngleDegree();
    void loadParam(const al::ByamlIter&);
    void start(const sead::Vector3f&);
    void startUserCtrl();
    void update(const CameraAngleUpdateInfo&);
    f32 getDefaultAngleDegree() const;
    void setAngleDegree(f32);
    void startTargetInterpole(f32);
    void startTargetInterpoleByStep(f32, s32);
    void startResetInterpole();
    void startResetInterpoleByStep(s32);
    void chaseToTargetDegree(f32);
    void chaseToTargetDegreeBySpeed(f32, f32);
    bool isFixInRange() const;
    void setRailAngleDegreeRangeAndInterp(f32, f32, s32);
    void resetRailAngleDegreeRange();
    void startWaterCtrl(s32);
    void invalidateAutoResetLowAngleV();
    void startSnap(f32);
    void endSnap();
    void exeUserCtrl();
    void exeWaterCtrl();
    void exeHackFlyerCtrl();
    void exeInterp();
    void exeSnapStart();
    void exeSnap();
    void exeSnapEnd();

private:
    CameraAngleUpdateInfo info;
    CameraInfo4* cinfo4;
    CameraInfo5* cinfo5;
    CameraInfo* cinfo;
    CameraInfo3* cinfo3;
    sead::Vector3f vecccer= {0.0f, 0.0f, 0.0f};
    sead::Vector3f theMisteriousOne = sead::Vector3f::ey;
    s32 steper = 0;
    s32 nadamant = 0;
    s32 sip = -1;
    bool booal = false;
    bool dude = false;
    f32 like = 23.0f;
    f32 lerp = 23.0f;
    f32 unliked = 0.0f;
    f32 vecSnap = 0.0f;
    s32 later = 0;
    bool noal = false;
    f32 yeah = 0.0f;
    s32 nidint = 0;
};

// static_assert(sizeof(CameraAngleVerticalCtrl) == 0xe0);

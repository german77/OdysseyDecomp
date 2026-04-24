#include "Camera/CameraAngleVerticalCtrl.h"

#include "Library/Math/MathUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Yaml/ByamlIter.h"
#include "Library/Yaml/ByamlUtil.h"

namespace {
NERVE_IMPL(CameraAngleVerticalCtrl, UserCtrl)
NERVE_IMPL(CameraAngleVerticalCtrl, WaterCtrl)
NERVE_IMPL(CameraAngleVerticalCtrl, HackFlyerCtrl)
NERVE_IMPL(CameraAngleVerticalCtrl, Interp)
NERVE_IMPL_(CameraAngleVerticalCtrl, InterpWakeUp, Interp)
NERVE_IMPL(CameraAngleVerticalCtrl, SnapStart)
NERVE_IMPL(CameraAngleVerticalCtrl, Snap)
NERVE_IMPL(CameraAngleVerticalCtrl, SnapEnd)

NERVES_MAKE_STRUCT(CameraAngleVerticalCtrl, UserCtrl, WaterCtrl, HackFlyerCtrl, Interp, SnapStart,
                   Snap, SnapEnd, InterpWakeUp)
}  // namespace

inline al::Nerve* getNerveCtrl(CameraInfo4* cinfo4) {
    if (cinfo4->range == 0)
        return &NrvCameraAngleVerticalCtrl.HackFlyerCtrl;

    if (cinfo4->angle == 1)
        return &NrvCameraAngleVerticalCtrl.WaterCtrl;

    if (cinfo4->angle == 0)
        return &NrvCameraAngleVerticalCtrl.UserCtrl;

    return nullptr;
}

inline f32 getValueInRange(CameraInfo* cinfos, CameraInfo3* cinfo3s, f32 angle) {
    f32 rangeMin = -35.0f;
    if (cinfos->isSetAngleRangeV)
        rangeMin = sead::Mathf::clampMin(rangeMin, cinfos->minAngle);
    if (cinfo3s->isValid)
        rangeMin = sead::Mathf::clampMin(rangeMin, cinfo3s->angle);
    rangeMin = sead::Mathf::clampMin(rangeMin, -87.5f);

    f32 rangeMax = 85.0f;
    if (cinfos->isSetAngleRangeV)
        rangeMax = sead::Mathf::clampMax(rangeMax, cinfos->maxAngle);
    if (cinfo3s->isValid)
        rangeMax = sead::Mathf::clampMax(rangeMax, cinfo3s->range);
    rangeMax = sead::Mathf::clampMax(rangeMax, 87.5f);

    return sead::Mathf::clamp(angle, rangeMin, rangeMax);
}

CameraAngleVerticalCtrl::CameraAngleVerticalCtrl() : al::NerveExecutor("カメラ垂直角度操作") {
    cinfo4 = new CameraInfo4;
    cinfo5 = new CameraInfo5;
    cinfo = new CameraInfo;
    cinfo3 = new CameraInfo3;

    al::NerveExecutor::initNerve(getNerveCtrl(cinfo4), 0);
}

f32 CameraAngleVerticalCtrl::getInitDefaultAngleDegree() {
    return 23.0f;
}

void CameraAngleVerticalCtrl::loadParam(const al::ByamlIter& iter) {
    CameraInfo* infos = cinfo;
    al::tryGetByamlF32(&infos->defaultAngle, iter, "DefaultAngle");
    if (al::tryGetByamlBool(&infos->isSetAngleRangeV, iter, "IsSetAngleRangeV") &&
        infos->isSetAngleRangeV) {
        al::tryGetByamlF32(&infos->minAngle, iter, "MinAngle");
        al::tryGetByamlF32(&infos->maxAngle, iter, "MaxAngle");
    }
    al::tryGetByamlF32(&infos->waterNeutralAngle, iter, "WaterNeutralAngle");
    al::tryGetByamlBool(&infos->isValidAutoLook, iter, "IsValidAutoLook");
    al::tryGetByamlBool(&infos->isInvalidHackFlyerCtrl, iter, "IsInvalidHackFlyerCtrl");
    al::tryGetByamlBool(&infos->isInvalidAutoLowAngleReset, iter, "IsInvalidAutoLowAngleReset");
}

void CameraAngleVerticalCtrl::start(const sead::Vector3f& pos) {
    later = 0;
    booal = false;
    vecccer.set(pos);
    startUserCtrl();

    al::setNerve(this, getNerveCtrl(cinfo4));
}

void CameraAngleVerticalCtrl::startUserCtrl() {
    cinfo4->angle = 0;
}

void CameraAngleVerticalCtrl::update(const CameraAngleUpdateInfo& updateInfo) {
    info = updateInfo;

    updateNerve();
    vecccer.set(updateInfo.vecC);
}

f32 CameraAngleVerticalCtrl::getDefaultAngleDegree() const {
    if (info.isD)
        return 15.0f;
    return cinfo->defaultAngle;
}

void CameraAngleVerticalCtrl::setAngleDegree(f32 angle) {
    f32 result = lerp = getValueInRange(cinfo, cinfo3, angle);
    ;

    like = result;
    lerp = result;
}

void CameraAngleVerticalCtrl::startTargetInterpole(f32 interpole) {
    f32 fVar3 = sead::Mathf::abs(interpole - like);
    f32 fVar4 = info.valC * 0.5f;

    if (0.0f >= fVar4 || fVar3 < fVar4)
        return;

    startTargetInterpoleByStep(interpole, sead::Mathi::clampMin(fVar3 / fVar4, 25));
}

void CameraAngleVerticalCtrl::startTargetInterpoleByStep(f32 interpole, s32 step) {
    if (step < 1) {
        setAngleDegree(interpole);
        if (al::isNerve(this, &NrvCameraAngleVerticalCtrl.Interp))
            al::setNerve(this, getNerveCtrl(cinfo4));
        return;
    }

    unliked = like;
    lerp = getValueInRange(cinfo, cinfo3, interpole);
    steper = step;
    al::setNerve(this, &NrvCameraAngleVerticalCtrl.Interp);
}

void CameraAngleVerticalCtrl::startResetInterpole() {
    booal = false;
    startTargetInterpole(info.valA);
}

void CameraAngleVerticalCtrl::startResetInterpoleByStep(s32 step) {
    booal = false;
    startTargetInterpoleByStep(info.valA, step);
}

void CameraAngleVerticalCtrl::chaseToTargetDegree(f32 angle) {
    f32 result = getValueInRange(cinfo, cinfo3, angle);

    f32 fVar4 = al::lerpValue(lerp, result, 0.7f);
    lerp = al::lerpValue(lerp, fVar4, 0.1f);
}

void CameraAngleVerticalCtrl::chaseToTargetDegreeBySpeed(f32 min, f32 max) {
    if (al::isNerve(this, &NrvCameraAngleVerticalCtrl.UserCtrl))
        chaseToTargetDegree(al::converge(lerp, min, max));
}

bool CameraAngleVerticalCtrl::isFixInRange() const {
    bool isValid = al::isNerve(this, &NrvCameraAngleVerticalCtrl.WaterCtrl);

    // return al::isNear(fVar3,fVar1);
}

void CameraAngleVerticalCtrl::setRailAngleDegreeRangeAndInterp(f32, f32, s32) {}

void CameraAngleVerticalCtrl::resetRailAngleDegreeRange() {
    cinfo3->isValid = false;
}

void CameraAngleVerticalCtrl::startWaterCtrl(s32 ctrl) {
    cinfo4->angle = 1;
    nadamant = ctrl;
}

void CameraAngleVerticalCtrl::invalidateAutoResetLowAngleV() {
    cinfo->isInvalidAutoLowAngleReset = true;
}

void CameraAngleVerticalCtrl::startSnap(f32 snap) {
    vecSnap = snap;
    al::setNerve(this, &NrvCameraAngleVerticalCtrl.SnapStart);
}

void CameraAngleVerticalCtrl::endSnap() {
    if (al::isNerve(this, &NrvCameraAngleVerticalCtrl.SnapStart)) {
        al::setNerve(this, &NrvCameraAngleVerticalCtrl.UserCtrl);
        return;
    }

    if (al::isNerve(this, &NrvCameraAngleVerticalCtrl.Snap))
        al::setNerve(this, &NrvCameraAngleVerticalCtrl.SnapEnd);
}

void CameraAngleVerticalCtrl::exeUserCtrl() {}

void CameraAngleVerticalCtrl::exeWaterCtrl() {}

void CameraAngleVerticalCtrl::exeHackFlyerCtrl() {}

void CameraAngleVerticalCtrl::exeInterp() {}

void CameraAngleVerticalCtrl::exeSnapStart() {}

void CameraAngleVerticalCtrl::exeSnap() {
    like = al::lerpValue(like, lerp, 0.1f);

    if (-0.15f <= info.vecE.y && info.vecE.y <= 0.15f)
        return;

    if (cinfo4->range == 0) {
        al::setNerve(this, &NrvCameraAngleVerticalCtrl.HackFlyerCtrl);
        return;
    }
    if (cinfo4->angle == 0) {
        al::setNerve(this, &NrvCameraAngleVerticalCtrl.UserCtrl);
        return;
    }

    al::setNerve(this, cinfo4->angle == 1 ? &NrvCameraAngleVerticalCtrl.WaterCtrl : nullptr);
}

void CameraAngleVerticalCtrl::exeSnapEnd() {}

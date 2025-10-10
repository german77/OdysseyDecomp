#include "Library/Clipping/FrustumRadar.h"

#include <math/seadMathCalcCommon.h>

#include "Library/Math/MathUtil.h"
#include "Library/Matrix/MatrixUtil.h"

namespace al {

FrustumRadar::FrustumRadar() = default;

void FrustumRadar::calcFrustumArea(const sead::Matrix34f& orthoMtx, f32 angle, f32 f2, f32 areaMin,
                                   f32 areaMax) {
    setLocalAxis(orthoMtx);

    setFactor(angle, f2);

    mAreaMin = areaMin;
    mAreaMax = areaMax;
}

void FrustumRadar::setLocalAxis(const sead::Matrix34f& orthoMtx) {
    sead::Matrix34f mtxInvertOrtho;
    calcMxtInvertOrtho(&mtxInvertOrtho, orthoMtx);

    mOrthoSide.x = mtxInvertOrtho.m[0][0];
    mOrthoSide.y = mtxInvertOrtho.m[1][0];
    mOrthoSide.z = mtxInvertOrtho.m[2][0];
    mOrthoUp.x = mtxInvertOrtho.m[0][1];
    mOrthoUp.y = mtxInvertOrtho.m[1][1];
    mOrthoUp.z = mtxInvertOrtho.m[2][1];
    const sead::Vector3f& base2 = mtxInvertOrtho.getBase(2);
    mOrthoFront.x = -base2.x;
    mOrthoFront.y = -base2.y;
    mOrthoFront.z = -base2.z;
    mOrthoTrans.x = mtxInvertOrtho.m[0][3];
    mOrthoTrans.y = mtxInvertOrtho.m[1][3];
    mOrthoTrans.z = mtxInvertOrtho.m[2][3];
    _40 = 0.0f;
}

void FrustumRadar::setFactor(f32 angle, f32 f2) {
    _38 = sead::Mathf::tan(sead::Mathf::deg2rad(angle * 0.5f));
    _3c = sead::Mathf::sqrt(_38 * _38 + 1.0f);

    _30 = _38 * f2;
    _34 = sead::Mathf::sqrt(_30 * _30 + 1.0f);
}

void FrustumRadar::calcFrustumArea(const sead::Matrix34f& orthoMtx,
                                   const sead::Matrix44f& factorMtx, f32 areaMin, f32 areaMax) {
    setLocalAxis(orthoMtx);
    setFactor(factorMtx);
    mAreaMin = areaMin;
    mAreaMax = areaMax;
}

void FrustumRadar::setFactor(const sead::Matrix44f& mtx) {
    _38 = 1.0f / mtx(1, 1);
    _3c = sead::Mathf::sqrt(_38 * _38 + 1.0f);

    _30 = 1.0f / mtx(0, 0);
    _34 = sead::Mathf::sqrt(_30 * _30 + 1.0f);
}

void FrustumRadar::calcFrustumAreaStereo(const sead::Matrix34f& orthoMtxLeft,
                                         const sead::Matrix34f& OrthoMtxRight,
                                         const sead::Matrix44f& mtx, f32 areaMin, f32 areaMax) {
    setLocalAxisStereo(orthoMtxLeft, OrthoMtxRight);
    setFactorStereo(mtx);
    mAreaMin = areaMin;
    mAreaMax = areaMax;
}

void FrustumRadar::setLocalAxisStereo(const sead::Matrix34f& orthoMtxLeft,
                                      const sead::Matrix34f& orthoMtxRight) {
    sead::Matrix34f mtxInvertOrthoLeft;
    sead::Matrix34f mtxInvertRight;

    calcMxtInvertOrtho(&mtxInvertOrthoLeft, orthoMtxLeft);

    mOrthoSide.x = mtxInvertOrthoLeft.m[0][0];
    mOrthoSide.y = mtxInvertOrthoLeft.m[1][0];
    mOrthoSide.z = mtxInvertOrthoLeft.m[2][0];
    mOrthoUp.x = mtxInvertOrthoLeft.m[0][1];
    mOrthoUp.y = mtxInvertOrthoLeft.m[1][1];
    mOrthoUp.z = mtxInvertOrthoLeft.m[2][1];
    const sead::Vector3f& base2 = mtxInvertOrthoLeft.getBase(2);
    mOrthoFront.x = -base2.x;
    mOrthoFront.y = -base2.y;
    mOrthoFront.z = -base2.z;

    calcMxtInvertOrtho(&mtxInvertRight, orthoMtxRight);
    mOrthoTrans = (mtxInvertOrthoLeft.getTranslation() + mtxInvertRight.getTranslation()) * 0.5f;

    _40 = mOrthoSide.dot(mOrthoTrans - mtxInvertOrthoLeft.getTranslation());
}

void FrustumRadar::setFactorStereo(const sead::Matrix44f& mtx) {
    setFactor(mtx);
    f32 f2 = mtx(0, 2);

    _44 = _30 * (1.0f - f2);
    _48 = sead::Mathf::sqrt(_44 * _44 + 1.0f);
    _4c = _30 * (f2 + 1.0f);
    _50 = sead::Mathf::sqrt(_4c * _4c + 1.0f);
}

bool FrustumRadar::judgeInLeft(const sead::Vector3f& pos, f32 f) const {
    f32 dot1 = mOrthoFront.dot(pos - mOrthoTrans);
    f32 dot2 = mOrthoSide.dot(pos - mOrthoTrans);

    return !(dot2 < -(dot1 * _30 + _34 * f));
}

bool FrustumRadar::judgeInRight(const sead::Vector3f& pos, f32 f) const {
    f32 dot1 = mOrthoFront.dot(pos - mOrthoTrans);
    f32 dot2 = mOrthoSide.dot(pos - mOrthoTrans);

    return !(dot1 * _30 + _34 * f < dot2);
}

bool FrustumRadar::judgeInTop(const sead::Vector3f& pos, f32 f) const {
    f32 dot1 = mOrthoFront.dot(pos - mOrthoTrans);
    f32 dot2 = mOrthoUp.dot(pos - mOrthoTrans);

    return !(dot1 * _38 + _3c * f < dot2);
}

bool FrustumRadar::judgeInBottom(const sead::Vector3f& pos, f32 f) const {
    f32 dot1 = mOrthoFront.dot(pos - mOrthoTrans);
    f32 dot2 = mOrthoUp.dot(pos - mOrthoTrans);

    return !(-(dot1 * _38 + _3c * f) > dot2);
}

bool FrustumRadar::judgeInArea(const sead::Vector3f& pos, f32 f1, f32 areaMin, f32 areaMax) const {
    f32 dot = mOrthoFront.dot(pos - mOrthoTrans);
    if (dot < areaMin - f1)
        return false;

    if (0.0f < areaMax && f1 + areaMax < dot)
        return false;

    f32 f4 = sead::Mathf::abs(mOrthoUp.dot(pos - mOrthoTrans));
    if (dot * _38 + _3c * f1 < f4)
        return false;

    f32 fVar2 = mOrthoSide.dot(pos - mOrthoTrans);
    if (isNearZero(_40)) {
        if (dot * _30 + _34 * f1 < sead::Mathf::abs(fVar2))
            return false;
    } else {
        f32 fVar3 = dot * _4c + _50 * f1;
        dot = dot * _44 + _48 * f1;
        f32 fVar4 = fVar2 - _40;
        fVar2 = fVar2 + _40;

        if (fVar4 > dot && fVar2 > fVar3)
            return false;

        if (fVar4 < -fVar3 && fVar2 < -dot)
            return false;
    }
    return true;
}

bool FrustumRadar::judgeInArea(const sead::Vector3f& pos, f32 f1, f32 areaMin) const {
    return judgeInArea(pos, f1, areaMin, mAreaMax);
}

bool FrustumRadar::judgeInArea(const sead::Vector3f& pos, f32 f1) const {
    return judgeInArea(pos, f1, mAreaMin, mAreaMax);
}

bool FrustumRadar::judgeInAreaNoFar(const sead::Vector3f& pos, f32 f1) const {
    return judgeInArea(pos, f1, mAreaMin, -1.0f);
}

// https://decomp.me/scratch/qnRZM
PointFlag FrustumRadar::judgePointFlag(const sead::Vector3f& pos, f32 areaMin, f32 areaMax) const {
    sead::Vector3f relPos = pos - mOrthoTrans;
    f32 dotFront = mOrthoFront.dot(relPos);

    u32 flag = (dotFront < areaMin) ? PointFlag::Dot : PointFlag::None;
    if (areaMax > 0.0f && dotFront > areaMax)
        flag |= PointFlag::Go;

    f32 dotUp = mOrthoUp.dot(relPos);
    if (dotUp < -(dotFront * _38))
        flag |= PointFlag::Little;

    if (dotFront * _38 < dotUp)
        flag |= PointFlag::Rocket;

    f32 dotSide = mOrthoSide.dot(relPos);
    if (isNearZero(_40)) {
        if (dotSide < -(dotFront * _30))
            flag |= PointFlag::Nain;
        if (dotFront * _30 < dotSide)
            flag |= PointFlag::Main;
        return (PointFlag)flag;
    }

    f32 m40 = _40;
    f32 m44 = _44;
    f32 m4c = _4c;

    f32 v44 = dotFront * m44;
    f32 v4c = dotFront * m4c;
    f32 sideMinus = dotSide - m40;
    f32 sidePlus = dotSide + m40;

    if (sidePlus > v4c && sideMinus > v44)
        flag |= PointFlag::Main;

    if (-v44 > sidePlus && -v4c > sideMinus)
        flag |= PointFlag::Nain;

    return (PointFlag)flag;
}

bool FrustumRadar::judgeInAreaObb(const sead::Matrix34f* mtx, const sead::BoundBox3f& boundBox,
                                  f32 areaMin, f32 areaMax) const {
    sead::Vector3f corners[8];
    calcObbCorners(corners, *mtx, boundBox);

    s32 combinedFlags = PointFlag::Invalid;
    for (int i = 0; i < 8; ++i) {
        s32 pointFlags = judgePointFlag(corners[i], areaMin, areaMax);
        if (pointFlags == PointFlag::None)
            return true;

        combinedFlags &= pointFlags;
    }

    return combinedFlags == PointFlag::None;
}

bool FrustumRadar::judgeInAreaObb(const sead::Matrix34f* mtx, const sead::BoundBox3f& boundBox,
                                  f32 areaMin) const {
    return judgeInAreaObb(mtx, boundBox, areaMin, mAreaMax);
}

bool FrustumRadar::judgeInAreaObb(const sead::Matrix34f* mtx,
                                  const sead::BoundBox3f& boundBox) const {
    return judgeInAreaObb(mtx, boundBox, mAreaMin, mAreaMax);
}

bool FrustumRadar::judgeInAreaObbNoFar(const sead::Matrix34f* mtx,
                                       const sead::BoundBox3f& boundBox) const {
    return judgeInAreaObb(mtx, boundBox, mAreaMin, -1.0f);
}

}  // namespace al

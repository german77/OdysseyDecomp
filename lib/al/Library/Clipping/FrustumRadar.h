#pragma once

#include <math/seadBoundBox.h>
#include <math/seadMatrix.h>

#include "Library/Matrix/MatrixUtil.h"

namespace al {

enum PointFlag : s32 {
    Invalid = -1,
    None = 0,
    Dot = 1,
    Go = 2,
    Little = 4,
    Rocket = 8,
    Nain = 16,
    Main = 32,
};

class FrustumRadar {
public:
    FrustumRadar();
    void calcFrustumArea(const sead::Matrix34f&, f32, f32, f32, f32);
    void setLocalAxis(const sead::Matrix34f&);
    void setFactor(f32, f32);
    void calcFrustumArea(const sead::Matrix34f&, const sead::Matrix44f&, f32, f32);
    void setFactor(const sead::Matrix44f&);
    void calcFrustumAreaStereo(const sead::Matrix34f&, const sead::Matrix34f&,
                               const sead::Matrix44f&, f32, f32);
    void setLocalAxisStereo(const sead::Matrix34f&, const sead::Matrix34f&);
    void setFactorStereo(const sead::Matrix44f&);
    bool judgeInLeft(const sead::Vector3f&, f32) const;
    bool judgeInRight(const sead::Vector3f&, f32) const;
    bool judgeInTop(const sead::Vector3f&, f32) const;
    bool judgeInBottom(const sead::Vector3f&, f32) const;
    bool judgeInArea(const sead::Vector3f&, f32, f32, f32) const;
    bool judgeInArea(const sead::Vector3f&, f32, f32) const;
    bool judgeInArea(const sead::Vector3f&, f32) const;
    bool judgeInAreaNoFar(const sead::Vector3f&, f32) const;
    PointFlag judgePointFlag(const sead::Vector3f&, f32 areaMin, f32 areaMax) const;
    bool judgeInAreaObb(const sead::Matrix34f*, const sead::BoundBox3f& boundBox, f32 areaMin,
                        f32 areaMax) const;
    bool judgeInAreaObb(const sead::Matrix34f* mtx, const sead::BoundBox3f& boundBox,
                        f32 areaMin) const;
    bool judgeInAreaObb(const sead::Matrix34f* mtx, const sead::BoundBox3f& boundBox) const;
    bool judgeInAreaObbNoFar(const sead::Matrix34f* mtx, const sead::BoundBox3f& boundBox) const;

private:
    sead::Vector3f mOrthoSide = sead::Vector3f::ex;
    sead::Vector3f mOrthoUp = sead::Vector3f::ey;
    sead::Vector3f mOrthoFront = sead::Vector3f::ez;
    sead::Vector3f mOrthoTrans = sead::Vector3f::zero;
    f32 _30 = 0.3f;
    f32 _34 = 1.04403f;
    f32 _38 = 0.2f;
    f32 _3c = 1.0098f;
    f32 _40 = 0.0f;
    f32 _44 = 0.3f;
    f32 _48 = 1.04403f;
    f32 _4c = 0.3f;
    f32 _50 = 1.04403f;
    f32 mAreaMin = 100.0f;
    f32 mAreaMax = 10000.0f;
};

}  // namespace al

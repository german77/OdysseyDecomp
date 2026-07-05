#include "Library/Play/Camera/CameraPoserTalk.h"

#include "Library/Camera/CameraPoserFunction.h"
#include "Library/Math/MathUtil.h"
#include "Library/Yaml/ByamlUtil.h"

namespace al {

CameraPoserTalk::CameraPoserTalk(const char* name) : CameraPoser(name) {
    param = new CameraPoserTalkParam();
}

void CameraPoserTalk::init() {
    alCameraPoserFunction::initAngleSwing(this);
    alCameraPoserFunction::invalidateKeepDistanceNextCameraIfNoCollide(this);
    alCameraPoserFunction::validateCollider(this);
}

void CameraPoserTalk::loadParam(const ByamlIter& iter) {
    CameraPoserTalkParam* paramer = param;
    tryGetByamlF32(&paramer->angleH, iter, "AngleH");
    tryGetByamlF32(&paramer->angleV, iter, "AngleV");
    tryGetByamlF32(&paramer->minDistance, iter, "MinDistance");
    tryGetByamlF32(&paramer->offsetY, iter, "OffsetY");
    tryGetByamlBool(&paramer->isKeepPreDir, iter, "IsKeepPreDir");
}

// TODO: probably a sead function?
inline f32 normalize2(sead::Vector3f* v, f32 scalar) {
    const f32 len = v->length();
    if (len > 0) {
        const f32 inv_len = scalar / len;
        v->x *= inv_len;
        v->y *= inv_len;
        v->z *= inv_len;
    }

    return len;
}

void CameraPoserTalk::start(const CameraStartInfo& info) {
    CameraPoserTalkParam* paramer = param;

    sead::Vector3f targetTrans = {0.0f, 0.0f, 0.0f};
    alCameraPoserFunction::calcTargetTrans(&targetTrans, this);
    targetTrans.y += paramer->offsetY;

    sead::Vector3f subTargetTrans = {0.0f, 0.0f, 0.0f};
    alCameraPoserFunction::calcSubTargetTrans(&subTargetTrans, this);
    sead::Vector3f dir = subTargetTrans - targetTrans;

    sead::Vector3f dirH = {0.0f, 0.0f, 0.0f};
    sead::Vector3f dirV = {0.0f, 0.0f, 0.0f};
    parallelizeVec(&dirH, getUp(), dir);
    dir.add(-dirH);
    sead::Vector3f groundDir = -dir;

    if (tryNormalizeOrZero(&dirV, groundDir)) {
        sead::Vector3f preDirH = {0.0f, 0.0f, 0.0f};
        alCameraPoserFunction::calcPreCameraDirH(&preDirH, this);

        f32 ratio = getFovyDegree() * alCameraPoserFunction::getAspect(this) * 0.5f * 0.8f;
        f32 angleH;
        if (paramer->isKeepPreDir)
            angleH = sead::Mathf::clampMin(calcAngleDegree(dirV, preDirH), 30.0f);
        else
            angleH = paramer->angleH;

        f32 cosAngle = 180.0f - (ratio + angleH);
        f32 sinAngle = 180 - (ratio + ratio + cosAngle);
        if (!(cosAngle <= 0.0f || sinAngle <= 0.0f)) {
            f32 cool = cosAngle * sead::Mathf::deg2rad(2.0f);
            f32 woah = sinAngle * sead::Mathf::deg2rad(2.0f);
            if (!isNearZero(sead::Mathf::sin(cool)) && !isNearZero(sead::Mathf::sin(woah))) {
                f32 yay = sead::Mathf::clamp(sead::Mathf::sin(woah) /
                                                 (sead::Mathf::sin(cool) + sead::Mathf::sin(woah)),
                                             0.0f, 1.0f);
                f32 len = dir.length();
                normalize2(&dir, yay * len);

                setAt(dir + targetTrans + yay * dirH);

                f32 coscos = sead::Mathf::sin(cosAngle);
                f32 sinsin = sead::Mathf::sin(sead::Mathf::deg2rad(ratio));

                sead::Vector3f direH = dirV;
                sead::Vector3f direV = dirV;
                f32 neat = yay * len * (coscos / sinsin);

                rotateVectorDegreeY(&direH, angleH);
                rotateVectorDegreeY(&direV, -angleH);

                f32 changeH = calcAngleDegree(groundDir, direH);
                f32 changeV = calcAngleDegree(groundDir, direV);

                sead::Vector3f best;
                if (changeV < changeH)
                    best = direH;
                else
                    best = direV;

                f32 sV = sinf(sead::Mathf::deg2rad(param->angleV));
                sead::Vector3f alt = sV * getUp();
                f32 dist =
                    sead::Mathf::clamp(neat, param->minDistance,
                                       alCameraPoserFunction::getSubTargetRequestDistance(this));

                f32 cV = cosf(sead::Mathf::deg2rad(param->angleV));

                if (!alCameraPoserFunction::checkFirstCameraCollisionArrow(
                        nullptr, nullptr, this, getAt(), dist * (alt + cV * best))) {
                    f32 c2v = cosf(sead::Mathf::deg2rad(param->angleV));
                    setEye(getAt() + dist * (alt + c2v * best));
                    return;
                }

                if (changeV < changeH)
                    best = direV;
                else
                    best = direH;
                f32 cV2 = cosf(sead::Mathf::deg2rad(param->angleV));
                if (!alCameraPoserFunction::checkFirstCameraCollisionArrow(
                        nullptr, nullptr, this, getAt(), dist * (alt + cV2 * best))) {
                    f32 c2v = cosf(sead::Mathf::deg2rad(param->angleV));
                    setEye(getAt() + dist * (alt + c2v * best));
                    return;
                }
            }
        }
    }
    setEye(alCameraPoserFunction::getPreCameraPos(this));
    setAt(alCameraPoserFunction::getPreLookAtPos(this));
    setCameraUp(alCameraPoserFunction::getPreUpDir(this));
}

void CameraPoserTalk::setMinDistance(f32 minDistance) {
    param->minDistance = minDistance;
}
}  // namespace al

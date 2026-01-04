#include "Library/Play/Camera/CameraVerticalAbsorber.h"

#include <gfx/seadViewport.h>

#include "Library/Camera/CameraPoser.h"
#include "Library/Camera/CameraPoserFunction.h"
#include "Library/Camera/CameraStartInfo.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Screen/ScreenFunction.h"
#include "Library/Yaml/ByamlUtil.h"

namespace {
using namespace al;
NERVE_IMPL(CameraVerticalAbsorber, FollowGround);
NERVE_IMPL(CameraVerticalAbsorber, FollowAbsolute);
NERVE_IMPL_(CameraVerticalAbsorber, FollowClimbPoleNoInterp, FollowClimbPole);
NERVE_IMPL_(CameraVerticalAbsorber, FollowSlow, Follow);
NERVE_IMPL(CameraVerticalAbsorber, Absorb);
NERVE_IMPL(CameraVerticalAbsorber, Follow);
NERVE_IMPL(CameraVerticalAbsorber, FollowClimbPole);

NERVES_MAKE_STRUCT(CameraVerticalAbsorber, FollowGround, FollowAbsolute, FollowClimbPoleNoInterp,
                   FollowSlow, Absorb, Follow, FollowClimbPole);
}  // namespace

namespace al {
CameraVerticalAbsorber::CameraVerticalAbsorber(const CameraPoser* cameraParent,
                                               bool isCameraPosAbsorb)
    : NerveExecutor("カメラの縦パン"), mCameraPoser(cameraParent),
      mIsNoCameraPosAbsorb(isCameraPosAbsorb) {
    initNerve(&NrvCameraVerticalAbsorber.FollowGround);
}

void CameraVerticalAbsorber::load(const ByamlIter& iter) {
    ByamlIter verticalAbsorbIter;
    if (!iter.tryGetIterByKey(&verticalAbsorbIter, "VerticalAbsorb"))
        return;

    tryGetByamlF32(&mAbsorbScreenPosUp, verticalAbsorbIter, "AbsorbScreenPosUp");
    tryGetByamlF32(&mAbsorbScreenPosDown, verticalAbsorbIter, "AbsorbScreenPosDown");
    tryGetByamlF32(&mHighJumpJudgeSpeedV, verticalAbsorbIter, "HighJumpJudgeSpeedV");

    ByamlIter advanceAbsorbUpIter;
    if (verticalAbsorbIter.tryGetIterByKey(&advanceAbsorbUpIter, "AdvanceAbsorbUp")) {
        mIsAdvanceAbsorbUp = true;
        mAdvanceAbsorbScreenPosUp =
            getByamlKeyFloat(advanceAbsorbUpIter, "AdvanceAbsorbScreenPosUp");
    }
}

void CameraVerticalAbsorber::start(const sead::Vector3f& pos, const CameraStartInfo& info) {
    alCameraPoserFunction::calcTargetFront(&mPrevTargetFront, mCameraPoser);

    mAbsorbVec= {0.0f, 0.0f, 0.0f};
    mPrevTargetTrans.set(pos);

    if (!isValid() || alCameraPoserFunction::isPlayerTypeNotTouchGround(mCameraPoser))
        return setNerve(this, &NrvCameraVerticalAbsorber.FollowAbsolute);

    if (alCameraPoserFunction::isTargetClimbPole(mCameraPoser))
        return setNerve(this, &NrvCameraVerticalAbsorber.FollowClimbPoleNoInterp);

    if (alCameraPoserFunction::isTargetGrabCeil(mCameraPoser))
        return setNerve(this, &NrvCameraVerticalAbsorber.FollowSlow);

    if (!info.isGrounded || alCameraPoserFunction::isTargetCollideGround(mCameraPoser))
        return setNerve(this, &NrvCameraVerticalAbsorber.FollowGround);

    mPrevTargetTrans.set(alCameraPoserFunction::getPreLookAtPos(mCameraPoser));

    const CameraPoser* poser = mCameraPoser;
    sead::Vector3f gravity = {0.0f, 0.0f, 0.0f};
    alCameraPoserFunction::calcTargetGravity(&gravity, poser);

    mAbsorbVec= pos - mPrevTargetTrans;
    parallelizeVec(&mAbsorbVec, gravity, mAbsorbVec);
    setNerve(this, &NrvCameraVerticalAbsorber.Absorb);
}

bool CameraVerticalAbsorber::isValid() const {
    return !_1aa && !mIsInvalidated;
}

// NON_MATCHING: https://decomp.me/scratch/xNT3w
void CameraVerticalAbsorber::update() {
    if (mIsStopUpdate)
        return;
    const CameraPoser* poser = mCameraPoser;
    sead::Vector3f gravity = {0.0f, 0.0f, 0.0f};
    alCameraPoserFunction::calcTargetGravity(&gravity, poser);
    mAbsorbVec= poser->getTargetTrans() - mPrevTargetTrans;
    parallelizeVec(&mAbsorbVec, gravity, mAbsorbVec);

    mLookAtCamera.setPos(mCameraPoser->getPosition());
    mLookAtCamera.setAt(mCameraPoser->getTargetTrans());
    mLookAtCamera.setUp(mCameraPoser->getCameraUp());
    mLookAtCamera.normalizeUp();
    makeLookAtCamera(&mLookAtCamera);
    mLookAtCamera.updateViewMatrix();

    mProjection.set(alCameraPoserFunction::getNear(mCameraPoser),
                    alCameraPoserFunction::getFar(mCameraPoser),
                    sead::Mathf::deg2rad(mCameraPoser->getFovyDegree()),
                    alCameraPoserFunction::getAspect(mCameraPoser));
    alCameraPoserFunction::calcTargetFront(&mTargetFront, mCameraPoser);

    if (!isNerve(this, &NrvCameraVerticalAbsorber.FollowGround) &&
        alCameraPoserFunction::isTargetCollideGround(mCameraPoser))
        setNerve(this, &NrvCameraVerticalAbsorber.FollowGround);
    if (!isNerve(this, &NrvCameraVerticalAbsorber.FollowAbsolute) &&
        alCameraPoserFunction::isPlayerTypeNotTouchGround(mCameraPoser))
        setNerve(this, &NrvCameraVerticalAbsorber.FollowAbsolute);
    updateNerve();
    sead::Vector3f prevTargetTrans = {0.0f, 0.0f, 0.0f};
    if (!mIsKeepInFrame) {
        prevTargetTrans = mAbsorbVec;
    } else {
        sead::Vector3f offsetTrans = {0.0f, 0.0f, 0.0f};
        alCameraPoserFunction::calcTargetTransWithOffset(&offsetTrans, mCameraPoser);
        alCameraPoserFunction::calcOffsetCameraKeepInFrameV(
            &gravity, &mLookAtCamera, offsetTrans, mCameraPoser, mKeepInFrameOffsetUp,
            alCameraPoserFunction::isPlayerTypeHighJump(mCameraPoser) ? 300.0f :
                                                                        mKeepInFrameOffsetDown);
        prevTargetTrans = mAbsorbVec- gravity;
    }
    mPrevTargetTrans = mCameraPoser->getTargetTrans() - prevTargetTrans;
    mPrevTargetFront = mTargetFront;
}

void CameraVerticalAbsorber::makeLookAtCamera(sead::LookAtCamera* lookAtCamera) const {
    if (!isValid())
        return;

    lookAtCamera->setAt(lookAtCamera->getAt() - mAbsorbVec);
    if (!mIsNoCameraPosAbsorb)
        lookAtCamera->setPos(lookAtCamera->getPos() - mAbsorbVec);
}

void CameraVerticalAbsorber::liberateAbsorb() {
    if (isNerve(this, &NrvCameraVerticalAbsorber.Absorb))
        setNerve(this, &NrvCameraVerticalAbsorber.Follow);
}

bool CameraVerticalAbsorber::isAbsorbing() const {
    return isValid() && isNerve(this, &NrvCameraVerticalAbsorber.Absorb);
}

void CameraVerticalAbsorber::invalidate() {
    mIsInvalidated = true;
    if (!isNerve(this, &NrvCameraVerticalAbsorber.FollowAbsolute))
        setNerve(this, &NrvCameraVerticalAbsorber.FollowAbsolute);
}

void CameraVerticalAbsorber::tryResetAbsorbVecIfInCollision(const sead::Vector3f& pos) {
    if (!alCameraPoserFunction::checkFirstCameraCollisionArrow(nullptr, nullptr, mCameraPoser,
                                                               pos + mAbsorbVec, -mAbsorbVec))
        return;

    mAbsorbVec= {0.0f, 0.0f, 0.0f};
    if (alCameraPoserFunction::isTargetCollideGround(mCameraPoser))
        setNerve(this, &NrvCameraVerticalAbsorber.FollowGround);
    else
        setNerve(this, &NrvCameraVerticalAbsorber.Follow);
}

inline f32 getSpeed(IUseNerve* nerve, const CameraPoser* camera) {
    if (alCameraPoserFunction::isPlayerTypeHighJump(camera))
        return 0.02f;
    if (isNerve(nerve, &NrvCameraVerticalAbsorber.FollowSlow))
        return 0.02f;
    if (isNerve(nerve, &NrvCameraVerticalAbsorber.FollowClimbPoleNoInterp))
        return 0.3f;
    if (isNerve(nerve, &NrvCameraVerticalAbsorber.FollowClimbPole))
        return calcNerveValue(nerve, 60, 0.05f, 0.3f);
    return 0.05f;
}

// TODO: rename parameters `rateA` and `rateB`
void updateFollowSpeed(f32 rateA, f32 rateB,f32* outValue, IUseNerve* nerve,
                       const CameraPoser* camera) {
    f32 initialValue = *outValue;
    f32 speed = getSpeed(nerve, camera);
    f32 endValue = lerpValue(initialValue, speed, rateA);
    *outValue = lerpValue(*outValue, endValue, rateB);
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

void CameraVerticalAbsorber::exeAbsorb() {
    if (isFirstStep(this)) {
        mIsExistCollisionUnderTarget = false;
        mLerp2 = 0.0f;
        mLerp1 = 0.0f;
    }

    if (isGreaterEqualStep(this, 3) && calcAngleDegree(mTargetFront, mPrevTargetFront) > 170.0f &&
        alCameraPoserFunction::calcTargetSpeedV(mCameraPoser) > 18.0f) {
        setNerve(this, &NrvCameraVerticalAbsorber.Follow);
        return;
    }

    if (!alCameraPoserFunction::isTargetCollideGround(mCameraPoser)) {
        f32 targetSpeedV = alCameraPoserFunction::calcTargetSpeedV(mCameraPoser);
        if (mHighJumpJudgeSpeedV < targetSpeedV) {
            setNerve(this, &NrvCameraVerticalAbsorber.Follow);
            return;
        }
    }

    if (alCameraPoserFunction::isTargetClimbPole(mCameraPoser) && !mCameraPoser->is_98()) {
        setNerve(this, &NrvCameraVerticalAbsorber.FollowClimbPole);
        return;
    }

    if (alCameraPoserFunction::isTargetGrabCeil(mCameraPoser) ||
        alCameraPoserFunction::isTargetWallCatch(mCameraPoser)) {
        setNerve(this, &NrvCameraVerticalAbsorber.FollowSlow);
        return;
    }

    sead::Vector2f pos = {0.0f, 0.0f};
    const sead::Vector3f& poserTrans = mCameraPoser->getTargetTrans();
    sead::Viewport viewport(0.0f, 0.0f, getDisplayWidth(), getDisplayHeight());
    mLookAtCamera.projectByMatrix(&pos, poserTrans, mProjection, viewport);

    pos.x += getDisplayWidth() * 0.5f;
    pos.y = getDisplayHeight() * 0.5f - pos.y;

    if (mAbsorbScreenPosDown < pos.y || pos.y < mAbsorbScreenPosUp) {
        setNerve(this, &NrvCameraVerticalAbsorber.Follow);
        return;
    }
    {
        sead::Vector3f pos2 = {0.0f, 0.0f, 0.0f};
        sead::Vector3f deltaPos = mLookAtCamera.getAt() - mLookAtCamera.getPos();
        normalize(&deltaPos);
        parallelizeVec(&pos2, deltaPos, mAbsorbVec);

        if (pos2.length() > 1000.0f ||
            alCameraPoserFunction::isExistWallCollisionUnderTarget(mCameraPoser)) {
            setNerve(this, &NrvCameraVerticalAbsorber.Follow);
            return;
        }
    }

    if (alCameraPoserFunction::isExistCollisionUnderTarget(mCameraPoser)) {
        if (mIsExistCollisionUnderTarget) {
            sead::Vector3f gravity = {0.0f, 0.0f, 0.0f};
            alCameraPoserFunction::calcTargetGravity(&gravity, mCameraPoser);
            sead::Vector3f popo = alCameraPoserFunction::getUnderTargetCollisionPos(mCameraPoser) -
                                  mUnderTargetCollisionPos;
            parallelizeVec(&popo, gravity, popo);

            if (!isNearZero(popo)) {
                if (popo.dot(gravity) < 0.0f) {
                    if (popo.length() < 30.0f) {
                        sead::Vector3f pos3 =
                            alCameraPoserFunction::getUnderTargetCollisionPos(mCameraPoser);
                        sead::Vector3f collisionDiff = pos3 - mUnderTargetCollisionPos;
                        sead::Vector3f collisionDiff2 = pos3 - mUnderTargetCollisionPos;

                        parallelizeVec(
                            &collisionDiff,
                            alCameraPoserFunction::getUnderTargetCollisionNormal(mCameraPoser),
                            collisionDiff);
                        parallelizeVec(&collisionDiff2, mUnderTargetCollisionNormal,
                                       collisionDiff2);

                        if (sead::Mathf::min(collisionDiff.length(), collisionDiff2.length()) <
                            5.0f) {
                            mLerp2 = popo.length();
                            mUnderTargetCollisionPos.set(
                                alCameraPoserFunction::getUnderTargetCollisionPos(mCameraPoser));
                            goto LAB_7100980840;
                        }
                    }
                    setNerve(this, &NrvCameraVerticalAbsorber.FollowSlow);
                    return;
                }
            }
            mIsExistCollisionUnderTarget = false;
        } else {
            mUnderTargetCollisionPos.set(
                alCameraPoserFunction::getUnderTargetCollisionPos(mCameraPoser));
            mUnderTargetCollisionNormal.set(
                alCameraPoserFunction::getUnderTargetCollisionNormal(mCameraPoser));
            mIsExistCollisionUnderTarget = true;
        }
    LAB_7100980840:

        f32 length = mAbsorbVec.length();
        f32 initialLerp = mLerp2;
        mLerp2 = lerpValue(initialLerp, 0.0f, 0.9f);
        mLerp2 = lerpValue(initialLerp, mLerp2, 0.9f);
        f32 rate = normalize(initialLerp - mLerp2, 0.0f, mAbsorbVec.length());
        normalize2(&mAbsorbVec, length * (1.0f - rate));
    } else {
        mIsExistCollisionUnderTarget = false;
    }

    f32 posUp = 50.0f;
    if (mIsAdvanceAbsorbUp)
        posUp = mAdvanceAbsorbScreenPosUp;
    else if (!alCameraPoserFunction::isTargetInMoonGravity(mCameraPoser))
        posUp = 100.0f;

    if (mAbsorbScreenPosUp < posUp && pos.y < posUp) {
        f32 minVal = easeIn(normalize(pos.y, mAbsorbScreenPosUp, posUp));
        f32 firstPass = lerpValue(mLerp1, minVal, 0.05f);
        mLerp1 = lerpValue(mLerp1, firstPass, 0.05f);
    }
    mAbsorbVec*= 1.0f - mLerp1;
}

void CameraVerticalAbsorber::exeFollow() {
    if (isFirstStep(this))
        mLerp1 = getSpeed(this, mCameraPoser);

   // updateFollowSpeed(&mLerp1, 0.05f, 0.05f, this, mCameraPoser);

    f32 length = mAbsorbVec.length();
    f32 initialLerp = mLerp2;
    mLerp2 = lerpValue(initialLerp, 0.0f, 0.9f);
    mLerp2 = lerpValue(initialLerp, mLerp2, 0.9f);
    f32 rate = normalize(initialLerp - mLerp2, 0.0f, mAbsorbVec.length());

    normalize2(&mAbsorbVec, length * (1.0f - rate));
    mAbsorbVec*= 1.0f - mLerp1;
}



void CameraVerticalAbsorber::exeFollowGround() {
    updateFollowSpeed(0.2f, 0.75f, &mLerp1, this, mCameraPoser);

    f32 length = mAbsorbVec.length();
    f32 initialLerp = mLerp2;
    mLerp2 = lerpValue(initialLerp, 0.0f, 0.9f);
    mLerp2 = lerpValue(initialLerp, mLerp2, 0.9f);
    f32 rate = normalize(initialLerp - mLerp2, 0.0f, mAbsorbVec.length());

    normalize2(&mAbsorbVec, length * (1.0f - rate));
    mAbsorbVec*= 1.0f - mLerp1;

    if (isGreaterEqualStep(this, 3) &&
        !alCameraPoserFunction::isTargetCollideGround(mCameraPoser)) {
        const CameraPoser* poser = mCameraPoser;
        if (alCameraPoserFunction::isExistSlopeCollisionUnderTarget(poser) &&
            !(alCameraPoserFunction::calcTargetSpeedH(poser) < 10.0f)) {
            const sead::Vector3f& normal =
                alCameraPoserFunction::getUnderTargetCollisionNormal(poser);
            if (!(normal.y < sead::Mathf::sin(sead::Mathf::deg2rad(20.0f)))) {
                sead::Vector3f deltaPos = mLookAtCamera.getPos() - mLookAtCamera.getAt();

                if (tryNormalizeOrZero(&deltaPos) &&
                    deltaPos.y < sead::Mathf::sin(sead::Mathf::deg2rad(-15.0f))) {
                    setNerve(this, &NrvCameraVerticalAbsorber.Follow);
                    return;
                }
            }
        }

        setNerve(this, &NrvCameraVerticalAbsorber.Absorb);
    }
}

void CameraVerticalAbsorber::exeFollowClimbPole() {
 //   updateFollowSpeed(&mLerp1, 0.2f, 0.75f, this, mCameraPoser);

    f32 length = mAbsorbVec.length();
    f32 initialLerp = mLerp2;
    mLerp2 = lerpValue(initialLerp, 0.0f, 0.9f);
    mLerp2 = lerpValue(initialLerp, mLerp2, 0.9f);
    f32 rate = normalize(initialLerp - mLerp2, 0.0f, mAbsorbVec.length());

    normalize2(&mAbsorbVec, length * (1.0f - rate));
    mAbsorbVec*= 1.0f - mLerp1;

    if (!alCameraPoserFunction::isTargetClimbPole(mCameraPoser))
        setNerve(this, &NrvCameraVerticalAbsorber.Follow);
}

void CameraVerticalAbsorber::exeFollowAbsolute() {
    mAbsorbVec*= 0.8f;
}

}  // namespace al

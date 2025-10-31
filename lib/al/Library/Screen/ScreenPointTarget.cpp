#include "Library/Screen/ScreenPointTarget.h"

#include "Library/Base/StringUtil.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/Screen/ScreenPointCheckGroup.h"
#include "Library/Yaml/ParameterBase.h"
#include "Library/Yaml/ParameterObj.h"

namespace al {
ScreenPointTarget::ScreenPointTarget(LiveActor* actor, const char* nameA, f32 vbloat,
                                     const sead::Vector3f* posA, const char* nameB,
                                     const sead::Vector3f& posB)
    :     mParameterObj(new ParameterObj()),
        mParameterBase(new ParameterStringRef(nameA ? nameA : "", "Name", "Name", "", mParameterObj, true)),
        mParameterBase2(new ParameterF32(vbloat, "Radius", "Radius", "", mParameterObj, true)),
        mParameterBase3(new ParameterV3f(posB, "Offset", "Offset", "", mParameterObj, true)),
        mJointName(new ParameterStringRef(nameB ? nameB : "", "Joint", "Joint", "", mParameterObj, true)),
        _38(posA), _48(posB), mActor(actor) {


        sead::Matrix34f* jointMtx = nullptr;
    if (getJointName()) {
        if (!isEqualString("", getJointName()))
            jointMtx = getJointMtxPtr(actor, getJointName());
        else
            jointMtx = nullptr;
    }
    mJointMtx = jointMtx;
}

void ScreenPointTarget::setFollowMtxPtrByJointName(const LiveActor* actor) {
    if (getJointName() && !isEqualString("", getJointName())) {
        mJointMtx = getJointMtxPtr(actor, getJointName());
        return;
    }

    mJointMtx = nullptr;
}

const char* ScreenPointTarget::getJointName() const {
    return mJointName->getValue();
}

void ScreenPointTarget::update() {}

void ScreenPointTarget::validate() {
    if (mBb)
        return;

    mBb = true;
    if (mBa)
        mCheckGroup->setValid(this);
}

void ScreenPointTarget::invalidate() {
    if (!mBb)
        return;

    mBb = false;
    if (mBa)
        mCheckGroup->setInvalid(this);
}

void ScreenPointTarget::validateBySystem() {
    if (mBa)
        return;

    if (mBb)
        mCheckGroup->setValid(this);
    mBa = true;
}

void ScreenPointTarget::invalidateBySystem() {
    if (!mBa)
        return;

    if (mBb)
        mCheckGroup->setInvalid(this);
    mBa = false;
}

f32 ScreenPointTarget::getTargetRadius() const {
    return mParameterBase2->getValue() * bloat;
}

const char* ScreenPointTarget::getTargetName() const {
    return mParameterBase->getValue();
}

void ScreenPointTarget::setTargetName(const char* name) {
    return mParameterBase->setValue(name);
}

void ScreenPointTarget::setTargetRadius(f32 radius) {
    return mParameterBase2->setValue(radius);
}

void ScreenPointTarget::setTargetFollowPosOffset(const sead::Vector3f& posOffset) {
    return mParameterBase3->setValue(posOffset);
}

void ScreenPointTarget::setJointName(const char* name) {
    return mJointName->setValue(name);
}

}  // namespace al

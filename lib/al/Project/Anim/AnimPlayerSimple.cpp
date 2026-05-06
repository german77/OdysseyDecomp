#include "Project/Anim/AnimPlayerSimple.h"

#include <nn/g3d/AnimObj.h>

#include "Project/Anim/AnimInfo.h"

namespace al {
AnimPlayerSimple::AnimPlayerSimple() {
    mModelInfo = new ModelInfo();
}

void AnimPlayerSimple::applyTo() {
    mModelInfo->animObj->Calculate();
    mModelInfo->animObj->ApplyTo(mModelInfo->modelObj);
}

bool AnimPlayerSimple::calcNeedUpdateAnimNext() {
    if (!isAnimationPlaying())
        return false;

    applyTo();

    if (getAnimFrameRate() <= 0 || (isAnimOneTime() && isAnimEnd())) {
        setAnimToModel(nullptr);
        stopAnimation();
    }
    return true;
}

void AnimPlayerSimple::startAnim(const char* name) {
    mResInfo = getAnimInfoTable()->findAnimInfo(name);
    setAnimToModel(mResInfo);
    applyTo();
    set10(true);
    startAnimation();
}

void AnimPlayerSimple::update() {
    if (isAnimationPlaying() && !is10())
        mModelInfo->animObj->getFrameCtrlPtr()->update();
}

void AnimPlayerSimple::clearAnim() {
    mModelInfo->animObj->ClearResult();
}

f32 AnimPlayerSimple::getAnimFrame() const {
    return mModelInfo->animObj->getFrameCtrlPtr()->getFrame();
}

void AnimPlayerSimple::setAnimFrame(f32 frame) {
    mModelInfo->animObj->getFrameCtrlPtr()->setFrame(frame);
    applyTo();
    set10(true);
    startAnimation();
}

f32 AnimPlayerSimple::getAnimFrameMax() const {
    return mModelInfo->animObj->getFrameCtrlPtr()->getFrameMax();
}

f32 AnimPlayerSimple::getAnimFrameMax(const char* name) const {
    return getAnimInfoTable()->findAnimInfo(name)->frameMax;
}

f32 AnimPlayerSimple::getAnimFrameRate() const {
    return mModelInfo->animObj->getFrameCtrlPtr()->getFrameRate();
}

void AnimPlayerSimple::setAnimFrameRate(f32 rate) {
    mModelInfo->animObj->getFrameCtrlPtr()->setFrameRate(rate);
    applyTo();
    startAnimation();
}

bool AnimPlayerSimple::isAnimExist(const char* name) const {
    return getAnimInfoTable()->tryFindAnimInfo(name) != nullptr;
}

bool AnimPlayerSimple::isAnimEnd() const {
    return !isAnimPlaying() || mModelInfo->animObj->getFrameCtrlPtr()->isEnd();
}

bool AnimPlayerSimple::isAnimOneTime() const {
    return mModelInfo->animObj->getFrameCtrlPtr()->isOneTime();
}

bool AnimPlayerSimple::isAnimOneTime(const char* name) const {
    return !getAnimInfoTable()->findAnimInfo(name)->isLooping;
}

bool AnimPlayerSimple::isAnimPlaying() const {
    return mResInfo != nullptr;
}

const char* AnimPlayerSimple::getPlayingAnimName() const {
    return isAnimPlaying() ? mResInfo->name : "";
}

}  // namespace al

#include "Library/Model/SkyDirector.h"

#include "Library/Base/StringUtil.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorInitInfo.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Obj/Sky.h"
#include "Library/Placement/PlacementInfo.h"
#include "Library/Yaml/ParameterBase.h"

namespace al {

// NON-MATCHING: Stack mismatch https://decomp.me/scratch/uG8AT
SkyParam::SkyParam() {
    mParameterObj = new ParameterObj();

    mSkyName = new ParameterString64(StringTmp<64>("RSGraphicTestSkyBlue"), mParameterObj, "Name",
                                     "空のモデル名", "", true);

    mRotate = new ParameterV3f({0.0f, 0.0f, 0.0f}, mParameterObj, "Rotate", "回転",
                               "Min=-360.0f, Max=360.0f", true);

    mStarIntensity = new ParameterF32(0.0f, mParameterObj, "StarIntensity", "星の明るさ",
                                      "Min=0.0f, Max=100.0f", true);
}

bool SkyParam::isEqual(const IUseRequestParam& other) const {
    const SkyParam* param = static_cast<const SkyParam*>(&other);

    return isNear(getRotate(), param->getRotate()) &&
           isEqualString(getSkyName(), param->getSkyName()) &&
           isNear(getStarIntensity(), param->getStarIntensity());
}

const sead::Vector3f& SkyParam::getRotate() const {
    return mRotate->getValue();
}

const char* SkyParam::getSkyName() const {
    return mSkyName->getValue().cstr();
}

f32 SkyParam::getStarIntensity() const {
    return mStarIntensity->getValue();
}

SkyDirector::SkyDirector() : mParamRequestInterp(new ParamRequestInterp) {
    _30="";
    mParamRequestInterp->initialize<SkyParam>();
    mSkyArray.allocBuffer(128, nullptr);
}

void SkyDirector::initProjectResource() {}

void SkyDirector::init(const ActorInitInfo& info) {
    PlacementInfo placementInfo;
    ActorInitInfo preset;
    preset.initNoViewId(&placementInfo, info);

    for (s32 i = 0; i < mSkyArray.size(); i++)
        mSkyArray[i]->initFromPreset(preset);

    mIsInitialized = true;
}

void SkyDirector::endInit() {
    if (mIsInitialized)
        mParamRequestInterp->endInit();
}

void SkyDirector::clearRequest() {
    if (mIsInitialized)
        mParamRequestInterp->clearRequest();
}

void SkyDirector::update() {
    if (!mIsInitialized)
        return;

    mParamRequestInterp->updateInterp();
    SkyParam* currentParam = getCurrentParam();
    if (!currentParam)
        return;

    if (mActor) {
        sead::Vector3f rotate = currentParam->getRotate();
        rotate.add(mRotateOffset);
        setRotate(mActor, rotate);
    }

    Sky* currentSky = tryGetSky(currentParam->getSkyName());
    if (!currentSky || mActor == currentSky)
        return;

    if (isDead(currentSky) && !currentSky->isOnlyCubeMap()) {
        mActor = currentSky;
        mActor->appear();
        const char* name = getModelName(mActor);
        tryStartAction(mActor, name);

        sead::Vector3f rotate = currentParam->getRotate();
        rotate.add(mRotateOffset);
        setRotate(mActor, rotate);
    }

    if (!mActor || !isAlive(mActor))
        return;

    for (s32 i = 0; i < mSkyArray.size(); i++) {
        Sky* sky = mSkyArray[i];
        if (sky != mActor)
            sky->kill();
    }
}

SkyParam* SkyDirector::getCurrentParam() const {
    return static_cast<SkyParam*>(mParamRequestInterp->getCurrentParam());
}

Sky* SkyDirector::tryGetSky(const char* name) const {
    for (s32 i = 0; i < mSkyArray.size(); i++) {
        Sky* sky = mSkyArray[i];
        if (isEqualString(sky->getName(), name))
            return sky;
    }
    return nullptr;
}

bool SkyDirector::tryRegistAndCreateSky(const char* name) {
    for (s32 i = 0; i < mSkyArray.size(); i++) {
        Sky* sky = mSkyArray[i];
        if (isEqualString(sky->getName(), name)) {
            if (sky)
                return false;
            break;
        }
    }

    if (mSkyArray.size() >= 128)
        return false;

    mSkyArray.pushBack(new Sky(name));
    return true;
}

bool SkyDirector::requestParam(s32 a, s32 b, const SkyParam& param) {
    return mParamRequestInterp->requestParam(a, b, param);
}

f32 SkyDirector::getCurrentStarIntensity() const {
    return getCurrentParam()->getStarIntensity();
}

Sky* SkyDirector::tryGetCurrentSky() const {
    return tryGetSky(getCurrentParam()->getSkyName());
}

}  // namespace al

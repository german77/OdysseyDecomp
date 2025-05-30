
#include "Library/Model/ModelKeeper.h"

#include "Library/Anim/AnimPlayerSkl.h"
#include "Library/Model/ModelCtrl.h"
#include "Library/Resource/Resource.h"
#include "Library/Resource/ResourceHolder.h"
#include "Project/Anim/AnimPlayerInitInfo.h"
#include "Project/Anim/AnimPlayerVisual.h"

namespace al {
ModelKeeper::ModelKeeper() = default;

ModelKeeper::~ModelKeeper() {
    if (mModelCtrl) {
        delete mModelCtrl;
        mModelCtrl = nullptr;
    }
};

void ModelKeeper::initResource(const al::ActorResource* resource) {
    mActorRes = resource;
    mName = getResourceName(mActorRes->getModelRes());
    mModelCtrl = new ModelCtrl();
    mModelCtrl->initResource(mActorRes->getModelRes(), mActorRes->getAnimRes());
}

void ModelKeeper::createMatAnimForProgram(s32 index) {
    AnimPlayerInitInfo initInfo;
    initInfo.animRes = mActorRes->getAnimRes() ? mActorRes->getAnimRes() : mActorRes->getModelRes() ;
    initInfo.modelObj = mModelCtrl->getModelObj();
    initInfo.modelRes = mActorRes->getModelRes();
    initInfo.animResData = mActorRes->getAnimResData();

    mAnimMat = AnimPlayerMat::tryCreate(&initInfo, index);
}

Resource* ModelKeeper::getAnimResource() const {
    return mActorRes->getAnimRes() ? mActorRes->getAnimRes() : mActorRes->getModelRes();
}

Resource* ModelKeeper::getModelResource() const {
    return mActorRes->getModelRes();
}

void ModelKeeper::setDisplayRootJointMtxPtr(const sead::Matrix34f* mtx) {
    mModelCtrl->setDisplayRootJointMtxPtr(mtx);
}

void ModelKeeper::setModelLodCtrl(ModelLodCtrl* modelLodCtrl) {
    mModelCtrl->setLodCtrl(modelLodCtrl);
}

void ModelKeeper::setDitherAnimator(DitherAnimator* ditherAnimator) {
    mModelCtrl->setDitherAnimator(ditherAnimator);
}

void ModelKeeper::initModel(s32, GpuMemAllocator*, ModelShaderHolder*,
                            ModelOcclusionCullingDirector*, ShadowDirector*,
                            PrepassTriangleCulling*) {}

void ModelKeeper::show() {
    if (mAnimSkl)
        mAnimSkl->reset();
    if (mModelCtrl)
        mModelCtrl->show();
}

void ModelKeeper::hide() {
    if (mModelCtrl)
        mModelCtrl->hide();
}

void ModelKeeper::update() {
    if (mAnimSkl)
        mAnimSkl->update();
    if (mAnimMtp)
        mAnimMtp->update();
    if (mAnimMcl)
        mAnimMcl->update();
    if (mAnimMts)
        mAnimMts->update();
    if (mAnimMat)
        mAnimMat->update();
    if (mAnimVisForAction)
        mAnimVisForAction->update();
    if (mAnimVis)
        mAnimVis->update();
}

void ModelKeeper::updateLast() {
    if (mAnimSkl)
        mAnimSkl->updateLast();
    if (mAnimMtp)
        mAnimMtp->updateLast();
    if (mAnimMcl)
        mAnimMcl->updateLast();
    if (mAnimMts)
        mAnimMts->updateLast();
    if (mAnimMat)
        mAnimMat->updateLast();
    if (mAnimVisForAction)
        mAnimVisForAction->updateLast();
    if (mAnimVis)
        mAnimVis->updateLast();
}

void ModelKeeper::calc(const sead::Matrix34f&, const sead::Vector3f&) {
    if (mAnimSkl)
        mAnimSkl->calcNeedUpdateAnimNext();
    if (mAnimMtp)
        mAnimMtp->calcNeedUpdateAnimNext();
    if (mAnimMcl)
        mAnimMcl->calcNeedUpdateAnimNext();
    if (mAnimMts)
        mAnimMts->calcNeedUpdateAnimNext();
    if (mAnimMat)
        mAnimMat->calcNeedUpdateAnimNext();
    if (mAnimVisForAction)
        mAnimVisForAction->calcNeedUpdateAnimNext();
    if (mAnimVis)
        mAnimVis->calcNeedUpdateAnimNext();
}

const sead::Matrix34f* ModelKeeper::getBaseMtx() const {
    return &mModelCtrl->getBaseMtx();
}

sead::Matrix34f* ModelKeeper::getWorldMtxPtrByIndex(s32 index) const {
    return &mWorldMtxHolder[index];
}

}  // namespace al

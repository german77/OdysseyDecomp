
#include "Library/Model/ModelKeeper.h"

#include <nn/g3d/ModelObj.h>
#include <nn/g3d/SkeletonObj.h>

#include "Library/Anim/AnimPlayerSkl.h"
#include "Library/Model/ModelCtrl.h"
#include "Library/Resource/ActorResource.h"
#include "Library/Resource/Resource.h"
#include "Library/Resource/ResourceFunction.h"
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
    Resource* animRes = getAnimResource();
    nn::g3d::ModelObj* modelObj = mModelCtrl->getModelObj();
    Resource* modelRes = getModelResource();
    InitResourceDataAnim* animResData = mActorRes->getAnimResData();

    AnimPlayerInitInfo initInfo{
        .animRes = animRes,
        .modelObj = modelObj,
        .modelRes = modelRes,
        .animResData = animResData,
    };

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

void ModelKeeper::calc(const sead::Matrix34f& mtx, const sead::Vector3f& vec) {
    bool isUpdate=false;
    if (mAnimSkl)
        isUpdate=mAnimSkl->calcNeedUpdateAnimNext();
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

    bool isSkeletonUpdate =  mModelCtrl->setSkeletonUpdateInfo(isUpdate,mtx,vec);
    if(_60){
        _60=false;
        mModelCtrl->recreateDisplayList();
    }

    mModelCtrl->calc(mtx,vec);
    auto skeleton=mModelCtrl->getModelObj()->GetSkeleton();
    if(skeleton && !isSkeletonUpdate){
        s32 size=skeleton->GetSize();
        for(s32 i=0;i<size;i++){
            skeleton->setFlag();
            nn::g3d::matrix44& mtx = skeleton->getMtxArray()[i];
            auto& mtx2 = mWorldMtxHolder[i];
            mtx2.m[0][0]=mtx.m[0][0];
            mtx2.m[0][1]=mtx.m[0][1];
            mtx2.m[0][2]=mtx.m[0][2];
            mtx2.m[0][3]=mtx.m[0][3];
            mtx2.m[1][0]=mtx.m[1][0];
            mtx2.m[1][1]=mtx.m[1][1];
        }
    }
}

const sead::Matrix34f* ModelKeeper::getBaseMtx() const {
    return &mModelCtrl->getBaseMtx();
}

sead::Matrix34f* ModelKeeper::getWorldMtxPtrByIndex(s32 index) const {
    return &mWorldMtxHolder[index];
}

}  // namespace al

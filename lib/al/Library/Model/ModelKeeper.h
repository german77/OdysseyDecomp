#pragma once

#include <basis/seadTypes.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>

#include "Library/HostIO/HioNode.h"

namespace al {
class ActorResource;
class AnimPlayerMat;
class AnimPlayerSimple;
class AnimPlayerSkl;
class AnimPlayerVis;
class DitherAnimator;
class GpuMemAllocator;
class ModelCtrl;
class ModelLodCtrl;
class ModelOcclusionCullingDirector;
class ModelShaderHolder;
class PrepassTriangleCulling;
class Resource;
class ShadowDirector;

class ModelKeeper : public HioNode {
public:
    ModelKeeper();

    virtual ~ModelKeeper();

    void initResource(const al::ActorResource* resource);
    void createMatAnimForProgram(s32);
    Resource* getAnimResource() const;
    Resource* getModelResource() const;
    void setDisplayRootJointMtxPtr(const sead::Matrix34f* mtx);
    void setModelLodCtrl(ModelLodCtrl* modelLodCtrl);
    void setDitherAnimator(DitherAnimator* ditherAnimator);
    void initModel(s32, GpuMemAllocator*, ModelShaderHolder*, ModelOcclusionCullingDirector*,
                   ShadowDirector*, PrepassTriangleCulling*);
    void show();
    void hide();
    void update();
    void updateLast();
    void calc(const sead::Matrix34f&, const sead::Vector3f&);
    const sead::Matrix34f* getBaseMtx() const;
    sead::Matrix34f* getWorldMtxPtrByIndex(s32 index) const;

    ModelCtrl* getModelCtrl() const { return mModelCtrl; }

    AnimPlayerSkl* getAnimSkl() const { return mAnimSkl; }

    AnimPlayerMat* getAnimMtp() const { return mAnimMtp; }

    AnimPlayerMat* getAnimMts() const { return mAnimMts; }

    AnimPlayerMat* getAnimMcl() const { return mAnimMcl; }

    AnimPlayerMat* getAnimMat() const { return mAnimMat; }

    AnimPlayerVis* getAnimVis() const { return mAnimVis; }

    AnimPlayerVis* getAnimVisForAction() const { return mAnimVisForAction; }

    bool isFixedModel() const { return mIsFixedModel; }

    bool isIgnoreUpdateDrawClipping() const { return mIsIgnoreUpdateDrawClipping; }

    bool isNeedSetBaseMtxAndCalcAnim() const { return mIsNeedSetBaseMtxAndCalcAnim; }

private:
    const char* mName = nullptr;
    ModelCtrl* mModelCtrl = nullptr;
    const ActorResource* mActorRes = nullptr;
    AnimPlayerSkl* mAnimSkl = nullptr;
    AnimPlayerMat* mAnimMtp = nullptr;
    AnimPlayerMat* mAnimMts = nullptr;
    AnimPlayerMat* mAnimMcl = nullptr;
    AnimPlayerMat* mAnimMat = nullptr;
    AnimPlayerVis* mAnimVisForAction = nullptr;
    AnimPlayerVis* mAnimVis = nullptr;
    sead::Matrix34f* mWorldMtxHolder = nullptr;
    bool _60 = false;
    bool mIsFixedModel = false;
    bool mIsIgnoreUpdateDrawClipping = false;
    bool mIsNeedSetBaseMtxAndCalcAnim = true;
};

}  // namespace al

#pragma once

#include <basis/seadTypes.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>

namespace agl {
class DisplayList;
}  // namespace agl

namespace nn::g3d {
class ModelObj;
class ResModel;
class Sphere;
}  // namespace nn::g3d

namespace sead {
class Heap;
}  // namespace sead

namespace al {
class ActorDitherAnimator;
class DitherAnimator;
class EnvTexInfo;
class GpuMemAllocator;
class GraphicsQualityInfo;
class ModelDisplayListHolder;
class ModelLodCtrl;
class ModelMaterialCategory;
class ModelOcclusionCullingDirector;
class ModelOcclusionQuery;
class ModelShader;
class ModelShaderHolder;
class ModelShapeCtrl;
class PrepassTriangleCulling;
class Resource;
class ShaderHolder;
class ShadowDirector;

class ModelCtrl {
public:
    ModelCtrl();
    ~ModelCtrl();

    void initialize(nn::g3d::ResModel*, s32, s32, sead::Heap*, ShaderHolder*);
    bool tryBindShader();
    bool tryUpdateModelAdditionalInfoUbo(s32);
    void updateWorldMatrix(const sead::Matrix34f&, const sead::Vector3f&);
    void updateGpuBuffer(s32);
    void calcBounding();
    void getLodLevel() const;
    void getLodLevelDepthShadow() const;
    void calcBoundingLod(s32);
    void updateQueryBox();
    void updateModelDrawBuffer(s32);
    void updateGpuBufferAll();
    bool isShapeVisible(s32) const;
    void setCubeMapIndexAllShape(s32);
    void recreateDisplayList();
    void setMaterialProgrammable(s32, bool);
    bool isMaterialProgrammable(s32);
    bool setSkeletonUpdateInfo(bool, const sead::Matrix34f&, const sead::Vector3f&);
    void setDirtyTexture();
    void onZPrePass();
    void offZPrePass();
    EnvTexInfo* getEnvTexInfo(s32) const;
    void requestModelAdditionalInfoUbo();
    void setLodCtrl(ModelLodCtrl*);
    s32 getLodLevelMax() const;
    s32 getLodLevelMaterial() const;
    s32 getLodLevelNoClamp() const;
    void setLodLevelForce(s32);
    bool updateLodCtrl();
    void setDitherAnimator(DitherAnimator*);
    void updateDitherAnimator();
    void checkChangeDisplayList();
    void addToDrawerCulling();
    void removeFromDrawerCulling();
    bool updateSubMesh();
    void setModelMaterialCategory(ModelMaterialCategory const*);
    void setModelAlphaMask(f32);
    void setModelUvOffset(const sead::Vector2f&);
    void setModelProjMtx0(const sead::Matrix44f&);
    void setModelProjMtx1(const sead::Matrix44f&);
    void setModelProjMtx2(const sead::Matrix44f&);
    void setModelProgProjMtx0(const sead::Matrix44f&);
    void setModelProgProjMtx1(const sead::Matrix44f&);
    void setModelProgProjMtx2(const sead::Matrix44f&);
    void setModelProgProjMtx3(const sead::Matrix44f&);
    void setModelProgConstant0(f32);
    void setModelProgConstant1(f32);
    void setModelProgConstant2(f32);
    void setModelProgConstant3(f32);
    void setModelProgConstant4(f32);
    void setModelProgConstant5(f32);
    void setModelProgConstant6(f32);
    void setModelProgConstant7(f32);
    void setNormalAxisXScale(f32);
    f32 calcCameraToBoundingSphereDistance() const;
    bool isUseLocalShadowMap() const;
    void validateOcclusionQuery();
    void invalidateOcclusionQuery();
    bool isValidOcclusionQuery() const;
    void createUniqShader();
    bool isCreateUniqShader(s32);
    ModelShader* getUniqModelShader(s32);
    ModelShader* getUniqModelShaderAssgin(s32);
    void pushDisplayListModel(agl::DisplayList*);
    void pushDisplayListShape(agl::DisplayList*, s32);
    ModelShapeCtrl* getModelShapeCtrl(s32) const;
    void initResource(Resource*, Resource*);
    void initModel(GpuMemAllocator*, ModelShaderHolder*, ModelOcclusionCullingDirector*,
                   ShadowDirector*, PrepassTriangleCulling*, s32, s32);
    void tryCreateCulledIndexBuffer();
    void show();
    void hide();
    void calc(const sead::Matrix34f&, const sead::Vector3f&);
    bool calcView();
    void calcModelObjBoundingWithOffset(nn::g3d::Sphere*) const;
    void setCameraInfo(const sead::Matrix34f*, const sead::Matrix34f*, const sead::Matrix44f*,
                       const sead::Matrix44f*);
    void getShapeObj(s32) const;

    nn::g3d::ModelObj* getModelObj() { return mModelObj; }

    ActorDitherAnimator* getActorDitherAnimator() { return mActorDitherAnimator; }

    s32 getCalcViewCore() const { return mCalcViewCore; }

    void setCalcViewCore(s32 core) { mCalcViewCore = core; }

    void setGraphicsQualityInfo(GraphicsQualityInfo* info) { mGraphicsQualityInfo = info; }

    const sead::Matrix34f& getBaseMtx() const { return mBaseMtx; }

    void setDisplayRootJointMtxPtr(const sead::Matrix34f* mtx) { displayRootJointMtx = mtx; }

    void setModelOcclusionQuery(ModelOcclusionQuery* query) { mModelOcclusionQuery = query; }

    void set_169(bool isSet){_169=isSet;}

private:
    nn::g3d::ModelObj* mModelObj;
    void* _8;
    GpuMemAllocator* mGpuMemAllocator;
    ModelShaderHolder* mShaderHolder;
    s32 mBlockBufferSize;
    char padding1[0x134];  // MemoryPool
    s32 _158;
    s32 _15c;
    bool mIsDirtyTexture;
    bool _161;
    bool _162;
    bool _163;
    bool _164;
    bool _165;
    bool _166;
    bool _167;
    bool _168;
    bool _169;
    GraphicsQualityInfo* mGraphicsQualityInfo;
    f32 _17c;
    sead::Vector2f mUvOffset;
    sead::Matrix44f _184;
    sead::Matrix44f _1c4;
    sead::Matrix44f _204;
    sead::Matrix44f _244;
    sead::Matrix44f _284;
    sead::Matrix44f _2c4;
    sead::Matrix44f _304;
    f32 mProgConstant0;
    f32 mProgConstant1;
    f32 mProgConstant2;
    f32 mProgConstant3;
    f32 mProgConstant4;
    f32 mProgConstant5;
    f32 mProgConstant6;
    f32 mProgConstant7;
    f32 mNormalAxisXScale;
    ModelLodCtrl* mModelLodCtrl;
    void* _370;
    DitherAnimator* mDitherAnimator;
    ActorDitherAnimator* mActorDitherAnimator;
    ModelDisplayListHolder* mModelDisplayListHolder;
    void* _390;
    ShadowDirector* mShadowDirector;
    PrepassTriangleCulling* mPrepassTriangleCulling;
    s32 _3a8;
    s32 mCalcViewCore;
    Resource* _3b0;
    Resource* _3b8;
    void* _3c0;
    sead::Matrix34f mBaseMtx;
    sead::Vector3f _3f8;
    const sead::Matrix34f* displayRootJointMtx;
    sead::Matrix34f* _410;
    sead::Matrix34f* _418;
    sead::Matrix44f* _420;
    sead::Matrix44f* _428;
    ModelOcclusionQuery* mModelOcclusionQuery;
    ModelOcclusionCullingDirector* mModelOcclusionCullingDirector;
    void* _440;
};

static_assert(sizeof(ModelCtrl) == 0x448);
}  // namespace al

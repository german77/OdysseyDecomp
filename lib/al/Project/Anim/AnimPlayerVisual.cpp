#include "Project/Anim/AnimPlayerVisual.h"

#include <nn/g3d/MaterialAnimObj.h>

#include "Project/Anim/AnimInfo.h"
#include "Project/Anim/AnimPlayerInitInfo.h"
#include "Project/Anim/InitResourceDataAnim.h"

namespace al {
AnimPlayerMat* AnimPlayerMat::tryCreate(const AnimPlayerInitInfo* info, s32 matType) {
    if (!info->animResData)
        return nullptr;

    if (info->animResData->getAnimInfoTable(matType))
        return nullptr;

    AnimPlayerMat* mat = new AnimPlayerMat(matType);
    mat->init(info);
    return mat;
}

void AnimPlayerMat::init(const AnimPlayerInitInfo* info) {
    setModelObj(info->modelObj);

    setAnimInfoTable(info->animResData->getAnimInfoTable(mMatType));
    mModelRes = info->modelRes;
    mAnimRes = info->animRes;

    setAnimObj(new nn::g3d::MaterialAnimObj);
}

void AnimPlayerMat::setAnimToModel(const AnimResInfo* info) {
    if (!info)
        return;

    nn::g3d::MaterialAnimObj* animObj = (nn::g3d::MaterialAnimObj*)getAnimObj();
    animObj->SetResource((nn::g3d::ResMaterialAnim*)info->buffer);
    animObj->Bind(getModelObj());
    if (mMatType == 1 && mAnimRes != mModelRes) {
    }
}

AnimPlayerVis* AnimPlayerVis::tryCreate(const AnimPlayerInitInfo*, s32) {}

AnimPlayerVis::AnimPlayerVis() {}

void AnimPlayerVis::init(const AnimPlayerInitInfo*) {}

void AnimPlayerVis::setAnimToModel(const AnimResInfo*) {}
}  // namespace al

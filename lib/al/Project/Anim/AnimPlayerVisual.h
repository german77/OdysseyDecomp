#pragma once

#include "Project/Anim/AnimPlayerSimple.h"

namespace al {
struct AnimPlayerInitInfo;
struct AnimResInfo;
class Resource;

class AnimPlayerMat : public AnimPlayerSimple {
public:
    static AnimPlayerMat* tryCreate(const AnimPlayerInitInfo*, s32 matType);

    AnimPlayerMat(s32 matType):mMatType(matType){}

    void init(const AnimPlayerInitInfo*) override;
    void setAnimToModel(const AnimResInfo*) override;

private:
    s32 mMatType;
    Resource* mModelRes=nullptr;
    Resource* mAnimRes=nullptr;
};
static_assert(sizeof(AnimPlayerMat) == 0x40);

class AnimPlayerVis : public AnimPlayerSimple {
public:
    static AnimPlayerVis* tryCreate(const AnimPlayerInitInfo*, s32);

    AnimPlayerVis();

    void init(const AnimPlayerInitInfo*) override;
    void setAnimToModel(const AnimResInfo*) override;
};
}  // namespace al

#pragma once

#include <math/seadQuat.h>
#include <math/seadVector.h>

namespace al {
class CollisionParts;
class LiveActor;
class Triangle;

class SphereInterpolator {
public:
    SphereInterpolator() {}

    void startInterp(const sead::Vector3f& posStart, const sead::Vector3f& posEnd, f32 sizeStart,
                     f32 sizeEnd, f32 steps);
    void nextStep();
    void calcInterpPos(sead::Vector3f* pos) const;
    void calcInterp(sead::Vector3f* pos, f32* size, sead::Vector3f* remainMoveVec) const;
    void calcRemainMoveVector(sead::Vector3f* remainMoveVec) const;
    void getMoveVector(sead::Vector3f* moveVec);
    void calcStepMoveVector(sead::Vector3f* moveVec) const;

private:
    sead::Vector3f mPos;
    sead::Vector3f mMove;
    f32 mSizeStart;
    f32 mSizeEnd;
    f32 mStepSize;
    f32 mCurrentStep;
    f32 mPrevStep;
};

class SpherePoseInterpolator {
public:
    SpherePoseInterpolator() {}

    void startInterp(const sead::Vector3f& posStart, const sead::Vector3f& posEnd, f32 sizeStart,
                     f32 sizeEnd, const sead::Quatf& quatStart, const sead::Quatf& quatEnd,
                     f32 steps);
    void nextStep();
    void calcInterpPos(sead::Vector3f* pos) const;
    void calcInterp(sead::Vector3f* pos, f32* size, sead::Quatf* quat,
                    sead::Vector3f* remainMoveVec) const;
    void calcRemainMoveVector(sead::Vector3f* remainMoveVec) const;
    f32 calcRadiusBaseScale(f32 unk) const;
    void getMoveVector(sead::Vector3f* moveVec);

private:
    sead::Vector3f mPos;
    sead::Vector3f mMove;
    f32 mSizeStart;
    f32 mSizeEnd;
    sead::Quatf mQuatStart;
    sead::Quatf mQuatEnd;
    f32 mStepSize;
    f32 mCurrentStep;
    f32 mPrevStep;
};

class CollisionPartsFilterBase {
public:
    virtual bool isInvalidParts(CollisionParts* collisionParts) = 0;
};

class CollisionPartsFilterActor : public CollisionPartsFilterBase {
public:
    CollisionPartsFilterActor(const LiveActor* actor) : mActor(actor) {}

    bool isInvalidParts(CollisionParts* collisionParts) override;

private:
    const LiveActor* mActor;
    bool mIsCompareEqual = true;
};

class CollisionPartsFilterSubActor : public CollisionPartsFilterBase {
public:
    CollisionPartsFilterSubActor(const LiveActor* actor) : mActor(actor) {}

    bool isInvalidParts(CollisionParts* collisionParts) override;

private:
    const LiveActor* mActor;
};

class CollisionPartsFilterSpecialPurpose : public CollisionPartsFilterBase {
public:
    CollisionPartsFilterSpecialPurpose(const char* specialPurpose)
        : mSpecialPurpose(specialPurpose) {}

    bool isInvalidParts(CollisionParts* collisionParts) override;

private:
    const char* mSpecialPurpose;
};

class CollisionPartsFilterIgnoreOptionalPurpose : public CollisionPartsFilterBase {
public:
    CollisionPartsFilterIgnoreOptionalPurpose(const char* specialPurpose)
        : mSpecialPurpose(specialPurpose) {}

    bool isInvalidParts(CollisionParts* collisionParts) override;

private:
    const char* mSpecialPurpose;
};

class CollisionPartsFilterMergePair : public CollisionPartsFilterBase {
public:
    CollisionPartsFilterMergePair(CollisionPartsFilterBase* firstFilter,
                                  CollisionPartsFilterBase* secondFilter)
        : mFirstFilter(firstFilter), mSecondFilter(secondFilter) {}

    bool isInvalidParts(CollisionParts* collisionParts) override;

private:
    CollisionPartsFilterBase* mFirstFilter;
    CollisionPartsFilterBase* mSecondFilter;
};

class TriangleFilterBase {
public:
    virtual bool isInvalidTriangle(const Triangle& triangle) const = 0;
};

class TriangleFilterGroundOnly : public TriangleFilterBase {
public:
    TriangleFilterGroundOnly(const sead::Vector3f& down) : mDown(down) {}

    bool isInvalidTriangle(const Triangle& triangle) const override;

private:
    const sead::Vector3f& mDown;
};

class TriangleFilterWallOnly : public TriangleFilterBase {
public:
    TriangleFilterWallOnly(const sead::Vector3f& down) : mDown(down) {}

    bool isInvalidTriangle(const Triangle& triangle) const override;

private:
    const sead::Vector3f& mDown;
};

template <typename T>
class TriangleFilterDelegator : public TriangleFilterBase {
public:
    using DelegateFilter = bool (T::*)(const Triangle&) const;

    TriangleFilterDelegator(T* parent, DelegateFilter filter) : mParent(parent), mFunc(filter) {}

    bool isInvalidTriangle(const Triangle& triangle) const override {
        return (mParent->*mFunc)(triangle);
    }

private:
    T* mParent;
    DelegateFilter mFunc;
};

}  // namespace al

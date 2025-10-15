#include "Library/Collision/CollisionPartsKeeperPtrArray.h"

#include "Library/Collision/CollisionCheckInfo.h"
#include "Library/Collision/CollisionParts.h"
#include "Library/Collision/CollisionPartsKeeperUtil.h"

namespace al {

CollisionPartsKeeperPtrArray::CollisionPartsKeeperPtrArray() = default;

s32 CollisionPartsKeeperPtrArray::checkStrikePoint(HitInfo* hitInfo,
                                                   const CollisionCheckInfoBase& checkInfo) const {
    s32 size = mCheckInfoArray->size();
    for (s32 i = 0; i < size; i++)
        if (mCheckInfoArray->at(i)->checkStrikePoint(hitInfo, checkInfo.mPos,
                                                     checkInfo.mTriFilterBase))
            return 1;
    return 0;
}

s32 CollisionPartsKeeperPtrArray::checkStrikeSphere(SphereHitResultBuffer* resultBuffer,
                                                    const SphereCheckInfo& checkInfo, bool unk,
                                                    const sead::Vector3f& vecA) const {
    s32 result = 0;
    s32 size = mCheckInfoArray->size();
    for (s32 i = 0; i < size; i++) {
        result += mCheckInfoArray->at(i)->checkStrikeSphere(
            resultBuffer, checkInfo.mPos, checkInfo.mRadius, unk, vecA, checkInfo.mTriFilterBase);
        if (resultBuffer->valA >= resultBuffer->valB)
            break;
    }
    return result;
}

s32 CollisionPartsKeeperPtrArray::checkStrikeArrow(ArrowHitResultBuffer* resultBuffer,
                                                   const ArrowCheckInfo& checkInfo) const {
    s32 result = 0;
    s32 size = mCheckInfoArray->size();
    for (s32 i = 0; i < size; i++) {
        result += mCheckInfoArray->at(i)->checkStrikeArrow(
            resultBuffer, checkInfo.mPos, *checkInfo.unk1, checkInfo.mTriFilterBase);
        if (resultBuffer->valA >= resultBuffer->valB)
            break;
    }
    return result;
}

s32 CollisionPartsKeeperPtrArray::checkStrikeSphereForPlayer(
    SphereHitResultBuffer* resultBuffer, const SphereCheckInfo& checkInfo) const {
    s32 result = 0;
    s32 size = mCheckInfoArray->size();
    for (s32 i = 0; i < size; i++) {
        result += mCheckInfoArray->at(i)->checkStrikeSphere(
            resultBuffer, checkInfo.mPos, checkInfo.mRadius, false, sead::Vector3f::zero,
            checkInfo.mTriFilterBase);
        if (resultBuffer->valA >= resultBuffer->valB)
            break;
    }
    return result;
}

s32 CollisionPartsKeeperPtrArray::checkStrikeDisk(DiskHitResultBuffer* resultBuffer,
                                                  const DiskCheckInfo& checkInfo) const {
    return 0;
}

void CollisionPartsKeeperPtrArray::searchWithSphere(
    const SphereCheckInfo& checkInfo, sead::IDelegate1<CollisionParts*>& callback) const {
    s32 size = mCheckInfoArray->size();
    for (s32 i = 0; i < size; i++) {
        if(!alCollisionUtil::isFarAway(*mCheckInfoArray->at(i),checkInfo.mPos, checkInfo.mRadius)){
            callback.invoke(mCheckInfoArray->at(i));
        }
    }
}

void CollisionPartsKeeperPtrArray::endInit() {}

void CollisionPartsKeeperPtrArray::addCollisionParts(CollisionParts* parts) {}

void CollisionPartsKeeperPtrArray::connectToCollisionPartsList(CollisionParts* parts) {}

void CollisionPartsKeeperPtrArray::disconnectToCollisionPartsList(CollisionParts* parts) {}

void CollisionPartsKeeperPtrArray::resetToCollisionPartsList(CollisionParts* parts) {}

void CollisionPartsKeeperPtrArray::movement() {}

}  // namespace al

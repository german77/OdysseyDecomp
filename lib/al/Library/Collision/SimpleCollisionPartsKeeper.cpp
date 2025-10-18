#include "Library/Collision/SimpleCollisionPartsKeeper.h"

#include "Library/Collision/CollisionParts.h"
namespace al {

SimpleCollisionPartsKeeper* createSimpleCollisionPartsKeeper() {
    return new SimpleCollisionPartsKeeper();
}

SimpleCollisionPartsKeeper::SimpleCollisionPartsKeeper() {}

void SimpleCollisionPartsKeeper::endInit() {}

void SimpleCollisionPartsKeeper::addCollisionParts(CollisionParts* parts) {
    mCollisionPartsList.pushFront(new sead::TListNode<CollisionParts*>(parts));
    parts->onJoinList();
    if( parts->isValidCollision()){
        mCollisionPartsList.popFront();
    }
}

void SimpleCollisionPartsKeeper::connectToCollisionPartsList(CollisionParts* parts) {}

void SimpleCollisionPartsKeeper::disconnectToCollisionPartsList(CollisionParts* parts) {}

void SimpleCollisionPartsKeeper::resetToCollisionPartsList(CollisionParts* parts) {}

s32 SimpleCollisionPartsKeeper::checkStrikePoint(HitInfo* hitInfo,
                                                 const CollisionCheckInfoBase& checkInfo) const {}

s32 SimpleCollisionPartsKeeper::checkStrikeSphere(SphereHitResultBuffer* resultBuffer,
                                                  const SphereCheckInfo& checkInfo, bool unk,
                                                  const sead::Vector3f& vecA) const {}

s32 SimpleCollisionPartsKeeper::checkStrikeArrow(ArrowHitResultBuffer* resultBuffer,
                                                 const ArrowCheckInfo& checkInfo) const {}

s32 SimpleCollisionPartsKeeper::checkStrikeSphereForPlayer(SphereHitResultBuffer* resultBuffer,
                                                           const SphereCheckInfo& checkInfo) const {
}

s32 SimpleCollisionPartsKeeper::checkStrikeDisk(DiskHitResultBuffer* resultBuffer,
                                                const DiskCheckInfo& checkInfo) const {}

void SimpleCollisionPartsKeeper::searchWithSphere(
    const SphereCheckInfo& checkInfo, sead::IDelegate1<CollisionParts*>& callback) const {}

void SimpleCollisionPartsKeeper::movement() {}

}  // namespace al

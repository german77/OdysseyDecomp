#pragma once

#include <container/seadTList.h>
#include <math/seadVector.h>

#include "Library/Collision/ICollisionPartsKeeper.h"

namespace sead {
template <typename>
class IDelegate1;
}

namespace al {
struct ArrowCheckInfo;
class ArrowHitResultBuffer;
struct CollisionCheckInfoBase;
class CollisionParts;
struct DiskCheckInfo;
class DiskHitResultBuffer;
class HitInfo;
struct SphereCheckInfo;
struct SphereHitResultBuffer;
class SimpleCollisionPartsKeeper;


SimpleCollisionPartsKeeper* createSimpleCollisionPartsKeeper();

class SimpleCollisionPartsKeeper : public ICollisionPartsKeeper {
public:
    SimpleCollisionPartsKeeper();

    void endInit() override;
    void addCollisionParts(CollisionParts* parts) override;
    void connectToCollisionPartsList(CollisionParts* parts) override;
    void disconnectToCollisionPartsList(CollisionParts* parts) override;
    void resetToCollisionPartsList(CollisionParts* parts) override;
    s32 checkStrikePoint(HitInfo* hitInfo,
                          const CollisionCheckInfoBase& checkInfo) const override;
    s32 checkStrikeSphere(SphereHitResultBuffer* resultBuffer,
                           const SphereCheckInfo& checkInfo, bool unk,
                           const sead::Vector3f& vecA) const override;
    s32 checkStrikeArrow(ArrowHitResultBuffer* resultBuffer,
                          const ArrowCheckInfo& checkInfo) const override;
    s32 checkStrikeSphereForPlayer(SphereHitResultBuffer* resultBuffer,
                                    const SphereCheckInfo& checkInfo) const override;
    s32 checkStrikeDisk(DiskHitResultBuffer* resultBuffer,
                         const DiskCheckInfo& checkInfo) const override;
    void searchWithSphere(const SphereCheckInfo& checkInfo,
                          sead::IDelegate1<CollisionParts*>& callback) const override;
    void movement() override;

private:
    sead::TList<CollisionParts*> mCollisionPartsList;
};

static_assert(sizeof(SimpleCollisionPartsKeeper) == 0x20);


}  // namespace al

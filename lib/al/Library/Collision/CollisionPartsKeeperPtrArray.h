#pragma once

#include <container/seadPtrArray.h>
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

class CollisionPartsKeeperPtrArray : public ICollisionPartsKeeper {
public:
    CollisionPartsKeeperPtrArray();

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
    void endInit() override;
    void addCollisionParts(CollisionParts* parts) override;
    void connectToCollisionPartsList(CollisionParts* parts) override;
    void disconnectToCollisionPartsList(CollisionParts* parts) override;
    void resetToCollisionPartsList(CollisionParts* parts) override;
    void movement() override;

private:
    sead::PtrArray<CollisionParts>* mCheckInfoArray = nullptr;
};

}  // namespace al

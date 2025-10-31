#include "Library/Screen/ScreenPointerUtil.h"

#include "Library/LiveActor/ActorInitInfo.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/Math/MathUtil.h"
#include "Library/Screen/ScreenPointDirector.h"
#include "Library/Screen/ScreenPointTarget.h"
#include "Library/Screen/ScreenPointUtil.h"

namespace al {

ScreenPointer::ScreenPointer(const al::ActorInitInfo& info, const char* name) {
    targetHitInfo.allocBuffer(0x400, nullptr);
    mDirector = info.screenPointDirector;
}

bool ScreenPointer::hitCheckSegment(const sead::Vector3f& a, const sead::Vector3f& b) {
    return mDirector->hitCheckSegment(this, &targetHitInfo, 0x400, a, b);
}

bool ScreenPointer::hitCheckScreenCircle(const sead::Vector2f& a, f32 b, f32 c) {
    return mDirector->hitCheckScreenCircle(this, &targetHitInfo, 0x400, a, b, c);
}

bool ScreenPointer::hitCheckLayoutCircle(const sead::Vector2f& a, f32 b, f32 c,
                                         s32 (*d)(const ScreenPointTargetHitInfo*,
                                                  const ScreenPointTargetHitInfo*)) {
    return mDirector->hitCheckLayoutCircle(this, &targetHitInfo, 0x400, a, b, c, d);
}

bool ScreenPointer::recheckAndSortSegment(const sead::Vector3f& a, const sead::Vector3f& b) {
    s32 size = targetHitInfo.size();
    for (s32 i = 0; i < size; i++) {
        ScreenPointTarget* target = targetHitInfo[i]->target;
        f32 targetRadius = target->getTargetRadius();

        if ((target->getTargetPos() - a).length() <= targetRadius)
            continue;

        sead::Vector3f aa = sead::Vector3f::zero;
        sead::Vector3f nn = sead::Vector3f::zero;
        bool s =
            checkHitSegmentSphereNearDepth(target->getTargetPos(), a, b, targetRadius, &aa, &nn);

        sead::Vector3f closestSegmentPoint;
        calcClosestSegmentPoint(&closestSegmentPoint, a, b, target->getTargetPos());

        targetHitInfo[i]->screenPoint =
            (target->getTargetPos() - closestSegmentPoint).length() - targetRadius;

        if (s)
            targetHitInfo[i]->directPoint = (aa - a).length();
        else
            targetHitInfo[i]->directPoint = (closestSegmentPoint - a).length();

        targetHitInfo[i]->aa.set(aa);
        targetHitInfo[i]->bb.set(nn);
    }

    targetHitInfo.sort(compareScreenPointTargetPriorDirectPoint);
    return true;
}

ScreenPointTarget* ScreenPointer::getHitTarget(s32 index) const {
    return targetHitInfo(index)->target;
}

bool ScreenPointer::isHitTarget(const ScreenPointTarget* target) const {
    for (s32 i = 0; i < targetHitInfo.size(); i++)
        if (getHitTarget(i) == target)
            return true;

    return false;
}

}  // namespace al

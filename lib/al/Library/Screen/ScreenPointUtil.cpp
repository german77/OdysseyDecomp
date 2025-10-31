#include "Library/Screen/ScreenPointUtil.h"

#include "Library/LiveActor/ActorInitInfo.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/Screen/ScreenPoint.h"
#include "Library/Screen/ScreenPointDirector.h"
#include "Library/Screen/ScreenPointKeeper.h"
#include "Library/Screen/ScreenPointTarget.h"

namespace al {

s32 compareScreenPointTarget(const ScreenPointTargetHitInfo* targetHitInfoA,
                             const ScreenPointTargetHitInfo* targetHitInfoB) {
    if (targetHitInfoA->directPoint - targetHitInfoB->directPoint < 0.0f)
        return -1;
    if (targetHitInfoA->directPoint - targetHitInfoB->directPoint > 0.0f)
        return 1;

    return targetHitInfoA->screenPoint < targetHitInfoB->screenPoint ? -1 :
           targetHitInfoA->screenPoint > targetHitInfoB->screenPoint ? 1 :
                                                                       0;
}

s32 compareScreenPointTargetPriorDirectPoint(const ScreenPointTargetHitInfo* targetHitInfoA,
                                             const ScreenPointTargetHitInfo* targetHitInfoB) {
    f32 AA = targetHitInfoA->screenPoint;
    f32 BB = targetHitInfoB->screenPoint;

    if (BB > 0.0f && AA <= 0.0f)
        return -1;
    if (BB <= 0.0f && AA > 0.0f)
        return 1;

    f32 NN = targetHitInfoA->directPoint;
    f32 CC = targetHitInfoB->directPoint;
    f32 diffb = NN - CC;
    if (AA <= 0.0f && BB < 0.0f) {
        if (diffb < 0.0f)
            return -1;
        if (diffb > 0.0f)
            return 1;
        if (AA < BB)
            return -1;
        return BB < AA ? 1 : 0;
    }

    if (AA < BB)
        return -1;
    if (AA > BB)
        return 1;
    if (diffb < 0.0f)
        return -1;
    if (diffb > 0.0f)
        return 1;

    return 0;
}

bool isExistScreenPointTargetKeeper(LiveActor* actor) {
    return actor->getScreenPointKeeper();
}

bool isScreenPointTargetArrayFull(LiveActor* actor) {
    return actor->getScreenPointKeeper()->isTargetArrayFull();
}

bool isExistScreenPointTarget(LiveActor* actor, const char* name) {
    return actor->getScreenPointKeeper()->isExistTarget(name);
}

ScreenPointTarget* addScreenPointTarget(LiveActor* actor, const ActorInitInfo& initInfo,
                                        const char* targetName, f32 radius, const char* jointName,
                                        const sead::Vector3f& vb) {
    ScreenPointTarget* target = actor->getScreenPointKeeper()->addTarget(
        actor, initInfo, targetName, radius, getTransPtr(actor), jointName, vb);

    ScreenPointDirector* director = initInfo.screenPointDirector;
    director->registerTarget(target);
    director->setCheckGroup(target);
    return target;
}

bool hitCheckSegmentScreenPointTarget(ScreenPointer* screenPointer, const sead::Vector3f& a,
                                      const sead::Vector3f& b) {
    return screenPointer->hitCheckSegment(a, b);
}

bool hitCheckScreenCircleScreenPointTarget(ScreenPointer* screenPointer, const sead::Vector2f& a,
                                           f32 b, f32 c) {
    return screenPointer->hitCheckScreenCircle(a, b, c);
}

bool hitCheckLayoutCircleScreenPointTarget(ScreenPointer* screenPointer, const sead::Vector2f& a,
                                           f32 b, f32 c,
                                           s32 (*d)(const ScreenPointTargetHitInfo*,
                                                    const ScreenPointTargetHitInfo*)) {
    return screenPointer->hitCheckLayoutCircle(a, b, c, d);
}

bool isHitScreenPointTarget(ScreenPointer* screenPointer, const ScreenPointTarget* target) {
    return screenPointer->isHitTarget(target);
}

void sendMsgScreenPointTarget(const SensorMsg& message, ScreenPointer* screenPointer,
                              ScreenPointTarget* target) {
    target->getActor()->receiveMsgScreenPoint(&message, screenPointer, target);
}

s32 getHitTargetNum(ScreenPointer* screenPointer) {
    return screenPointer->getHitTargetNum();
}

const sead::Vector3f& getHitTargetPos(ScreenPointer* screenPointer, s32 index) {
    return screenPointer->getHitTarget(index)->getTargetPos();
}

f32 getHitTargetRadius(ScreenPointer* screenPointer, s32 index) {
    return screenPointer->getHitTarget(index)->getTargetRadius();
}

}  // namespace al

namespace alScreenPointFunction {

void updateScreenPointAll(al::LiveActor* actor) {
    actor->getScreenPointKeeper()->update();
}

}  // namespace alScreenPointFunction

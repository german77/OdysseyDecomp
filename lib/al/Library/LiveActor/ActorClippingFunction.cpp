#include "Library/LiveActor/ActorClippingFunction.h"

#include "Library/Audio/System/AudioKeeper.h"
#include "Library/Clipping/ClippingActorHolder.h"
#include "Library/Clipping/ClippingActorInfo.h"
#include "Library/Clipping/ClippingJudge.h"
#include "Library/Collision/CollisionPartsKeeperUtil.h"
#include "Library/Effect/EffectKeeper.h"
#include "Library/Execute/ExecuteUtil.h"
#include "Library/HitSensor/HitSensorKeeper.h"
#include "Library/HitSensor/SensorFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorInitInfo.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorSceneInfo.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/LiveActor/LiveActorFlag.h"
#include "Library/Obj/PlacementClippingExpander.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Placement/PlacementInfo.h"
#include "Library/Shadow/ActorShadowUtil.h"
#include "Project/Clipping/ClippingDirector.h"

namespace al {

void initActorClipping(LiveActor* actor, const ActorInitInfo& initInfo) {
    actor->getSceneInfo()->clippingDirector->registerActor(actor, initInfo.viewIdHolder);
}

void initGroupClipping(LiveActor* actor, const ActorInitInfo& initInfo) {
    actor->getSceneInfo()->clippingDirector->addToGroupClipping(actor, initInfo);
}

f32 getClippingRadius(const LiveActor* actor) {
    return actor->getSceneInfo()->clippingDirector->getClippingActorHolder()->getClippingRadius(
        actor);
}

void setClippingInfo(LiveActor* actor, f32 radius, const sead::Vector3f* pos) {
    return actor->getSceneInfo()->clippingDirector->getClippingActorHolder()->setTypeToSphere(
        actor, radius, pos);
}

void setClippingObb(LiveActor* actor, const sead::BoundBox3f& boundingBox) {
    return actor->getSceneInfo()->clippingDirector->getClippingActorHolder()->setTypeToObb(
        actor, boundingBox);
}

const sead::BoundBox3f& getClippingObb(LiveActor* actor) {
    return actor->getSceneInfo()->clippingDirector->getClippingActorHolder()->getClippingObb(actor);
}

const sead::Vector3f& getClippingCenterPos(const LiveActor* actor) {
    return actor->getSceneInfo()->clippingDirector->getClippingActorHolder()->getClippingCenterPos(
        actor);
}

void setClippingNearDistance(LiveActor* actor, f32 near) {
    return actor->getSceneInfo()->clippingDirector->getClippingActorHolder()->setNearClipDistance(
        actor, near);
}

void expandClippingRadiusByShadowLength(LiveActor* actor, sead::Vector3f* pos, f32 radius) {
    sead::Vector3f trans = getTrans(actor);
    f32 clippingRadius =
        actor->getSceneInfo()->clippingDirector->getClippingActorHolder()->getClippingRadius(actor);
    if (clippingRadius >= radius)
        return;

    if (pos) {
        radius = (clippingRadius + radius) / 2.0f;
        *pos = trans + getGravity(actor) * (radius - clippingRadius);
        setClippingInfo(actor, radius, pos);
    } else {
        setClippingInfo(actor, sead::Mathf::clampMin(radius, clippingRadius), nullptr);
    }
}

bool tryExpandClippingToGround(LiveActor* actor, sead::Vector3f* pos, f32 radius) {
    f32 clippingRadius =
        actor->getSceneInfo()->clippingDirector->getClippingActorHolder()->getClippingRadius(actor);
    sead::Vector3f hitPos = sead::Vector3f::zero;
    sead::Vector3f trans = getTrans(actor);
    sead::Vector3f gravity = getGravity(actor) * radius;

    if (!alCollisionUtil::getFirstPolyOnArrow(actor, &hitPos, nullptr, trans, gravity, nullptr,
                                              nullptr)) {
        return false;
    }
    f32 distance = (hitPos - trans).length();
    if (distance > clippingRadius)
        return false;
    if (distance > radius)
        return false;

    radius = (clippingRadius + distance) * 0.5;
    *pos = trans + (hitPos - trans) * ((distance - radius) / distance);

    setClippingInfo(actor, radius, pos);
    return true;
}

bool tryExpandClippingByShadowMaskLength(LiveActor* actor, sead::Vector3f* pos) {
    if (isExistShadow(actor)) {
        expandClippingRadiusByShadowLength(actor, pos, getShadowMaskDropLengthMax(actor));
        return true;
    }
    return false;
}

bool tryExpandClippingByDepthShadowLength(LiveActor* actor, sead::Vector3f*) {}

bool tryExpandClippingByExpandObject(LiveActor* actor, const ActorInitInfo& initInfo) {
    PlacementInfo linksInfo;
    if (tryGetLinksInfo(&linksInfo, initInfo, "ClippingExpander")) {
        PlacementClippingExpander* clippingExpander = new PlacementClippingExpander();
        clippingExpander->init(actor, linksInfo);
        return true;
    }
    return false;
}

bool isClipped(const LiveActor* actor) {
    return actor->getFlags()->isClipped;
}

bool isInvalidClipping(const LiveActor* actor) {
    return actor->getFlags()->isClippingInvalid;
}

void invalidateClipping(LiveActor* actor) {
    if (actor->getFlags()->isClipped)
        actor->endClipped();

    if (!actor->getFlags()->isClippingInvalid)
        actor->getSceneInfo()->clippingDirector->getClippingActorHolder()->invalidateClipping(
            actor);
}

void validateClipping(LiveActor* actor) {
    if (actor->getFlags()->isClippingInvalid)
        actor->getSceneInfo()->clippingDirector->getClippingActorHolder()->validateClipping(actor);
}

void onDrawClipping(LiveActor* actor) {
    actor->getFlags()->isDrawClipped = true;
    if (!actor->getFlags()->isClipped)
        return;

    alActorSystemFunction::addToExecutorMovement(actor);
    if (actor->getHitSensorKeeper()) {
        actor->getHitSensorKeeper()->validateBySystem();
        alSensorFunction::updateHitSensorsAll(actor);
    }
    if (actor->getEffectKeeper())
        actor->getEffectKeeper()->onCalcAndDraw();

    if (actor->getAudioKeeper())
        actor->getAudioKeeper()->startClipped();
}

void offDrawClipping(LiveActor* actor) {
    actor->getFlags()->isDrawClipped = false;
    if (!actor->getFlags()->isClipped)
        return;

    alActorSystemFunction::removeFromExecutorMovement(actor);
    if (actor->getHitSensorKeeper())
        actor->getHitSensorKeeper()->invalidateBySystem();

    if (actor->getEffectKeeper())
        actor->getEffectKeeper()->offCalcAndDraw();

    if (actor->getAudioKeeper())
        actor->getAudioKeeper()->endClipped();
}

void onGroupClipping(LiveActor* actor) {
    actor->getSceneInfo()->clippingDirector->onGroupClipping(actor);
}

void offGroupClipping(LiveActor* actor) {
    actor->getSceneInfo()->clippingDirector->offGroupClipping(actor);
}

bool isInClippingFrustum(const LiveActor* actor, const sead::Vector3f& pos, f32 idx, f32 idy,
                         s32 idz) {
    return isInClippingFrustum(actor->getSceneInfo()->clippingDirector, pos, idx, idy, idz);
}

bool isInClippingFrustum(const ClippingDirector* director, const sead::Vector3f& pos, f32 idx,
                         f32 idy, s32 idz) {
    return director->getClippingJudge()->isInClipFrustum(pos, idx, idy, idz);
}

bool isInClippingFrustumAllView(const LiveActor* actor, const sead::Vector3f& pos, f32 radius,
                                f32 idy) {
    return actor->getSceneInfo()->clippingDirector->getClippingJudge()->isInClipFrustumAllView(
        pos, radius, idy);
}

}  // namespace al

namespace alActorFunction {

void invalidateFarClipping(al::LiveActor* actor) {
    actor->getSceneInfo()->clippingDirector->getClippingActorHolder()->setFarClipLevelMax(actor);
}

void validateFarClipping(al::LiveActor* actor) {
    actor->getSceneInfo()->clippingDirector->getClippingActorHolder()->setFarClipLevel20M(actor);
}

f32 getFarClipDistance(const al::LiveActor* actor) {
    return actor->getSceneInfo()->clippingDirector->getFarClipDistance();
}

bool isInvalidFarClipping(const al::LiveActor* actor) {
    return actor->getSceneInfo()
        ->clippingDirector->getClippingActorHolder()
        ->find(actor)
        ->isFarClipLevelMax();
}

bool isDrawClipping(const al::LiveActor* actor) {
    return actor->getFlags()->isDrawClipped;
}

bool checkActiveViewGroupAny(const al::LiveActor* actor) {
    return actor->getSceneInfo()
        ->clippingDirector->getClippingActorHolder()
        ->find(actor)
        ->checkActiveViewGroupAny();
}

}  // namespace alActorFunction

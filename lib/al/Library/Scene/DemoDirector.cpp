#include "Library/Scene/DemoDirector.h"

#include "Library/Effect/EffectSystem.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/LiveActor/LiveActorFunction.h"
#include "Library/Placement/PlacementId.h"

namespace al {

AddDemoInfo::AddDemoInfo(s32 size) : mActorListSize{size} {
    mActorList = new LiveActor*[mActorListSize];
    for (s32 i = 0; i < mActorListSize; i++)
        mActorList[i] = nullptr;
}

void AddDemoInfo::init(const PlacementId& placementId) {
    mPlacementId = new PlacementId();
    *mPlacementId = placementId;
}

void AddDemoInfo::addDemoActor(LiveActor* actor) {
    mActorList[mActorListCount] = actor;
    mActorListCount++;
}

const char* AddDemoInfo::getDemoName() const {
    return mName ?: mPlacementId->getId();
}

void AddDemoInfo::reset() {
    mName = nullptr;
    for (s32 i = 0; i < mActorListCount; i++)
        mActorList[i] = nullptr;
    mActorListCount = 0;
}

LiveActor* AddDemoInfo::getDemoActor(s32 index) const {
    return mActorList[index];
}

DemoDirector::DemoDirector(s32 size): mActorListSize{size}{
    mActorList = new LiveActor*[mActorListSize];
    mDemoInfoName=new const char*();
    mDemoInfoList=new AddDemoInfo*[10];
    for(s32 i=0;i<10;i++){
        AddDemoInfo* info= new AddDemoInfo(10);
        mDemoInfoList[i]=info;
    }
}

void DemoDirector::addDemoActorWithSubActor(LiveActor* actor) {
    tryAddDemoActor(actor);

    if (isExistSubActorKeeper(actor)) {
        s32 subActorNum = getSubActorNum(actor);
        for (s32 i = 0; i < subActorNum; i++)
            addDemoActorWithSubActor(getSubActor(actor, i));
    }
}

void DemoDirector::addDemoActor(LiveActor* actor) {
    tryAddDemoActor(actor);
}

void DemoDirector::endInit(const ActorInitInfo& actorInitInfo) {}

bool DemoDirector::isActiveDemo() const {
    return mActiveDemoName != nullptr;
}

const char* DemoDirector::getActiveDemoName() const {
    return mActiveDemoName;
}

bool DemoDirector::requestStartDemo(const char* name) {
    if (startDemo(name)) {
        mActiveDemoName = name;
        mActiveDemoTime = 0;
        return true;
    }
    return false;
}

void DemoDirector::requestEndDemo(const char* name) {
    endDemo(name);
    mActiveDemoName = nullptr;
    *mDemoInfoName = nullptr;
    mActorListCount = 0;
    _28 = nullptr;
    mLastDemoInfo = nullptr;
    mActiveDemoTime = 0;
}

bool DemoDirector::tryAddDemoActor(LiveActor* actor) {
    if (mActorListCount >= mActorListSize)
        return false;

    for (s32 i = 0; i < mActorListCount; i++)
        if (mActorList[i] == actor)
            return false;

    mActorList[mActorListCount] = actor;
    mActorListCount++;
    return true;
}

LiveActor** DemoDirector::getDemoActorList() const {
    return mActorList;
}

s32 DemoDirector::getDemoActorNum() const {
    return mActorListCount;
}

void DemoDirector::updateDemoActor(EffectSystem* effectSystem) {
    if (isActiveDemo())
        mActiveDemoTime++;
    for (s32 i = 0; i < mActorListCount; i++) {
        if (effectSystem) {
            LiveActor* actor = mActorList[i];
            if (isAlive(actor)) {
                actor->movement();
                if (actor->getModelKeeper())
                    actor->calcAnim();
            }
            if (actor->getEffectKeeper())
                effectSystem->addCalcEffect((u64)actor->getEffectKeeper());
        } else {
            LiveActor* actor = mActorList[i];
            if (isAlive(actor)) {
                actor->movement();
                if (actor->getModelKeeper())
                    actor->calcAnim();
            }
        }
    }
}

AddDemoInfo* DemoDirector::registDemoRequesterToAddDemoInfo(const PlacementId& placementId) {
    return findOrCreateAddDemoInfo(placementId);
}

AddDemoInfo* DemoDirector::findOrCreateAddDemoInfo(const PlacementId& placementId) {}

void DemoDirector::registActorToAddDemoInfo(LiveActor* actor, const PlacementId& placementId) {
    findOrCreateAddDemoInfo(placementId)->addDemoActor(actor);
}

static s32 blackBox(s32 value) {
    __asm__("" : "+r"(value));
    return value;
}

AddDemoInfo* DemoDirector::tryFindAddDemoInfo(const PlacementId& placementId) const {
    for (s32 i = 0; i < mDemoInfoListCount; i++) {
        PlacementId* demoPlacementId = mDemoInfoList[i]->mPlacementId;
        s32 value;
        if (demoPlacementId != nullptr){
            bool isInVolume = demoPlacementId->isEqual(placementId);
            value = isInVolume;
        }else{
            value=4;
        }

        if ((blackBox(value | 4) & 7) != 4)
            break;
    }
    return nullptr;

}


AddDemoInfo* DemoDirector::findAddDemoInfo(const PlacementId& placementId) const {
    for (s32 i = 0; i < mDemoInfoListCount; i++) {
        PlacementId* demoPlacementId = mDemoInfoList[i]->mPlacementId;
        s32 value;
        if (demoPlacementId != nullptr){
            bool isInVolume = demoPlacementId->isEqual(placementId);
            value = isInVolume;
        }else{
            value=4;
        }

        if ((blackBox(value | 4) & 7) != 4)
            break;
    }
    return nullptr;
}

bool DemoDirector::startDemo(const char* name) {
    return true;
}

void DemoDirector::endDemo(const char* name) {}

}  // namespace al

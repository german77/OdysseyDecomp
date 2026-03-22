#pragma once

#include "Library/LiveActor/LiveActor.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace al {
class IUseSceneObjHolder;
class PlacementId;
}  // namespace al

class GrowPlantGrowPlace;
class GrowPlantSeed;

class GrowPlantDirector : public al::LiveActor, public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_GrowPlantDirector;

    GrowPlantDirector();

    void initAfterPlacementSceneObj(const al::ActorInitInfo& info) override;
    void registerGrowPlantPlace(GrowPlantGrowPlace*, f32);
    void registerGrowPlantSeed(GrowPlantSeed*);
    void registerRouteGuidePointForPlace(GrowPlantGrowPlace*);
    bool isPlantEnd(const al::PlacementId&) const;
    bool isPlantEndAll() const;
    void updateGuideWindow();
    void control() override;
    void updateRouteGuideGrowPlant();
    void invalidateGuideAll();
    bool isClimb() const;
    bool isGrowPlantAlready() const;
    void onPlaceAlreadyGuide();
    bool isHoldSeed() const;
    void onPlaceAllGuide();
    void onGrowSeedGuide();

private:
    s8 filler[0x3A0 - sizeof(al::LiveActor) - sizeof(al::ISceneObj)];
};

static_assert(sizeof(GrowPlantDirector) == 0x3A0);

namespace rs {
void registerGrowPlantPlace(GrowPlantGrowPlace*, f32);
void registerRouteGuidePointForPlace(GrowPlantGrowPlace*);
void registerGrowPlantSeed(GrowPlantSeed*);
bool isPlantEnd(const al::PlacementId&, const al::IUseSceneObjHolder*);
bool isPlantEndAll(const al::IUseSceneObjHolder*);
void updateRouteGuideGrowPlant(const al::IUseSceneObjHolder*);
}  // namespace rs

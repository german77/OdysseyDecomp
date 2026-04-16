#pragma once

#include <math/seadQuat.h>
#include <math/seadVector.h>

#include "Library/LiveActor/LiveActor.h"
#include "Library/Scene/ISceneObj.h"

#include "Npc/SessionMusicianType.h"
#include "Scene/SceneObjFactory.h"

class SessionMusicianNpc;
class SessionMayorNpc;

namespace al {
struct ActorInitInfo;
class PlacementInfo;
}  // namespace al

class SessionMusicianManager : public al::LiveActor, public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_SessionMusicianManager;

    SessionMusicianManager(const char*);
    void initAfterPlacementSceneObj(const al::ActorInitInfo&) override;
    void entryMusician(SessionMusicianNpc*);
    bool isJoinedMusician() const;
    void getJoinedMusician() const;
    bool isSubscribed(SessionMusicianType) const;
    void tryAppearPowerPlant();
    void findPowerPlant() const;
    void tryStartWarp(al::PlacementInfo*);
    void addDemoAllMusicians();

    const char* getSceneObjName() const override { return "SessionMusicianManager"; }
    void initSceneObj() override {}

    void exeWait();
    void exeComplete();
};

namespace SessionMusicianLocalFunction {
void tryCreateSessionMusicianManager(const al::IUseSceneObjHolder*);
SessionMusicianManager* getSessionMusicianManager(const al::IUseSceneObjHolder*);
bool isExistSessionMusicianManager(const al::IUseSceneObjHolder*);
void tryStartWarpToSessionMayor(const al::IUseSceneObjHolder*, al::PlacementInfo*);
void entrySessionMayorToManager(SessionMayorNpc*);
bool isJoinedSessionMusician(const al::IUseSceneObjHolder*);
void tryGetJoinedSessionMusicanActor(const al::IUseSceneObjHolder*);
void tryAddJoinedSessionMusicianDemoActor(const al::IUseSceneObjHolder*);
void tryGetSessionMoonGetDemoPlayerPos(sead::Vector3f*, const al::IUseSceneObjHolder*);
void tryGetSessionMoonGetDemoPlayerPose(sead::Quatf*, const al::IUseSceneObjHolder*);
void trySetJoinedSessionMusicianTransformForMoonGetDemo(const al::IUseSceneObjHolder*);
void addDemoAllMusicians(const al::IUseSceneObjHolder*);
}  // namespace SessionMusicianLocalFunction

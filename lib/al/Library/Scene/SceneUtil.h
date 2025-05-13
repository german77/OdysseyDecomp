#pragma once

#include <basis/seadTypes.h>
#include <common/aglRenderBuffer.h>
#include <gfx/seadCamera.h>
#include <heap/seadHeap.h>

namespace al {
class ActorFactory;
struct ActorInitInfo;
class LayoutInitInfo;
class GameDataHolderBase;
class SceneMsgCtrl;
class PlacementInfo;
class Scene;
struct SceneInitInfo;
class StageInfo;
class PauseCameraCtrl;
struct AudioDirectorInitInfo;
class Projection;
class LiveActor;
class LiveActorGroup;
class AreaObjFactory;
class DemoDirector;
class ItemDirectorBase;
class SwitchOnAreaGroup;
class SwitchKeepOnAreaGroup;
class CameraPoserFactory;
class IUseAudioKeeper;
class AreaObjDirector;
class ExecuteDirector;
class PlayerHolder;
class ItemDirectorBase;
class Resource;

s32 getStageInfoMapNum(const Scene* scene);
s32 getStageInfoDesignNum(const Scene* scene);
s32 getStageInfoSoundNum(const Scene* scene);
StageInfo* getStageInfoMap(const Scene* scene, s32 index);
StageInfo* getStageInfoDesign(const Scene* scene, s32 index);
StageInfo* getStageInfoSound(const Scene* scene, s32 index);
Resource* getStageResourceMap(const Scene* scene, s32 index);
Resource* tryGetStageResourceDesign(const Scene* scene, s32 index);
Resource* tryGetStageResourceSound(const Scene* scene, s32 index);
agl::RenderBuffer* getSceneFrameBufferMain(const Scene* scene);
agl::DrawContext* getSceneDrawContext(const Scene* scene);
agl::RenderBuffer* getSceneFrameBufferConsole(const Scene* scene);
agl::RenderBuffer* getSceneFrameBufferHandheld(const Scene* scene);
f32 getSceneFrameBufferMainAspect(const Scene* scene);
bool isChangedGraphicsQualityMode(const Scene* scene);
AreaObjDirector* getSceneAreaObjDirector(const Scene* scene);
ExecuteDirector* getSceneExecuteDirector(const Scene* scene);
PlayerHolder* getScenePlayerHolder(const Scene* scene);
ItemDirectorBase* getSceneItemDirector(const Scene* scene);
void initActorInitInfo(ActorInitInfo* actorInitInfo, const Scene* scene,
                       const PlacementInfo* placementInfo, const LayoutInitInfo* layoutInfo,
                       const ActorFactory* actorFactory, SceneMsgCtrl* sceneMsgCtrl,
                       GameDataHolderBase* gameDataHolderBase);
void initLayoutInitInfo(LayoutInitInfo*, const Scene* scene, const SceneInitInfo&);
void initPlacementAreaObj(Scene* scene, const ActorInitInfo&);
void initPlacementGravityObj(Scene* scene);
bool tryGetPlacementInfoAndCount(PlacementInfo*, s32*, const StageInfo*, const char*);
void initPlacementObjectMap(Scene* scene, const ActorInitInfo&, const char*);
void initPlacementByStageInfo(const StageInfo*, const char*, const ActorInitInfo&);
void initPlacementObjectDesign(Scene* scene, const ActorInitInfo&, const char*);
void initPlacementObjectSound(Scene* scene, const ActorInitInfo&, const char*);
LiveActor* tryInitPlacementSingleObject(Scene* scene, const ActorInitInfo&, s32, const char*);
LiveActor* tryInitPlacementSingleObject(Scene* scene, const ActorInitInfo&, s32, const char*,
                                        const char*);
bool tryInitPlacementActorGroup(LiveActorGroup*, Scene* scene, const ActorInitInfo&, s32, const char*,
                                const char*);
void initPlacementByStageInfoSingle(const StageInfo*, const char*, const ActorInitInfo&);
bool tryGetPlacementInfo(PlacementInfo*, const StageInfo*, const char*);
void getPlacementInfo(PlacementInfo*, const StageInfo*, const char*);
void getPlacementInfoAndCount(PlacementInfo*, s32*, const StageInfo*, const char*);
void initAreaObjDirector(Scene* scene, const AreaObjFactory*);
void initDemoDirector(Scene* scene, DemoDirector*);
void initHitSensorDirector(Scene* scene);
void initGravityHolder(Scene* scene);
void initItemDirector(Scene* scene, ItemDirectorBase*);
void initNatureDirector(Scene* scene);
void initSwitchAreaDirector(Scene* scene, s32, s32);
void registerSwitchOnAreaGroup(Scene* scene, SwitchOnAreaGroup*);
void registerSwitchKeepOnAreaGroup(Scene* scene, SwitchKeepOnAreaGroup*);
void initGraphicsSystemInfo(Scene* scene, const char*, s32);
void initCameraDirector(Scene* scene, const char*, s32, const CameraPoserFactory*);
void initCameraDirectorWithoutStageResource(Scene* scene, const CameraPoserFactory*);
void initCameraDirectorWithoutStageResource(Scene* scene, const CameraPoserFactory*);
void initCameraDirectorFix(Scene* scene, const sead::Vector3f&, const sead::Vector3f&,
                           const CameraPoserFactory*);
void initSceneCameraFovyDegree(Scene* scene, f32);
void initSnapShotCameraAudioKeeper(Scene* scene, IUseAudioKeeper*);
void setCameraAspect(Scene* scene, f32, f32);
void resetSceneInitEntranceCamera(Scene* scene);
void stopCameraByDeathPlayer(Scene* scene);
void restartCameraByDeathPlayer(Scene* scene);
void startInvalidCameraStopJudgeByDemo(Scene* scene);
void endInvalidCameraStopJudgeByDemo(Scene* scene);
void startCameraSnapShotMode(Scene* scene, bool useInterpole);
void endCameraSnapShotMode(Scene* scene);
bool isCameraReverseInputH(const Scene* scene);
void onCameraReverseInputH(Scene* scene);
void offCameraReverseInputH(Scene* scene);
bool isCameraReverseInputV(const Scene* scene);
void onCameraReverseInputV(Scene* scene);
void offCameraReverseInputV(Scene* scene);
s32 getCameraStickSensitivityLevel(const Scene* scene);
void setCameraStickSensitivityLevel(Scene* scene, s32);
bool isValidCameraGyro(const Scene* scene);
void validateCameraGyro(Scene* scene);
void invalidateCameraGyro(Scene* scene);
s32 getCameraGyroSensitivityLevel(const Scene* scene);
void setCameraGyroSensitivityLevel(Scene* scene, s32);
PauseCameraCtrl* initAndCreatePauseCameraCtrl(Scene* scene, f32);
void startCameraPause(PauseCameraCtrl*);
void endCameraPause(PauseCameraCtrl*);
void initAudioDirector2D(Scene* scene, const SceneInitInfo&, AudioDirectorInitInfo&);
void initAudioDirector3D(Scene* scene, const SceneInitInfo&, AudioDirectorInitInfo&);
void initAudioDirector3D(Scene* scene, const SceneInitInfo&, AudioDirectorInitInfo&,
                         const sead::LookAtCamera*, const Projection*, AreaObjDirector*);
void initSceneAudioKeeper(Scene* scene, const SceneInitInfo&, const char*);
void setIsSafeFinalizingInParallelThread(Scene* scene, bool);
void updateKit(Scene* scene);
void updateKitTable(Scene* scene, const char*);
void updateKitList(Scene* scene, const char*, const char*);
void updateKitList(Scene* scene, const char*);
void updateLayoutKit(Scene* scene);
void updateEffect(Scene* scene);
void updateEffectSystem(Scene* scene);
void updateEffectPlayer(Scene* scene);
void updateEffectDemo(Scene* scene);
void updateEffectDemoWithPause(Scene* scene);
void updateEffectLayout(Scene* scene);
void updateGraphicsPrev(Scene* scene);
void updateKitListPrev(Scene* scene);
void updateKitListPost(Scene* scene);
void updateKitListPostDemoWithPauseNormalEffect(Scene* scene);
void updateKitListPostOnNerveEnd(Scene* scene);
void drawKit(const Scene* scene, const char*);
void drawKitList(const Scene* scene, const char*, const char*);
void drawLayoutKit(const Scene* scene, const char*);
void drawEffectDeferred(const Scene* scene, s32);
void startForwardPlayerScreenFader(const Scene* scene, s32, s32, f32);
void endForwardPlayerScreenFader(const Scene* scene, s32);
bool isStopScene(const Scene* scene);
void startCheckViewCtrlByCameraPos(Scene* scene);
void startCheckViewCtrlByLookAtPos(Scene* scene);
void startCheckViewCtrlByPlayerPos(Scene* scene);
bool isExistScreenCoverCtrl(const Scene* scene);
bool isRequestCaptureScreenCover(const Scene* scene);
void resetRequestCaptureScreenCover(const Scene* scene);
bool isOffDrawScreenCover(const Scene* scene);
void resetCaptureScreenCover(const Scene* scene);
void validatePostProcessingFilter(const Scene* scene);
void invalidatePostProcessingFilter(const Scene* scene);
void incrementPostProcessingFilterPreset(const Scene* scene);
void decrementPostProcessingFilterPreset(const Scene* scene);
s32 getPostProcessingFilterPresetId(const Scene* scene);
bool isActiveDemo(const Scene* scene);
const char* getActiveDemoName(const Scene* scene);
LiveActor* getDemoActorList(const Scene* scene);
s32 getDemoActorNum(const Scene* scene);
void updateDemoActor(const Scene* scene);
void updateDemoActorForPauseEffect(const Scene* scene);
void stopAllSe(const Scene* scene, u32);
void initPadRumble(const Scene* scene, const SceneInitInfo&);
void stopPadRumble(const Scene* scene);
void pausePadRumble(const Scene* scene);
void endPausePadRumble(const Scene* scene);
void validatePadRumble(Scene* scene);
void invalidatePadRumble(Scene* scene);
void setPadRumblePowerLevel(Scene* scene, s32);
const Resource* getPreLoadFileListArc();
bool tryRequestPreLoadFile(const Scene* scene, const SceneInitInfo&, s32, sead::Heap*);
}  // namespace al

namespace alSceneFunction {
void initAreaCameraSwitcherMultiForPrototype(const al::Scene*);
}

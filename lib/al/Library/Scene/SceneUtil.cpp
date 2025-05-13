#include "Library/Scene/SceneUtil.h"

#include <common/aglRenderBuffer.h>
#include <math/seadVector.h>

#include "Library/Draw/GraphicsQualityController.h"
#include "Library/Draw/GraphicsSystemInfo.h"
#include "Library/Effect/EffectSystem.h"
#include "Library/LiveActor/LiveActorKit.h"
#include "Library/Model/ModelDrawBufferUpdater.h"
#include "Library/Scene/Scene.h"
#include "Library/Stage/StageResourceKeeper.h"
#include "Library/Stage/StageResourceList.h"
#include "Library/System/GameSystemInfo.h"

namespace al {
s32 getStageInfoMapNum(const Scene* scene) {
    if (!scene->getStageResourceKeeper())
        return 0;

    StageResourceList* resourceList = scene->getStageResourceKeeper()->getMapStageInfo();
    if (!resourceList)
        return 0;

    return resourceList->getStageResourceNum();
}

s32 getStageInfoDesignNum(const Scene* scene) {
    if (!scene->getStageResourceKeeper())
        return 0;

    StageResourceList* resourceList = scene->getStageResourceKeeper()->getDesignStageInfo();
    if (!resourceList)
        return 0;

    return resourceList->getStageResourceNum();
}

s32 getStageInfoSoundNum(const Scene* scene) {
    if (!scene->getStageResourceKeeper())
        return 0;

    StageResourceList* resourceList = scene->getStageResourceKeeper()->getSoundStageInfo();
    if (!resourceList)
        return 0;

    return resourceList->getStageResourceNum();
}

StageInfo* getStageInfoMap(const Scene* scene, s32 index) {
    return scene->getStageResourceKeeper()->getMapStageInfo()->getStageInfo(index);
}

StageInfo* getStageInfoDesign(const Scene* scene, s32 index) {
    return scene->getStageResourceKeeper()->getDesignStageInfo()->getStageInfo(index);
}

StageInfo* getStageInfoSound(const Scene* scene, s32 index) {
    return scene->getStageResourceKeeper()->getSoundStageInfo()->getStageInfo(index);
}

Resource* getStageResourceMap(const Scene* scene, s32 index) {
    return getStageInfoMap(scene, index)->getResource();
}

Resource* tryGetStageResourceDesign(const Scene* scene, s32 index) {
    if (getStageInfoDesignNum(scene) == 0)
        return nullptr;
    return scene->getStageResourceKeeper()
        ->getDesignStageInfo()
        ->getStageInfo(index)
        ->getResource();
}

Resource* tryGetStageResourceSound(const Scene* scene, s32 index) {
    if (getStageInfoSoundNum(scene) == 0)
        return nullptr;
    return scene->getStageResourceKeeper()->getSoundStageInfo()->getStageInfo(index)->getResource();
}

agl::RenderBuffer* getSceneFrameBufferMain(const Scene* scene) {
    DrawSystemInfo* drawSystemInfo = scene->getDrawSystemInfo();
    return drawSystemInfo->isDocked ? drawSystemInfo->dockedRenderBuffer :
                                      drawSystemInfo->handheldRenderBuffer;
}

agl::DrawContext* getSceneDrawContext(const Scene* scene) {
    return scene->getDrawSystemInfo()->drawContext;
}

agl::RenderBuffer* getSceneFrameBufferConsole(const Scene* scene) {
    return scene->getDrawSystemInfo()->dockedRenderBuffer;
}

agl::RenderBuffer* getSceneFrameBufferHandheld(const Scene* scene) {
    return scene->getDrawSystemInfo()->handheldRenderBuffer;
}

f32 getSceneFrameBufferMainAspect(const Scene* scene) {
    const sead::Vector2f& virtualSize = getSceneFrameBufferMain(scene)->getVirtualSize();

    return virtualSize.x / virtualSize.y;
}

bool isChangedGraphicsQualityMode(const Scene* scene) {
    return scene->getLiveActorKit()
        ->getGraphicsSystemInfo()
        ->getGraphicsQualityController()
        ->isChangedGraphicsQualityMode();
}

AreaObjDirector* getSceneAreaObjDirector(const Scene* scene) {
    return scene->getLiveActorKit()->getAreaObjDirector();
}

ExecuteDirector* getSceneExecuteDirector(const Scene* scene) {
    return scene->getLiveActorKit()->getExecuteDirector();
}

PlayerHolder* getScenePlayerHolder(const Scene* scene) {
    return scene->getLiveActorKit()->getPlayerHolder();
}

ItemDirectorBase* getSceneItemDirector(const Scene* scene) {
    return scene->getLiveActorKit()->getItemDirector();
}

void initActorInitInfo(ActorInitInfo* actorInitInfo, const Scene* scene,
                       const PlacementInfo* placementInfo, const LayoutInitInfo* layoutInfo,
                       const ActorFactory* actorFactory, SceneMsgCtrl* sceneMsgCtrl,
                       GameDataHolderBase* gameDataHolderBase) {
    AudioDirector* audioDirector = scene->getAudioDirector();
    LiveActorKit* actorKit = scene->getLiveActorKit();
    ActorResourceHolder* actorResourceHolder = actorKit->getActorResourceHolder();
    AreaObjDirector* areaObjDirector = actorKit->getAreaObjDirector();
    CameraDirector* cameraDirector = actorKit->getCameraDirector();
    ClippingDirector* clippingDirector = actorKit->getClippingDirector();
    LiveActorGroup* liveActorGroup = actorKit->getLiveActorGroupAllActors();
    DemoDirector* demoDirector = actorKit->getDemoDirector();
    ExecuteDirector* executeDirector = actorKit->getExecuteDirector();
    GravityHolder* gravityHolder = actorKit->getGravityHolder();
    GamePadSystem* gamePadSystem = actorKit->getGamePadSystem();
    PlayerHolder* playerHolder = actorKit->getPlayerHolder();
    HitSensorDirector* hitSensorDirector = actorKit->getHitSensorDirector();

    EffectSystem* effectSystem=actorKit->getEffectSystem();
    EffectSystemInfo* effectSystemInfo = effectSystem?effectSystem->getEffectSystemInfo():nullptr;

    CollisionDirector* collisionDirector = actorKit->getCollisionDirector();
    ItemDirectorBase* itemDirectorBase = actorKit->getItemDirector();
    PadRumbleDirector* padRumbleDirector = actorKit->getPadRumbleDirector();
    NatureDirector* natureDirector = actorKit->getNatureDirector();
    SceneObjHolder* sceneObjHolder = scene->getSceneObjHolder();

    actorInitInfo->initNew(
        placementInfo, layoutInfo, liveActorGroup, actorFactory, actorResourceHolder,
        areaObjDirector, audioDirector, cameraDirector, clippingDirector,
        collisionDirector, demoDirector, effectSystemInfo, executeDirector, gameDataHolderBase,
        gravityHolder, hitSensorDirector, itemDirectorBase, natureDirector, gamePadSystem,
        padRumbleDirector, playerHolder, sceneObjHolder, sceneMsgCtrl, scene->getSceneStopCtrl(),
        scene->getScreenCoverCtrl(), actorKit->getScreenPointDirector(),
        actorKit->getShadowDirector(), actorKit->getStageSwitchDirector(),
        actorKit->getModelGroup(), actorKit->getGraphicsSystemInfo(),
        actorKit->getModelDrawBufferUpdater()->getModelDrawBufferCounter(),
        actorKit->getDynamicDrawActorGroup());
}

void initLayoutInitInfo(LayoutInitInfo*, const Scene* scene, const SceneInitInfo&);

void initPlacementAreaObj(Scene* scene, const ActorInitInfo&);

void initPlacementGravityObj(Scene*);

bool tryGetPlacementInfoAndCount(PlacementInfo*, s32*, const StageInfo*, const char*);

void initPlacementObjectMap(Scene* scene, const ActorInitInfo&, const char*);

void initPlacementByStageInfo(const StageInfo*, const char*, const ActorInitInfo&);

void initPlacementObjectDesign(Scene* scene, const ActorInitInfo&, const char*);

void initPlacementObjectSound(Scene* scene, const ActorInitInfo&, const char*);

LiveActor* tryInitPlacementSingleObject(Scene* scene, const ActorInitInfo&, s32, const char*);

LiveActor* tryInitPlacementSingleObject(Scene* scene, const ActorInitInfo&, s32, const char*,
                                        const char*);

bool tryInitPlacementActorGroup(LiveActorGroup*, Scene* scene, const ActorInitInfo&, s32,
                                const char*, const char*);

void initPlacementByStageInfoSingle(const StageInfo*, const char*, const ActorInitInfo&);

bool tryGetPlacementInfo(PlacementInfo*, const StageInfo*, const char*);

void getPlacementInfo(PlacementInfo*, const StageInfo*, const char*);

void getPlacementInfoAndCount(PlacementInfo*, s32*, const StageInfo*, const char*);

void initAreaObjDirector(Scene* scene, const AreaObjFactory*);

void initDemoDirector(Scene* scene, DemoDirector*);

void initHitSensorDirector(Scene*);

void initGravityHolder(Scene*);

void initItemDirector(Scene* scene, ItemDirectorBase*);

void initNatureDirector(Scene*);

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

void resetSceneInitEntranceCamera(Scene*);

void stopCameraByDeathPlayer(Scene*);

void restartCameraByDeathPlayer(Scene*);

void startInvalidCameraStopJudgeByDemo(Scene*);

void endInvalidCameraStopJudgeByDemo(Scene*);

void startCameraSnapShotMode(Scene* scene, bool);

void endCameraSnapShotMode(Scene*);

bool isCameraReverseInputH(const Scene* scene);

void onCameraReverseInputH(Scene*);

void offCameraReverseInputH(Scene*);

bool isCameraReverseInputV(const Scene* scene);

void onCameraReverseInputV(Scene*);

void offCameraReverseInputV(Scene*);

s32 getCameraStickSensitivityLevel(const Scene* scene);

void setCameraStickSensitivityLevel(Scene* scene, s32);

bool isValidCameraGyro(const Scene* scene);

void validateCameraGyro(Scene*);

void invalidateCameraGyro(Scene*);

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

void updateKit(Scene*);

void updateKitTable(Scene* scene, const char*);

void updateKitList(Scene* scene, const char*, const char*);

void updateKitList(Scene* scene, const char*);

void updateLayoutKit(Scene*);

void updateEffect(Scene*);

void updateEffectSystem(Scene*);

void updateEffectPlayer(Scene*);

void updateEffectDemo(Scene*);

void updateEffectDemoWithPause(Scene*);

void updateEffectLayout(Scene*);

void updateGraphicsPrev(Scene*);

void updateKitListPrev(Scene*);

void updateKitListPost(Scene*);

void updateKitListPostDemoWithPauseNormalEffect(Scene*);

void updateKitListPostOnNerveEnd(Scene*);

void drawKit(const Scene* scene, const char*);

void drawKitList(const Scene* scene, const char*, const char*);

void drawLayoutKit(const Scene* scene, const char*);

void drawEffectDeferred(const Scene* scene, s32);

void startForwardPlayerScreenFader(const Scene* scene, s32, s32, f32);

void endForwardPlayerScreenFader(const Scene* scene, s32);

bool isStopScene(const Scene* scene);

void startCheckViewCtrlByCameraPos(Scene*);

void startCheckViewCtrlByLookAtPos(Scene*);

void startCheckViewCtrlByPlayerPos(Scene*);

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

void validatePadRumble(Scene*);

void invalidatePadRumble(Scene*);

void setPadRumblePowerLevel(Scene* scene, s32);

const Resource* getPreLoadFileListArc();

bool tryRequestPreLoadFile(const Scene* scene, const SceneInitInfo&, s32, sead::Heap*);
}  // namespace al

#include "Scene/StageScene.h"

#include <common/aglDrawContext.h>
#include <common/aglRenderBuffer.h>
#include <gfx/seadColor.h>
#include <gfx/seadFrameBuffer.h>
#include <gfx/seadViewport.h>

#include "Library/Area/AreaObj.h"
#include "Library/Area/AreaObjDirector.h"
#include "Library/Area/AreaObjGroup.h"
#include "Library/Area/AreaObjUtil.h"
#include "Library/Area/TrafficAreaDirector.h"
#include "Library/Audio/AudioDirector.h"
#include "Library/Audio/AudioDirectorInitInfo.h"
#include "Library/Audio/AudioFunction.h"
#include "Library/Audio/System/AudioKeeperFunction.h"
#include "Library/Audio/System/SimpleAudioUser.h"
#include "Library/Base/StringUtil.h"
#include "Library/Bgm/BgmLineFunction.h"
#include "Library/Bgm/BgmPlayingRequest.h"
#include "Library/Camera/CameraUtil.h"
#include "Library/Controller/InputFunction.h"
#include "Library/Controller/PadRumbleDirector.h"
#include "Library/Controller/PadRumbleFunction.h"
#include "Library/Data/DataStoreDirector.h"
#include "Library/Demo/DemoSyncedEventFunction.h"
#include "Library/Demo/DemoSyncedEventKeeper.h"
#include "Library/Demo/EventFlowExecutorHolder.h"
#include "Library/Draw/ChromakeyDrawer.h"
#include "Library/Draw/GraphicsFunction.h"
#include "Library/Draw/GraphicsSystemInfo.h"
#include "Library/Draw/SubCameraRenderer.h"
#include "Library/Draw/ViewRenderer.h"
#include "Library/Effect/EffectEnvUtil.h"
#include "Library/Effect/EffectSystem.h"
#include "Library/Effect/FootPrintServer.h"
#include "Library/Execute/ExecuteUtil.h"
#include "Library/Framework/GameFrameworkNx.h"
#include "Library/Layout/LayoutActorUtil.h"
#include "Library/Layout/LayoutInitInfo.h"
#include "Library/Layout/LayoutTextureRenderer.h"
#include "Library/Layout/LayoutUtil.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorSceneFunction.h"
#include "Library/LiveActor/LiveActorGroup.h"
#include "Library/LiveActor/LiveActorKit.h"
#include "Library/Math/MathUtil.h"
#include "Library/Math/ParabolicPath.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Network/NetworkSystem.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Placement/PlacementId.h"
#include "Library/Placement/PlacementInfo.h"
#include "Library/Play/Layout/SimpleLayoutAppearWaitEnd.h"
#include "Library/Play/Layout/WindowConfirm.h"
#include "Library/Play/Layout/WipeHolder.h"
#include "Library/Play/Layout/WipeSimple.h"
#include "Library/Player/PlayerUtil.h"
#include "Library/Projection/Projection.h"
#include "Library/Ranking/RankingDirector.h"
#include "Library/Resource/ResourceFunction.h"
#include "Library/Scene/DemoDirector.h"
#include "Library/Scene/SceneObjUtil.h"
#include "Library/Scene/SceneUtil.h"
#include "Library/Screen/ScreenFunction.h"
#include "Library/Se/SeDirector.h"
#include "Library/Se/SeFunction.h"
#include "Library/Stage/StageRhythm.h"
#include "Library/System/GameSystemInfo.h"
#include "Library/Thread/FunctorV0M.h"
#include "Project/Clipping/ClippingDirector.h"

#include "Amiibo/AmiiboNpcDirector.h"
#include "Amiibo/HelpAmiiboDirector.h"
#include "Audio/AnimeSyncDirector.h"
#include "Audio/CollectBgmPlayer.h"
#include "Audio/ProjectSeNamedList.h"
#include "Boss/GiantWanderBoss.h"
#include "Boss/Koopa/KoopaHackFunction.h"
#include "Boss/Koopa/KoopaLv1.h"
#include "Camera/ProjectCameraInput.h"
#include "Camera/SafetyPointRecoveryCameraWatcher.h"
#include "Demo/CapManHeroDemoDirector.h"
#include "Demo/OpeningStageStartDemo.h"
#include "Demo/ProjectDemoDirector.h"
#include "Event/EventDemoCtrl.h"
#include "Item/CoinCollectWatcher.h"
#include "Item/ShineChipWatcherHolder.h"
#include "Layout/CapMessageMoonNotifier.h"
#include "Layout/CinemaCaption.h"
#include "Layout/CollectBgmRegister.h"
#include "Layout/ControllerGuideSnapShotCtrl.h"
#include "Layout/InputSeparator.h"
#include "Layout/KidsModeLayoutAccessor.h"
#include "Layout/LocationNameCtrl.h"
#include "Layout/MapLayout.h"
#include "Layout/MiniGameMenu.h"
#include "Layout/PlayGuideSkip.h"
#include "Layout/StageSceneLayout.h"
#include "MapObj/CapMessageDirector.h"
#include "MapObj/CapMessageShowInfo.h"
#include "MapObj/CheckpointFlag.h"
#include "MapObj/CheckpointFlagWatcher.h"
#include "MapObj/CollectionList.h"
#include "MapObj/Doshi.h"
#include "MapObj/FukankunZoomObjHolder.h"
#include "MapObj/GrowPlantDirector.h"
#include "MapObj/PaintObjHolder.h"
#include "MapObj/PlayerStartInfo.h"
#include "MapObj/PlayerStartInfoHolder.h"
#include "MapObj/Pyramid.h"
#include "MapObj/RouteGuideDirector.h"
#include "MapObj/ShineTowerRocket.h"
#include "MapObj/ShoppingWatcher.h"
#include "ModeBalloon/TimeBalloonDirector.h"
#include "Npc/CapManHeroDemoUtil.h"
#include "Npc/EventFlowSceneExecuteCtrl.h"
#include "Npc/NpcEventCtrlInfo.h"
#include "Npc/NpcEventDirector.h"
#include "Npc/SessionMusicianManager.h"
#include "Npc/TalkNpcParamHolder.h"
#include "Npc/TalkNpcSceneEventSwitcher.h"
#include "Player/PlayerActorBase.h"
#include "Player/PlayerCameraTarget.h"
#include "Player/PlayerFactory.h"
#include "Player/PlayerFunction.h"
#include "Player/PlayerHackStartTexKeeper.h"
#include "Player/PlayerInitInfo.h"
#include "Scene/BgmAnimeSyncDirector.h"
#include "Scene/BirdGatheringSpotDirector.h"
#include "Scene/DemoSoundSynchronizer.h"
#include "Scene/GuidePosInfoHolder.h"
#include "Scene/HtmlViewerRequester.h"
#include "Scene/InformationWindowDirector.h"
#include "Scene/MoviePlayer.h"
#include "Scene/ProjectActorFactory.h"
#include "Scene/ProjectAreaFactory.h"
#include "Scene/ProjectCameraPoserFactory.h"
#include "Scene/ProjectItemDirector.h"
#include "Scene/QuestInfoHolder.h"
#include "Scene/ScenarioStartCameraHolder.h"
#include "Scene/SceneAudioSystemPauseController.h"
#include "Scene/SceneEventNotifier.h"
#include "Scene/SceneObjFactory.h"
#include "Scene/StageSceneStateCarryMeat.h"
#include "Scene/StageSceneStateCheckpointWarp.h"
#include "Scene/StageSceneStateCloset.h"
#include "Scene/StageSceneStateCollectBgm.h"
#include "Scene/StageSceneStateCollectionList.h"
#include "Scene/StageSceneStateGetLifeMaxUpItem.h"
#include "Scene/StageSceneStateGetShine.h"
#include "Scene/StageSceneStateGetShineGrand.h"
#include "Scene/StageSceneStateGetShineMain.h"
#include "Scene/StageSceneStateGetShineMainLast.h"
#include "Scene/StageSceneStateGetShineMainSandWorld.h"
#include "Scene/StageSceneStateGetShineMainWaterfallWorld.h"
#include "Scene/StageSceneStateGetShineMainWithCageShine.h"
#include "Scene/StageSceneStateMiniGameRanking.h"
#include "Scene/StageSceneStateMiss.h"
#include "Scene/StageSceneStatePauseMenu.h"
#include "Scene/StageSceneStateRaceManRace.h"
#include "Scene/StageSceneStateRadicon.h"
#include "Scene/StageSceneStateRecoverLife.h"
#include "Scene/StageSceneStateScenarioCamera.h"
#include "Scene/StageSceneStateShop.h"
#include "Scene/StageSceneStateSkipDemo.h"
#include "Scene/StageSceneStateSnapShot.h"
#include "Scene/StageSceneStateStageMap.h"
#include "Scene/StageSceneStateTalk.h"
#include "Scene/StageSceneStateTimeBalloon.h"
#include "Scene/StageSceneStateTitleLogo.h"
#include "Scene/StageSceneStateWarp.h"
#include "Scene/StageSceneStateWorldIntroCamera.h"
#include "Scene/StageSceneStateWorldMap.h"
#include "Scene/StageSceneStateYukimaruRace.h"
#include "Scene/StageSceneStateYukimaruRaceTutorial.h"
#include "Scene/SwitchKeepOn2DAreaGroup.h"
#include "Scene/SwitchKeepOn2DExceptDokanInAreaGroup.h"
#include "Scene/SwitchKeepOnIgnoreOffAreaTarget.h"
#include "Scene/SwitchKeepOnPlayerInWaterAreaGroup.h"
#include "Scene/SwitchOn2DAreaGroup.h"
#include "Scene/SwitchOnPlayerOnGroundAreaGroup.h"
#include "Scene/TemporaryScenarioCameraHolder.h"
#include "Scene/TimeBalloonSequenceInfo.h"
#include "Scene/WipeHolderRequester.h"
#include "Sequence/GameSequenceInfo.h"
#include "System/Application.h"
#include "System/GameConfigData.h"
#include "System/GameDataFile.h"
#include "System/GameDataFunction.h"
#include "System/GameDataHolder.h"
#include "System/GameDataHolderAccessor.h"
#include "System/GameDataHolderWriter.h"
#include "System/GameDataUtil.h"
#include "System/ProjectNfpDirector.h"
#include "Util/AmiiboUtil.h"
#include "Util/CameraUtil.h"
#include "Util/CheckpointUtil.h"
#include "Util/DemoUtil.h"
#include "Util/HelpFunction.h"
#include "Util/InputInterruptTutorialUtil.h"
#include "Util/PlayerDemoUtil.h"
#include "Util/PlayerUtil.h"
#include "Util/ScenePlayerCameraFunction.h"
#include "Util/ScenePrepoFunction.h"
#include "Util/SpecialBuildUtil.h"
#include "Util/StageInputFunction.h"

namespace {

class ProjectTrafficAreaUpdateJudge : public al::ITrafficAreaUpdateJudge {
public:
    ProjectTrafficAreaUpdateJudge(al::DemoDirector* demoDirector) : mDemoDirector(demoDirector) {}

    bool judge() const override { return !mDemoDirector->isActiveDemo(); }

    al::DemoDirector* mDemoDirector;
};

NERVE_IMPL(StageScene, StartStageBgm);
NERVE_IMPL(StageScene, CollectBgm);
NERVE_IMPL(StageScene, CollectionList);
NERVE_IMPL(StageScene, MiniGameRanking);
NERVE_IMPL(StageScene, Shop);
NERVE_IMPL(StageScene, Closet);
NERVE_IMPL(StageScene, DemoShineGet);
NERVE_IMPL(StageScene, DemoTalk);
NERVE_IMPL(StageScene, Radicon);
NERVE_IMPL(StageScene, Warp);
NERVE_IMPL(StageScene, DemoGetLifeMaxUpItem);
NERVE_IMPL(StageScene, DemoWorldIntroCamera);
NERVE_IMPL_(StageScene, DemoWorldIntroCameraBeforeAppearElectricDemo, DemoWorldIntroCamera);
NERVE_IMPL(StageScene, SnapShot);
NERVE_IMPL(StageScene, DemoShineMainGet);
NERVE_IMPL(StageScene, DemoShineGrandGet);
NERVE_IMPL(StageScene, SkipDemo);
NERVE_IMPL(StageScene, Miss);
NERVE_IMPL(StageScene, DemoCarryMeat);
NERVE_IMPL(StageScene, RaceYukimaru);
NERVE_IMPL(StageScene, RaceYukimaruTutorial);
NERVE_IMPL(StageScene, RaceManRace);
NERVE_IMPL(StageScene, DemoTitleLogo);
NERVE_IMPL(StageScene, Pause);
NERVE_IMPL(StageScene, ArriveAtCheckpoint);
NERVE_IMPL(StageScene, TimeBalloon);
NERVE_IMPL(StageScene, DemoScenarioCamera);
NERVE_IMPL(StageScene, AppearFromWorldWarpHole);
NERVE_IMPL_(StageScene, DemoStageStartOpening, DemoStageStart);
NERVE_IMPL(StageScene, DemoStageStart);
NERVE_IMPL(StageScene, DemoPlayerDown);
NERVE_IMPL(StageScene, DemoStageStartCapManHeroTalk);
NERVE_IMPL_(StageScene, DemoCapManHeroTalkMoonRock, DemoStageStartCapManHeroTalk);
NERVE_IMPL(StageScene, DemoReturnToHome);
NERVE_IMPL(StageScene, DemoHackStart);
NERVE_END_IMPL(StageScene, Play);
NERVE_IMPL_(StageScene, DemoWithPlayerUseCoinNoFirst, DemoWithPlayerUseCoin);
NERVE_IMPL(StageScene, DemoDamageStopPlayer);
NERVE_IMPL(StageScene, DemoSceneStartPlayerWalk);
NERVE_IMPL(StageScene, DemoRiseMapParts);
NERVE_IMPL(StageScene, DemoShineChipComplete);
NERVE_IMPL(StageScene, DemoHackStartFirst);
NERVE_IMPL(StageScene, DemoNormalBazookaElectric);
NERVE_IMPL(StageScene, DemoWithPlayerUseCoin);
NERVE_IMPL(StageScene, DemoWithPlayer);
NERVE_IMPL(StageScene, DemoNormal);
NERVE_IMPL(StageScene, Amiibo);
NERVE_IMPL_(StageScene, DemoStageStartElectricDemo, DemoStageStart);
NERVE_IMPL(StageScene, WaitStartWarpForSession);
NERVE_IMPL(StageScene, DemoCountCoin);
NERVE_IMPL(StageScene, WarpToCheckpoint);
NERVE_IMPL(StageScene, DemoAppearFromHome);
NERVE_IMPL(StageScene, DemoRisePyramid);
NERVE_IMPL(StageScene, WaitWarpToCheckpoint);

NERVES_MAKE_STRUCT(StageScene, StartStageBgm, CollectBgm, CollectionList, MiniGameRanking, Shop,
                   Closet, DemoShineGet, DemoTalk, Radicon, Warp, DemoGetLifeMaxUpItem,
                   DemoWorldIntroCamera, DemoWorldIntroCameraBeforeAppearElectricDemo, SnapShot,
                   DemoShineMainGet, DemoShineGrandGet, SkipDemo, Miss, DemoCarryMeat, RaceYukimaru,
                   RaceYukimaruTutorial, RaceManRace, DemoTitleLogo, Pause, ArriveAtCheckpoint,
                   TimeBalloon, DemoScenarioCamera, AppearFromWorldWarpHole, DemoStageStartOpening,
                   DemoStageStart, DemoPlayerDown, DemoStageStartCapManHeroTalk,
                   DemoCapManHeroTalkMoonRock, DemoReturnToHome, DemoHackStart, Play,
                   DemoWithPlayerUseCoinNoFirst, DemoDamageStopPlayer, DemoSceneStartPlayerWalk,
                   DemoRiseMapParts, DemoShineChipComplete, DemoHackStartFirst,
                   DemoNormalBazookaElectric, DemoWithPlayerUseCoin, DemoWithPlayer, DemoNormal,
                   Amiibo, DemoStageStartElectricDemo, WaitStartWarpForSession, DemoCountCoin,
                   WarpToCheckpoint, DemoAppearFromHome, DemoRisePyramid, WaitWarpToCheckpoint)
}  // namespace

// This inline is required for two matches
inline void positionAudioControl(al::Scene* scene) {
    al::startAudioEffectWithAreaCheck(scene);
    if (!al::isPlayingBgm(scene, "Shop"))
        al::startAndStopBgmInCurPosition(scene, true);
}

StageScene::StageScene() : al::Scene("StageScene") {
    mStageName.clear();
}

StageScene::~StageScene() {
    if (mDemoSyncedEventKeeper)
        mDemoSyncedEventKeeper->finalize();
    al::stopPadRumble(this);
    auto* gameFrameworkNx =
        sead::DynamicCast<al::GameFrameworkNx>(Application::instance()->getGameFramework());
    gameFrameworkNx->enableClearRenderBuffer();
    if (mChromakeyDrawer)
        mChromakeyDrawer->finalize();
    al::tryDeleteSceneObj<PaintObjHolder>(this);
    al::tryDeleteSceneObj(this, SceneObjID_alFriendsProfileDownloader);
    rs::getQuestInfoHolder(this)->finalizeForScene();
    alEffectEnvUtil::resetNatureDirector(getLiveActorKit()->getEffectSystem());
    al::tryDeleteSceneObj(this, SceneObjID_PlayerHackStartTexKeeper);
    al::tryDeleteSceneObj<TimeBalloonDirector>(this);
    if (mStateMiniGameRanking) {
        delete mStateMiniGameRanking;
        mStateMiniGameRanking = nullptr;
    }
    if (mLayoutTextureRenderer) {
        delete mLayoutTextureRenderer;
        mLayoutTextureRenderer = nullptr;
    }
    if (mStateWorldMap) {
        delete mStateWorldMap;
        mStateWorldMap = nullptr;
    }
}

void StageScene::init(const al::SceneInitInfo& initInfo) {
    using StageSceneFunctor = al::FunctorV0M<StageScene*, void (StageScene::*)()>;

    mStageName = initInfo.initStageName;
    mScenarioNo = initInfo.scenarioNo;
    initDrawSystemInfo(initInfo);
    al::initRandomSeedByString(mStageName.cstr());
    if(rs::isModeE3MovieRom() || rs::isModeMovieRom()){
        sead::DynamicCast<al::GameFrameworkNx>(Application::instance()->getGameFramework())->setVBlankWaitInterval(2);
    }else{
        sead::DynamicCast<al::GameFrameworkNx>(Application::instance()->getGameFramework())->setVBlankWaitInterval(1);
    }

    initLayoutKit(initInfo);
    al::addResourceCategory("シーン", 0x200, sead::HeapMgr::instance()->getCurrentHeap());
    al::SceneObjHolder* sceneObjHolder = SceneObjFactory::createSceneObjHolder();
    initSceneObjHolder(sceneObjHolder);
    al::createSceneObj<KidsModeLayoutAccessor>(this);
    al::createSceneObj<GuidePosInfoHolder>(this);
    al::createSceneObj<SceneEventNotifier>(this);
    initAndLoadStageResource(mStageName.cstr(), initInfo.scenarioNo);
    GameDataHolder* dataHolder = (GameDataHolder*)initInfo.gameDataHolder;
    mGameDataHolder = dataHolder;
    mTimeBalloonSequenceInfo = dataHolder->getTimeBalloonSequenceInfo();
    _408 = mTimeBalloonSequenceInfo;
    dataHolder->getQuestInfoHolder()->initSceneObjHolder(getSceneObjHolder());
    rs::tryStartRaceStage(this, mGameDataHolder);
    mGameDataHolder->startStage(mStageName.cstr(), initInfo.scenarioNo);
    mGameDataHolder->setIsWaitingForStageStart(false);
    al::setSceneObj(this, mGameDataHolder);
    al::setSceneObj(this, mGameDataHolder->getQuestInfoHolder());
    al::createSceneObj<PaintObjHolder>(this);
    al::createSceneObj<FukankunZoomObjHolder>(this);
    al::createSceneObj<al::StageSyncCounter>(this);
    al::createSceneObj<GrowPlantDirector>(this);
    al::createSceneObj<CapManHeroDemoDirector>(this);
    initScreenCoverCtrl();
    initSceneStopCtrl();
    al::GraphicsInitArg graphicsInitArg(al::getSceneDrawContext(this),
                                        al::getSceneFrameBufferConsole(this));
    al::PlacementInfo placementInfos;
    s32 placementInfoCount = 0;
    al::tryGetPlacementInfoAndCount(&placementInfos, &placementInfoCount,
                                    al::getStageInfoMap(this, 0), "ObjectList");
    bool foundMirror = false;
    for (s32 i = 0; i < placementInfoCount; i++) {
        al::PlacementInfo placementInfo;
        al::getPlacementInfoByIndex(&placementInfo, placementInfos, i);
        if (al::isObjectName(placementInfo, "Mirror")) {
            foundMirror = true;
            break;
        }
    }

    graphicsInitArg.atmosScatterViewNum = foundMirror;
    graphicsInitArg._3c = 2;
    graphicsInitArg._10 = 2 << (s32)foundMirror;
    initLiveActorKitWithGraphics(graphicsInitArg, initInfo, 5120, 4, 2);
    al::initGraphicsSystemInfo(this, mStageName.cstr(), mScenarioNo);
    if (rs::isModeJungleGymRom() || rs::isModeDiverRom() || rs::isModeE3MovieRom() ||
        rs::isModeMovieRom() || GameDataFunction::isHomeShipStage(mGameDataHolder))
        alGraphicsFunction::invalidateCameraBlurSystem(this);
    al::NetworkSystem* networkSystem = initInfo.gameSystemInfo->networkSystem;
    if (networkSystem) {
        al::DataStoreDirector* dataStoreDir = networkSystem->getDataStoreDirector();
        if (dataStoreDir)
            al::setSceneObj(this, dataStoreDir);
        al::RankingDirector* rankingDir = networkSystem->getRankingDirector();
        if (rankingDir)
            al::setSceneObj(this, rankingDir);
    }
    ProjectItemDirector* itemDir = new ProjectItemDirector();
    al::initItemDirector(this, itemDir);
    mProjectItemDirector = itemDir;
    al::initHitSensorDirector(this);
    ProjectDemoDirector* demoDir = new ProjectDemoDirector(
        al::getScenePlayerHolder(this), getLiveActorKit()->getGraphicsSystemInfo());
    al::initDemoDirector(this, demoDir);
    {
        al::AudioDirectorInitInfo audioDirInitInfo;
        audioDirInitInfo.seDirectorInitInfo.defaultListenerName = "カスタム線リスナ";
        al::GraphicsSystemInfo* graphicsSysInfo = getLiveActorKit()->getGraphicsSystemInfo();
        if (graphicsSysInfo)
            audioDirInitInfo.seDirectorInitInfo.occlusionCullingJudge =
                graphicsSysInfo->getOcclusionCullingJudge();
        al::initAudioDirector3D(this, initInfo, audioDirInitInfo);
    }
    alAudioSystemFunction::enableAudioMaximizer(initInfo.gameSystemInfo);
    mAudioSystemPauseController = new SceneAudioSystemPauseController(getAudioDirector());
    mDemoSoundSynchronizer = new DemoSoundSynchronizer(
        initInfo.gameSystemInfo->applicationMessageReciever, getAudioDirector());
    mStageStartAtmosSe = new al::SimpleAudioUser("StageStartAtmosSe", getAudioDirector());
    mSePlayArea = new al::SimpleAudioUser("SePlayArea", getAudioDirector());
    mSnapShotCameraCtrl = new al::SimpleAudioUser("SnapShotCameraCtrl", getAudioDirector());
    mSeNamedList = new ProjectSeNamedList();
    al::initSceneAudioKeeper(this, initInfo, "StageSceneDefault");

    getAudioDirector()->setPlayerHolder(al::getScenePlayerHolder(this));
    al::setTriggerEventForStopAllBgm(this, StageSceneFunctor(this, &StageScene::stopCollectBgm));

    if (rs::isModeE3MovieRom()) {
        getAudioDirector()->getSeDirector()->setMuteSeInPVList(
            mSeNamedList->getMuteSeInPVList(), mSeNamedList->getMuteSeInPVListSize());
    }
    if (rs::isModeMovieRom()) {
        getAudioDirector()->getSeDirector()->setMuteSeInPVList(
            mSeNamedList->getMuteSeInPVList(), mSeNamedList->getMuteSeInPVListSize());
    }
    al::deactivateAudioEventController(this);
    BgmAnimeSyncDirector* bgmAnimeSyncDir = new BgmAnimeSyncDirector();
    mBgmAnimeSyncDirector = bgmAnimeSyncDir;
    al::setSceneObj(this, bgmAnimeSyncDir);
    al::setSceneObj(this, mCollectBgmPlayer);
    if (getLiveActorKit()->getDemoDirector()) {
        al::DemoSyncedEventKeeper* demoSyncedEventKeeper =
            new al::DemoSyncedEventKeeper(getAudioDirector());
        mDemoSyncedEventKeeper = demoSyncedEventKeeper;
        alDemoSyncedEventFunction::tryCreateAndSetSeKeeper(demoSyncedEventKeeper,
                                                           getAudioDirector(), "StageScene");
    }
    al::initPadRumble(this, initInfo);
    al::initNatureDirector(this);
    alEffectEnvUtil::setNatureDirector(getLiveActorKit()->getEffectSystem(),
                                       getLiveActorKit()->getNatureDirector());
    ProjectActorFactory actorFactory;
    al::LayoutInitInfo layoutInitInfo;
    al::initLayoutInitInfo(&layoutInitInfo, this, initInfo);
    al::PlacementInfo placementInfo;
    al::ActorInitInfo actorInitInfo;
    al::initActorInitInfo(&actorInitInfo, this, &placementInfo, &layoutInitInfo, &actorFactory,
                          nullptr, mGameDataHolder);
    CapManHeroDemoUtil::initCapManHeroDemoDirector(this, actorInitInfo);
    al::LiveActor* checkpointWarpCapActor = new al::LiveActor("チェックポイントワープ用帽子");
    mCheckpointWarpCapActor = checkpointWarpCapActor;
    al::initActorWithArchiveName(checkpointWarpCapActor, actorInitInfo, "MarioCap",
                                 "CheckpointFlagWarp");
    mCheckpointWarpCapActor->makeActorDead();
    mCheckpointWarpParabolicPath = new al::ParabolicPath();
    mDemoShine = new al::LiveActor("デモ用シャイン");
    al::LiveActor* demoPowerStarActor = new al::LiveActor("デモ用パワースター");
    al::initChildActorWithArchiveNameNoPlacementInfo(demoPowerStarActor, actorInitInfo, "PowerStar",
                                                     "Demo");
    al::initChildActorWithArchiveNameNoPlacementInfo(mDemoShine, actorInitInfo, "Shine", "Demo");
    mDemoShine->kill();
    demoPowerStarActor->kill();
    mDemoDotShine = new al::LiveActor("デモ用ドットシャイン");
    al::initChildActorWithArchiveNameNoPlacementInfo(mDemoDotShine, actorInitInfo, "ShineDot",
                                                     "Demo");
    mDemoDotShine->kill();
    mProjectItemDirector->initPlayerDeadCoin(mGameDataHolder, actorInitInfo);
    mGameDataHolder->setUnkNumber(0);
    mLayoutTextureRenderer = new al::LayoutTextureRenderer();
    al::setSceneObj(this, mLayoutTextureRenderer);
    al::createSceneObj<HtmlViewerRequester>(this);
    mHtmlViewer = initInfo.gameSystemInfo->htmlViewer;
    al::initCameraDirector(this, mStageName.cstr(), initInfo.scenarioNo,
                           new ProjectCameraPoserFactory());
    if (rs::isKidsMode(this))
        alCameraFunction::validateCameraAreaKids(this);
    al::initSceneCameraFovyDegree(this, 40.0f);
    al::setNearClipDistance(this, 25.0f, 0);
    al::setFarClipDistance(this, 500000.0f, 0);
    al::setCameraAspect(this, al::getSceneFrameBufferMainAspect(this),
                        al::getSceneFrameBufferMainAspect(this));
    al::initSnapShotCameraAudioKeeper(this, mSnapShotCameraCtrl);
    ProjectAreaFactory* areaFactory = new ProjectAreaFactory();
    al::initAreaObjDirector(this, areaFactory);
    EventFlowSceneExecuteCtrl* eventFlowExecCtrl = new EventFlowSceneExecuteCtrl();
    mNpcEventDirector =
        new NpcEventDirector(al::getScenePlayerHolder(this), getLiveActorKit()->getCameraDirector(),
                             getLiveActorKit()->getCollisionDirector(),
                             initInfo.gameSystemInfo->messageSystem, eventFlowExecCtrl);
    mNpcEventDirector->init(actorInitInfo);
    al::setSceneObj(this, mNpcEventDirector->getNpcEventCtrlInfo());
    al::setSceneObj(this, mNpcEventDirector->getEventDemoCtrl());
    al::createSceneObj<TalkNpcParamHolder>(this);
    al::createSceneObj<TalkNpcSceneEventSwitcher>(this);

    al::initPlacementAreaObj(this, actorInitInfo);

    if (rs::isKidsMode(this)) {
        auto* safetyWatcher = new SafetyPointRecoveryCameraWatcher(
            getLiveActorKit()->getCameraDirector(), al::getScenePlayerHolder(this),
            al::getSceneAreaObjDirector(this)->getAreaObjGroup("RecoveryTargetPosKidsArea"),
            al::getSceneAreaObjDirector(this)->getAreaObjGroup("ForceRecoveryKidsArea"));
        al::registerExecutorUser(safetyWatcher, al::getSceneExecuteDirector(this),
                                 "エリア監視オブジェ");
    }

    al::initPlacementObjectMap(this, actorInitInfo, "NatureList");

    al::AreaObjDirector* areaObjDirector = al::getSceneAreaObjDirector(this);
    al::AreaObjGroup* trafficAreaGroup = areaObjDirector->getAreaObjGroup("TrafficArea");
    if (trafficAreaGroup) {
        auto* updateJudge = new ProjectTrafficAreaUpdateJudge(getLiveActorKit()->getDemoDirector());
        auto* trafficDir = new al::TrafficAreaDirector(trafficAreaGroup, updateJudge);
        al::registerExecutorUser(trafficDir, al::getSceneExecuteDirector(this),
                                 "エリア監視オブジェ");
        al::setSceneObj(this, trafficDir);
    }

    const al::AreaObjGroup* birdAreaGroup =
        al::getSceneAreaObjDirector(this)->getAreaObjGroup("BirdGatheringSpotArea");
    if (birdAreaGroup) {
        auto* birdDir = new BirdGatheringSpotDirector();
        birdDir->init(actorInitInfo, birdAreaGroup, getLiveActorKit()->getCameraDirector(),
                      getSceneObjHolder(),
                      getLiveActorKit()->getClippingDirector()->getClippingJudge(),
                      mStageName.cstr());
        al::registerExecutorUser(birdDir, al::getSceneExecuteDirector(this), "エリア監視オブジェ");
    }

    auto* infoWindowDir = new InformationWindowDirector(
        al::getSceneAreaObjDirector(this), getSceneObjHolder(), al::getScenePlayerHolder(this));
    infoWindowDir->init(actorInitInfo, layoutInitInfo);
    al::setSceneObj(this, infoWindowDir);
    al::registerExecutorUser(infoWindowDir, al::getSceneExecuteDirector(this),
                             "エリア監視オブジェ");

    PlayerStartInfoHolder* playerStartInfoHolder = al::createSceneObj<PlayerStartInfoHolder>(this);
    playerStartInfoHolder->init(getLiveActorKit()->getCameraDirector(), 128);

    al::AreaObjGroup* changeStageAreaGroup =
        al::getSceneAreaObjDirector(this)->getAreaObjGroup("ChangeStageArea");
    if (changeStageAreaGroup) {
        s32 areaCount = changeStageAreaGroup->getSize();
        for (s32 i = 0; i < areaCount; i++) {
            al::AreaObj* areaObj = changeStageAreaGroup->getAreaObj(i);
            const char* shineStageName = nullptr;
            const char* shineObjId = nullptr;
            s32 shineGotOnOff = 0;
            al::tryGetStringArg(&shineStageName, *areaObj->getPlacementInfo(), "ShineStageName");
            al::tryGetStringArg(&shineObjId, *areaObj->getPlacementInfo(), "ShineObjId");
            al::tryGetArg(&shineGotOnOff, *areaObj->getPlacementInfo(), "ShineGotOnOff");
            // TODO: This looks like it could be simplified, but if "is 1 and isGotShine or is 2 and
            // not isGotShine" -> invalidate() causes a worsw mismatch
            if (shineGotOnOff == 1) {
                if (!GameDataFunction::isGotShine(mGameDataHolder, shineStageName, shineObjId))
                    areaObj->invalidate();
            } else if (shineGotOnOff == 2) {
                if (GameDataFunction::isGotShine(mGameDataHolder, shineStageName, shineObjId))
                    areaObj->invalidate();
            }
        }
    }

    playerStartInfoHolder->registerChangeStageAreaGroup(changeStageAreaGroup, actorInitInfo);

    al::AreaObjGroup* warpAreaGroup =
        al::getSceneAreaObjDirector(this)->getAreaObjGroup("WarpArea");
    playerStartInfoHolder->registerWarpAreaGroup(warpAreaGroup, actorInitInfo);

    al::AreaObjGroup* restartAreaGroup =
        al::getSceneAreaObjDirector(this)->getAreaObjGroup("RestartArea");
    playerStartInfoHolder->registerRestartAreaGroup(restartAreaGroup);

    bool foundCactus = false;
    s32 stageInfoMapNum = al::getStageInfoMapNum(this);
    // Removing this causes a worse mismatch
    if (stageInfoMapNum >= 1) {
        for (s32 i = 0; i < stageInfoMapNum; i++) {
            const al::StageInfo* stageInfoMap = al::getStageInfoMap(this, i);
            al::PlacementInfo playerAffectPlacementInfo;
            s32 playerAffectCount = 0;
            al::tryGetPlacementInfoAndCount(&playerAffectPlacementInfo, &playerAffectCount,
                                            stageInfoMap, "PlayerAffectObjList");
            for (s32 j = 0; j < playerAffectCount; j++) {
                al::PlacementInfo pInfo;
                al::getPlacementInfoByIndex(&pInfo, playerAffectPlacementInfo, j);
                const char* objName = nullptr;
                al::getObjectName(&objName, pInfo);
                const char* modelName = nullptr;
                alPlacementFunction::tryGetModelName(&modelName, pInfo);
                if (al::isEqualString(objName, "HackMoveParts")) {
                    al::createPlacementActorFromFactory(actorInitInfo, &pInfo);
                    foundCactus |= al::isEqualString(modelName, "HackMoveCactus");
                } else if (al::isEqualString(objName, "CactusMini") ||
                           al::isEqualString(objName, "Cactus")) {
                    al::createPlacementActorFromFactory(actorInitInfo, &pInfo);
                    foundCactus = true;
                }
            }
        }
    }

    al::LiveActor* pyramid = al::tryInitPlacementSingleObject(
        this, actorInitInfo, 0, "SceneWatchObjList", "SandWorldHomePyramidKai000");
    if (pyramid) {
        mPyramid = (Pyramid*)pyramid;
        mStateGetShineMainSandWorld = new StageSceneStateGetShineMainSandWorld(
            "メインムーンゲット[砂ワールド]", this, actorInitInfo);
    }

    al::initPlacementObjectMap(this, actorInitInfo, "RaceList");

    StageSceneStateGetShineMainWithCageShine* cageShineState =
        StageSceneStateGetShineMainWithCageShine::tryCreate(this, actorInitInfo);

    auto* checkpointFlagWatcher = new CheckpointFlagWatcher(getLiveActorKit()->getCameraDirector());
    checkpointFlagWatcher->initStageInfo(mStageName.cstr(), mScenarioNo);
    al::setSceneObj(this, checkpointFlagWatcher);
    al::registerExecutorUser(checkpointFlagWatcher, al::getSceneExecuteDirector(this),
                             "バルーン位置更新");

    al::initPlacementObjectMap(this, actorInitInfo, "CheckPointList");
    al::initPlacementObjectMap(this, actorInitInfo, "PlayerStartInfoList");

    mShoppingWatcherGroup = new al::DeriveActorGroup<ShoppingWatcher>("ショップ店員", 26);
    al::tryInitPlacementActorGroup(mShoppingWatcherGroup, this, actorInitInfo, 0,
                                   "SceneWatchObjList", "ShoppingWatcher");

    al::DeriveActorGroup<Doshi>* doshiGroup =
        new al::DeriveActorGroup<Doshi>("ドッシーグループ", 4);
    if (al::tryInitPlacementActorGroup(doshiGroup, this, actorInitInfo, 0, "SceneWatchObjList",
                                       "Doshi")) {
        for (s32 i = 0; i < doshiGroup->getActorCount(); i++) {
            al::LiveActor* shoppingWatcher = doshiGroup->getDeriveActor(i)->getShoppingWatcher();
            if (shoppingWatcher)
                mShoppingWatcherGroup->registerActor(shoppingWatcher);
        }
    }

    al::LiveActor* collectionList = al::tryInitPlacementSingleObject(
        this, actorInitInfo, 0, "SceneWatchObjList", "CollectionList");
    if (collectionList)
        mCollectionList = (CollectionList*)collectionList;

    al::LiveActor* timeBalloonNpc = al::tryInitPlacementSingleObject(
        this, actorInitInfo, 0, "SceneWatchObjList", "TimeBalloonNpc");
    if (timeBalloonNpc)
        mTimeBalloonNpc = (TimeBalloonNpc*)timeBalloonNpc;

    mStateCloset = StageSceneStateCloset::tryCreate(this, actorInitInfo, layoutInitInfo);

    al::createSceneObj<CapMessageDirector>(this);
    auto* capMessageMoonNotifier = static_cast<CapMessageMoonNotifier*>(
        al::createSceneObj(this, SceneObjID_CapMessageMoonNotifier));
    capMessageMoonNotifier->initialize(al::getStageInfoMap(this, 0), actorInitInfo);

    auto* footPrintServer = new al::FootPrintServer(actorInitInfo, "FootPrint", 32);
    al::setSceneObj(this, (al::ISceneObj*)footPrintServer, SceneObjID_alFootPrintServer);

    al::PlacementInfo restartPlacementInfo;
    const char* restartPointId = GameDataFunction::tryGetRestartPointIdString(mGameDataHolder);
    const char* nextPlayerStartId  = mGameDataHolder->getNextPlayerStartId();
    if (!restartPointId)
        restartPointId = nextPlayerStartId ;

    sead::FixedSafeString<256> playerStartId;

    if (GameDataFunction::isWarpCheckpoint(mGameDataHolder)) {
        const char* checkpointStartId = "Home";
        if (restartPointId)
            checkpointStartId = restartPointId;

        const al::PlacementInfo* checkpointRestartInfo =
            rs::tryFindCheckpointFlagPlayerRestartInfo(this, checkpointStartId);
        al::PlacementId placementIdForCheckpoint;
        al::tryGetPlacementId(&placementIdForCheckpoint, *checkpointRestartInfo);
        playerStartId=al::makeStringPlacementId(&placementIdForCheckpoint);
        restartPointId = playerStartId.cstr();
    }

    s32 lastRaceRanking = GameDataFunction::getLastRaceRanking(this);
    mGameDataHolder->reset_492();

    bool restartPlacementInfoValid = restartPlacementInfo.getPlacementIter().isValid();
    const PlayerStartInfo* playerRestartInfo = nullptr;
    if (restartPointId && restartPlacementInfoValid) {
        playerRestartInfo = playerStartInfoHolder->tryFindInitInfoByStartId(restartPointId);
        if (playerRestartInfo) {
            restartPlacementInfo.set(playerRestartInfo->placementInfo->getPlacementIter(),
                                     playerRestartInfo->placementInfo->getZoneIter());

            if (lastRaceRanking <= 0 && playerRestartInfo->cameraTicket)
                al::startCamera(this, playerRestartInfo->cameraTicket, -1);
            else
                al::resetSceneInitEntranceCamera(this);
        }
    }

    CheckpointFlag* checkpointFlag = rs::tryFindCheckpointFlag(this, GameDataFunction::getCheckpointWarpObjId(mGameDataHolder));
    bool isWarpCheckpoint = GameDataFunction::isWarpCheckpoint(mGameDataHolder);
    if (checkpointFlag && isWarpCheckpoint && checkpointFlag->isHome())
        al::resetSceneInitEntranceCamera(this);

    const char* costumeTypeName = GameDataFunction::getCurrentCostumeTypeName(mGameDataHolder);
    const char* capTypeName = GameDataFunction::getCurrentCapTypeName(mGameDataHolder);
    if (costumeTypeName) {
        if (!al::isEqualString(costumeTypeName, ""))
            mCostumeName.format("%s", costumeTypeName);
    }
    if (capTypeName) {
        if (!al::isEqualString(capTypeName, ""))
            mCapTypeName.format("%s", capTypeName);
    }
    if (rs::isModeDiverOrJungleGymRom()) {
        mCostumeName.format("Mario");
        mCapTypeName.format("Mario");
    }

    PlayerFactory playerFactory;
    al::PlacementInfo playerListPlacementInfo;
    s32 playerListCount = 0;
    al::tryGetPlacementInfoAndCount(&playerListPlacementInfo, &playerListCount,
                                    al::getStageInfoMap(this, 0), "PlayerList");
    for (s32 i = 0; i < playerListCount; i++) {
        al::PlacementInfo playerPlacementInfo;
        al::getPlacementInfoByIndex(&playerPlacementInfo, playerListPlacementInfo, i);
        const char* changeStageId = nullptr;
        bool hasChangeStageId =
            al::tryGetStringArg(&changeStageId, playerPlacementInfo, "ChangeStageId");
        if (restartPointId && !restartPlacementInfo.getPlacementIter().isValid() &&
            !hasChangeStageId == false && al::isEqualString(changeStageId, restartPointId)) {
            restartPlacementInfo.set(playerPlacementInfo.getPlacementIter(),
                                     playerPlacementInfo.getZoneIter());
        }

        sead::Vector3f trans = sead::Vector3f::zero;
        sead::Quatf quat = sead::Quatf::unit;
        if (playerRestartInfo) {
            trans.set(playerRestartInfo->trans);
            quat.set(playerRestartInfo->quat);
        } else if (restartPlacementInfo.getPlacementIter().isValid()) {
            al::getTrans(&trans, restartPlacementInfo);
            al::getQuat(&quat, restartPlacementInfo);
        } else {
            al::getTrans(&trans, playerPlacementInfo);
            al::getQuat(&quat, playerPlacementInfo);
        }

        al::ActorInitInfo playerActorInitInfo;
        playerActorInitInfo.initViewIdSelf(&playerPlacementInfo, actorInitInfo);

        PlayerInitInfo playerInitInfo;
        playerInitInfo.gamePadSystem = initInfo.gameSystemInfo->gamePadSystem;
        playerInitInfo.viewMtxPtr = al::getViewMtxPtr(this, 0);
        playerInitInfo.controllerPort = al::getMainControllerPort();
        playerInitInfo.costumeName = mCostumeName.cstr();
        playerInitInfo.capTypeName = mCapTypeName.cstr();
        playerInitInfo._45 = (mStateCloset != nullptr);
        playerInitInfo.trans = trans;
        playerInitInfo.quat = quat;
        playerInitInfo._44 = foundCactus;

        if (!mCostumeName.isEmpty())
            GameDataFunction::wearCostume(mGameDataHolder, mCostumeName.cstr());
        if (!mCapTypeName.isEmpty())
            GameDataFunction::wearCap(mGameDataHolder, mCapTypeName.cstr());

        const char* objectName = nullptr;
        al::getObjectName(&objectName, playerActorInitInfo);
        const char* className = nullptr;
        al::getClassName(&className, playerActorInitInfo);

        PlayerActorBase* player = playerFactory.createActor(playerActorInitInfo, className);
        player->initPlayer(playerActorInitInfo, playerInitInfo);

        al::PadRumbleKeeper* padRumbleKeeper =
            al::createPadRumbleKeeper(player, player->getPortNo());
        alPlayerFunction::registerPlayer(player, padRumbleKeeper);

        al::setCameraTarget(this, new PlayerCameraTarget(player));
        al::setCameraInput(this, new ProjectCameraInput(player));

        if (al::isObjectName(playerActorInitInfo, "PlayerActorHakoniwa"))
            GameDataFunction::setStageHakoniwa(mGameDataHolder);
        if (al::isObjectName(playerActorInitInfo, "PlayerActor2D3D"))
            GameDataFunction::setStageHakoniwa(mGameDataHolder);

        if (!mCheckpointWarpArriveCamera)
            mCheckpointWarpArriveCamera = rs::initWarpCheckpointCamera(
                this, mCheckpointWarpCapActor, actorInitInfo, "CheckpointWarpCamera",
                &mCheckpointWarpTargetPos, &mCheckpointWarpParabolicPathPos, nullptr);
    }

    al::initPlacementByStageInfo(al::getStageInfoMap(this, 0), "SkyList", actorInitInfo);

    ScenarioCameraRelationInfo scenarioCameraBuffer[64];
    TemporaryScenarioCameraHolder tempScenarioCameraHolder(scenarioCameraBuffer, 64);
    mGameDataHolder->setTemporaryScenarioCameraHolder(&tempScenarioCameraHolder);

    al::initPlacementObjectMap(this, actorInitInfo, "ObjectList");
    al::initPlacementObjectDesign(this, actorInitInfo, "ObjectList");
    al::initPlacementObjectSound(this, actorInitInfo, "ObjectList");

    NpcEventCtrlInfo* npcEventCtrlInfoObj = al::getSceneObj<NpcEventCtrlInfo>(this);
    npcEventCtrlInfoObj->getExecutorHolder()->initAfterPlacement();

    mScenarioStartCameraHolder = new ScenarioStartCameraHolder();
    mScenarioStartCameraHolder->init(this, actorInitInfo);

    s32 demoStageInfoMapNum = al::getStageInfoMapNum(this);
    bool hasOpeningDemo = false;
    bool hasWaterfallStartDemo = false;
    for (s32 i = 0; i < demoStageInfoMapNum; i++) {
        const al::StageInfo* demoStageInfoMap = al::getStageInfoMap(this, i);
        al::PlacementInfo demoPlacementInfo;
        s32 demoObjCount = 0;
        al::tryGetPlacementInfoAndCount(&demoPlacementInfo, &demoObjCount, demoStageInfoMap,
                                        "DemoObjList");
        for (s32 j = 0; j < demoObjCount; j++) {
            al::PlacementInfo demoObjPlacementInfo;
            al::getPlacementInfoByIndex(&demoObjPlacementInfo, demoPlacementInfo, j);
            const char* demoObjName = nullptr;
            al::getObjectName(&demoObjName, demoObjPlacementInfo);
            if (al::isEqualString(demoObjName, "DemoRegister") ||
                al::isEqualString(demoObjName, "WorldIntroCamera") ||
                al::isEqualString(demoObjName, "RaceIntroCamera") ||
                al::isEqualString(demoObjName, "RaceCountDownCamera"))
                continue;
            OpeningStageStartDemo* demoActor =
                (OpeningStageStartDemo*)al::createPlacementActorFromFactory(actorInitInfo,
                                                                            &demoObjPlacementInfo);
            if (al::isEqualString(demoObjName, "StageEventDemo") ||
                al::isEqualString(demoObjName, "StageEventDemoNoSave")) {
                if (demoActor->isEnableStart()) {
                    mOpeningStageStartDemo = demoActor;
                    const char* demoStageName = nullptr;
                    al::getStringArg(&demoStageName, demoObjPlacementInfo, "DemoStageName");
                    hasWaterfallStartDemo |=
                        al::isEqualString(demoStageName, "DemoStartWorldWaterfallStage");
                }
            } else if (al::isEqualString(demoObjName, "OpeningStageStartDemo")) {
                mOpeningStageStartDemo = demoActor;
                if (demoActor->isEnableStart()) {
                    hasOpeningDemo = true;
                } else {
                    hasOpeningDemo = false;
                    mOpeningStageStartDemo = nullptr;
                }
            } else if (!al::isEqualString(demoObjName, "StageTalkDemoNpcCap") &&
                       !al::isEqualString(demoObjName, "StartEntranceCamera")) {
                al::isEqualString(demoObjName, "StageTalkDemoNpcCapMoonRock");
            }
        }
    }

    if (GameDataFunction::isFindKoopa(mGameDataHolder) ||
        al::isEqualString(mStageName, "TestKitazonoKoopaLv0Stage")) {
        al::LiveActorGroup* actorGroup = actorInitInfo.allActorsGroup;
        for (s32 i = 0; i < actorGroup->getActorCount(); i++) {
            if (al::isEqualString(actorGroup->getActor(i)->getName(), "クッパLV1")) {
                mKoopaLv1 = (KoopaLv1*)actorGroup->getActor(i);
                break;
            }
        }
    }

    QuestInfoHolder* questInfoHolder = rs::getQuestInfoHolder(this);
    s32 mainScenarioNo = mGameDataHolder->getGameDataFile()->getMainScenarioNoCurrent();
    questInfoHolder->initAfterPlacementQuestObj(mainScenarioNo);

    if (!rs::isModeE3Rom() && !rs::isModeE3LiveRom()) {
        mProjectNfpDirector = (ProjectNfpDirector*)initInfo.gameSystemInfo->nfpDirector;
        AmiiboFunction::tryCreateHelpAmiiboDirector(this);
        mHelpAmiiboDirector = al::getSceneObj<HelpAmiiboDirector>(this);
        if (mHelpAmiiboDirector)
            mHelpAmiiboDirector->init(mProjectNfpDirector, al::getScenePlayerHolder(this),
                                      getAudioDirector(), layoutInitInfo);
        AmiiboNpcDirector* amiiboNpcDirector = al::tryGetSceneObj<AmiiboNpcDirector>(this);
        if (amiiboNpcDirector)
            amiiboNpcDirector->init(mProjectNfpDirector, getAudioDirector());
    }

    mPlacementInfo = new al::PlacementInfo();
    if (!al::isPlayingBgm(this, "CollectBgm"))
        mCollectBgmPlayer->reset();

    mCollectBgmRegister =
        new CollectBgmRegister(getAudioDirector(), mGameDataHolder, mCollectBgmPlayer);

    mPlayGuideSkip = new PlayGuideSkip("デモスキップガイド", layoutInitInfo);
    mCinemaCaption = new CinemaCaption(layoutInitInfo);

    mSceneLayout =
        new StageSceneLayout("シーン情報", layoutInitInfo, al::getScenePlayerHolder(this),
                             getLiveActorKit()->getGraphicsSystemInfo()->getSubCameraRenderer());

    mScenarioStartLayout = new al::SimpleLayoutAppearWaitEnd("シナリオ開始", "StartScenario",
                                                             layoutInitInfo, nullptr, false);
    mScenarioStartLayout->kill();

    mWorldStartCountryLayout = new al::SimpleLayoutAppearWaitEnd("ワールド開始[国]", "StartWorld",
                                                                 layoutInitInfo, nullptr, false);
    mWorldStartCountryLayout->kill();

    mWorldStartRegionLayout = new al::SimpleLayoutAppearWaitEnd(
        "ワールド開始[地域]", "StartWorldRegion", layoutInitInfo, nullptr, false);
    mWorldStartRegionLayout->kill();

    mControllerGuideSnapshotCtrl = new ControllerGuideSnapShotCtrl(
        "スナップショットレイアウト更新", layoutInitInfo,
        getLiveActorKit()->getGraphicsSystemInfo()->getPostProcessingFilter());

    auto* inputSep = new InputSeparator(mWorldStartCountryLayout, false);
    mInputSeparator = inputSep;

    mLocationNameCtrl = new LocationNameCtrl(al::getSceneAreaObjDirector(this), mGameDataHolder,
                                             layoutInitInfo, al::getScenePlayerHolder(this));

    auto* windowConfirm = new al::WindowConfirm(layoutInitInfo, "WindowConfirm", "WindowConfirm");
    mWindowConfirm = windowConfirm;
    windowConfirm->kill();

    mMiniGameMenu = new MiniGameMenu(layoutInitInfo);

    mWipeSimple = new al::WipeSimple("ワープ用ワイプ", "WipeCircle", layoutInitInfo, nullptr);

    if (!mWipeHolder) {
        mWipeHolder = new al::WipeHolder(6);
        mWipeHolder->registerWipe("WipeCircle", new al::WipeSimple("ワープ用ワイプ", "WipeCircle",
                                                                   layoutInitInfo, nullptr));
        mWipeHolder->registerWipe(
            "FadeBlack", new al::WipeSimple("黒フェード", "FadeBlack", layoutInitInfo, nullptr));
        mWipeHolder->registerWipe(
            "FadeWhite", new al::WipeSimple("白フェード", "FadeWhite", layoutInitInfo, nullptr));
        mWipeHolder->registerWipe(
            "WipeMiss", new al::WipeSimple("ミスワイプ", "WipeMiss", layoutInitInfo, nullptr));
        mWipeHolder->registerWipe(
            "WipeSkip", new al::WipeSimple("スキップワイプ", "WipeSkip", layoutInitInfo, nullptr));
    }

    al::createSceneObj<WipeHolderRequester>(this);

    s32 currentWorldId = GameDataFunction::getCurrentWorldId(this);
    mMapLayout = new MapLayout(layoutInitInfo, al::getScenePlayerHolder(this), currentWorldId);
    al::setSceneObj(this, mMapLayout, SceneObjID_MapLayout);

    mPauseMenu =
        new al::SimpleLayoutAppearWaitEnd("ポーズメニュー", "Menu", layoutInitInfo, nullptr, false);
    mPauseMenu->kill();

    mStateCollectBgm = StageSceneStateCollectBgm::tryCreate(this, actorInitInfo, layoutInitInfo,
                                                            mCollectBgmPlayer);
    mStateMiniGameRanking =
        StageSceneStateMiniGameRanking::tryCreate(this, actorInitInfo, layoutInitInfo);

    mStateGetShine = new StageSceneStateGetShine(
        "シャインゲットデモ", this, mSceneLayout, initInfo, actorInitInfo, layoutInitInfo,
        mDemoShine, mDemoDotShine, demoPowerStarActor, mGameDataHolder, mProjectItemDirector);

    auto* stateGetShineMain = new StageSceneStateGetShineMain(
        "メインシャインゲットデモ", this, mSceneLayout, initInfo, actorInitInfo, layoutInitInfo,
        mDemoShine, mScenarioStartCameraHolder, mGameDataHolder);
    mStateGetShineMain = stateGetShineMain;

    auto* stateGetShineMainLast = new StageSceneStateGetShineMainLast(
        "メインシャインゲットデモ最後", this, stateGetShineMain->getDemoShineActor(),
        stateGetShineMain->getCameraTicket());

    mStateGetShineGrand = new StageSceneStateGetShineGrand("グランドシャインゲットデモ", this,
                                                           initInfo, actorInitInfo, layoutInitInfo);

    mStateSkipDemo = new StageSceneStateSkipDemo("デモスキップ", this, mWindowConfirm, mWipeHolder,
                                                 mPlayGuideSkip, mAudioSystemPauseController,
                                                 mDemoSyncedEventKeeper);

    mStateCheckpointWarp = new StageSceneStateCheckpointWarp(
        "チェックポイントワープ到着デモ", this, mCheckpointWarpCapActor, mGameDataHolder,
        al::initDemoProgramableCamera(this, actorInitInfo, "CheckpointWarpArriveCamera",
                                      &mCheckpointWarpTargetPos, &mCheckpointWarpParabolicPathPos,
                                      nullptr),
        &mCheckpointWarpTargetPos, &mCheckpointWarpParabolicPathPos);

    mStateCarryMeat = new StageSceneStateCarryMeat("肉運びデモ", this);

    mStagePauseMenu = new StageSceneStatePauseMenu(
        "ポーズ画面", this, mPauseMenu, mGameDataHolder, initInfo, actorInitInfo, layoutInitInfo,
        mWindowConfirm, mSceneLayout, false, mAudioSystemPauseController);

    mStateCollectionList =
        new StageSceneStateCollectionList("コレクションリスト", this, layoutInitInfo, mMapLayout,
                                          mStateCollectBgm, mAudioSystemPauseController);

    mStateWorldMap = StageSceneStateWorldMap::tryCreate(this, initInfo, actorInitInfo,
                                                        layoutInitInfo, mStateCollectionList);
    mStateSkipDemo->setWorldMapState(mStateWorldMap);

    mStateMiss = new StageSceneStateMiss(
        "StageScene:ミス", this, layoutInitInfo, mGameDataHolder, mSceneLayout,
        getLiveActorKit()->getGraphicsSystemInfo()->getSubCameraRenderer());

    mStateShop = new StageSceneStateShop("ショップ", this, mSceneLayout);
    mStateShop->initAndSetting(mShoppingWatcherGroup, actorInitInfo);

    mStateSnapShot = new StageSceneStateSnapShot(
        "スナップショット", this, mControllerGuideSnapshotCtrl, mAudioSystemPauseController,
        mInputSeparator, mNpcEventDirector);
    mStateSnapShot->init();

    mStateYukimaruRace = StageSceneStateYukimaruRace::tryCreate(
        this, actorInitInfo, mSceneLayout, mAudioSystemPauseController, mStateSnapShot);
    mStateYukimaruRaceTutorial = StageSceneStateYukimaruRaceTutorial::tryCreate(
        this, actorInitInfo, layoutInitInfo, mGameDataHolder, mStateSnapShot);
    mStateTitleLogo = StageSceneStateTitleLogo::tryCreate(this, actorInitInfo, layoutInitInfo,
                                                          mWipeHolder, mGameDataHolder);

    mStateScenarioCamera =
        new StageSceneStateScenarioCamera("シナリオ紹介カメラ", this, mStageName.cstr(),
                                          mScenarioNo, mStateGetShineMain->getDemoShineActor());
    mStateScenarioCamera->init();
    mStateScenarioCamera->setStateSkipDemo(mStateSkipDemo);

    mStateRecoverLife = new StageSceneStateRecoverLife("ライフ回復", this, mSceneLayout);
    mStateRecoverLife->init();
    mStateGetShine->setLifeRecoverState(mStateRecoverLife);

    mStateTalk = new StageSceneStateTalk(
        "会話", this, eventFlowExecCtrl, mSceneLayout, mCinemaCaption, mPlayGuideSkip,
        mStateMiniGameRanking, mStateSkipDemo, mStateWorldMap, mStateGetShine, mStateGetShineMain,
        mStateCollectionList, mGameDataHolder, mWipeSimple);

    mStateWarp =
        new StageSceneStateWarp("ワープ", this, mWipeSimple, mGameDataHolder, mLocationNameCtrl);
    mStateWarp->init();

    auto* stateGetLifeMaxUpItem =
        new StageSceneStateGetLifeMaxUpItem("最大ライフアップアイテム取得", this, mSceneLayout);
    stateGetLifeMaxUpItem->init();

    StageSceneStateGetShineMainWaterfallWorld* waterfallState =
        StageSceneStateGetShineMainWaterfallWorld::tryCreate(this, actorInitInfo);

    mStateRaceManRace = StageSceneStateRaceManRace::tryCreate(
        this, mGameDataHolder, mSceneLayout, mStateMiss, mStateCollectionList,
        stateGetLifeMaxUpItem, mStateWarp, actorInitInfo, mAudioSystemPauseController,
        mStateSnapShot);

    mStateWorldIntroCamera = new StageSceneStateWorldIntroCamera(
        "ワールド紹介カメラ", actorInitInfo, this, mWorldStartCountryLayout,
        mWorldStartRegionLayout, mStageStartAtmosSe, mStateSkipDemo);

    mStateRadicon = new StageSceneStateRadicon("ラジコン", this, mStateCollectionList,
                                               mStagePauseMenu, mStateSnapShot);

    if (mTimeBalloonNpc) {
        mTimeBalloonDirector = al::createSceneObj<TimeBalloonDirector>(this);
        mTimeBalloonDirector->init(actorInitInfo, mTimeBalloonNpc, mTimeBalloonSequenceInfo, false,
                                   false, mSceneLayout);
        mTimeBalloonSequenceInfo->setAccessor(mTimeBalloonDirector->getAccessor());

        mStateTimeBalloon = new StageSceneStateTimeBalloon(
            this, mTimeBalloonDirector, mTimeBalloonSequenceInfo, mGameDataHolder, mSceneLayout,
            actorInitInfo, mTimeBalloonNpc, false, mMiniGameMenu,
            mStateCollectionList->getStateStageMap(), mStateWarp);
        mTimeBalloonSequenceInfo->addHioNode();
    }

    al::initSwitchAreaDirector(this, 3, 2);

    al::AreaObjDirector* switchAreaObjDir = al::getSceneAreaObjDirector(this);
    al::PlayerHolder* switchPlayerHolder = al::getScenePlayerHolder(this);

    SwitchOnPlayerOnGroundAreaGroup* switchOnPlayerOnGround =
        SwitchOnPlayerOnGroundAreaGroup::tryCreate("SwitchOnPlayerOnGroundArea", switchAreaObjDir,
                                                   switchPlayerHolder);
    if (switchOnPlayerOnGround)
        al::registerSwitchOnAreaGroup(this, switchOnPlayerOnGround);

    SwitchOn2DAreaGroup* switchOn2D =
        SwitchOn2DAreaGroup::tryCreate("SwitchOn2DArea", switchAreaObjDir, switchPlayerHolder);
    if (switchOn2D)
        al::registerSwitchOnAreaGroup(this, switchOn2D);

    SwitchKeepOn2DAreaGroup* switchKeepOn2D = SwitchKeepOn2DAreaGroup::tryCreate(
        "SwitchKeepOn2DArea", switchAreaObjDir, switchPlayerHolder);
    if (switchKeepOn2D)
        al::registerSwitchKeepOnAreaGroup(this, switchKeepOn2D);

    SwitchKeepOn2DExceptDokanInAreaGroup* switchKeepOn2DExcept =
        SwitchKeepOn2DExceptDokanInAreaGroup::tryCreate("SwitchKeepOn2DExceptDokanInArea",
                                                        switchAreaObjDir, switchPlayerHolder);
    if (switchKeepOn2DExcept)
        al::registerSwitchKeepOnAreaGroup(this, switchKeepOn2DExcept);

    SwitchKeepOnPlayerInWaterAreaGroup* switchKeepOnWater =
        SwitchKeepOnPlayerInWaterAreaGroup::tryCreate("SwitchKeepOnPlayerInWaterArea",
                                                      switchAreaObjDir, switchPlayerHolder);
    if (switchKeepOnWater)
        al::registerSwitchKeepOnAreaGroup(this, switchKeepOnWater);

    SwitchKeepOnIgnoreOffAreaTarget* switchKeepOnIgnoreOff =
        SwitchKeepOnIgnoreOffAreaTarget::tryCreate("SwitchKeepOnIgnoreOffAreaTarget",
                                                   switchAreaObjDir);
    if (switchKeepOnIgnoreOff)
        al::registerSwitchKeepOnAreaGroup(this, switchKeepOnIgnoreOff);

    rs::applyGameConfigData(this, mGameDataHolder->getGameConfigData());
    rs::setSeparatePlayMode(this, rs::isSeparatePlay(this));

    al::Scene::endInit(actorInitInfo);

    al::createAudioEffect(this, "DistanceReverbSe");
    al::createAudioEffect(this, "DistanceReverbBgm");
    al::createAudioEffect(this, "BgmLevelDetection");

    NerveExecutor::initNerve(&NrvStageScene.StartStageBgm, 30);

    if (mStateCollectBgm)
        al::initNerveState(this, mStateCollectBgm, &NrvStageScene.CollectBgm, "コレクトBGM");
    if (mStateCollectionList)
        al::initNerveState(this, mStateCollectionList, &NrvStageScene.CollectionList,
                           "コレクションリスト");
    if (mStateMiniGameRanking)
        al::initNerveState(this, mStateMiniGameRanking, &NrvStageScene.MiniGameRanking,
                           "ミニゲームランキング");
    if (mStateShop)
        al::initNerveState(this, mStateShop, &NrvStageScene.Shop, "ショップ");
    if (mStateCloset)
        al::initNerveState(this, mStateCloset, &NrvStageScene.Closet, "クローゼット");

    al::initNerveState(this, mStateGetShine, &NrvStageScene.DemoShineGet, "シャインゲットデモ");
    al::initNerveState(this, mStateTalk, &NrvStageScene.DemoTalk, "会話");
    al::initNerveState(this, mStateRadicon, &NrvStageScene.Radicon, "ラジコン");
    al::addNerveState(this, mStateWarp, &NrvStageScene.Warp, "ワープ");
    al::addNerveState(this, stateGetLifeMaxUpItem, &NrvStageScene.DemoGetLifeMaxUpItem,
                      "最大ライフアップアイテム取得");
    al::initNerveState(this, mStateWorldIntroCamera, &NrvStageScene.DemoWorldIntroCamera,
                       "ワールド紹介カメラ");
    al::addNerveState(this, mStateWorldIntroCamera,
                      &NrvStageScene.DemoWorldIntroCameraBeforeAppearElectricDemo,
                      "ワールド紹介カメラ[電線デモ前]");
    al::addNerveState(this, mStateSnapShot, &NrvStageScene.SnapShot, "スナップショット");

    if (mPyramid) {
        mStateGetShineMainSandWorld->setStateGetShine(mStateGetShineMain);
        mStateGetShineMainSandWorld->setStateScenarioCamera(mStateScenarioCamera);
        mStateGetShineMainSandWorld->setStateRecoverLife(mStateRecoverLife);
        mStateGetShineMainSandWorld->setStateGetShineLast(stateGetShineMainLast);
        mStateGetShineMainSandWorld->setPyramid(mPyramid);
        al::initNerveState(this, mStateGetShineMainSandWorld, &NrvStageScene.DemoShineMainGet,
                           "メインシャインゲットデモ[砂ワールド]");
    } else if (waterfallState) {
        waterfallState->setStateGetShine(mStateGetShineMain);
        waterfallState->setStateScenarioCamera(mStateScenarioCamera);
        waterfallState->setStateRecoverLife(mStateRecoverLife);
        waterfallState->setStateGetShineLast(stateGetShineMainLast);
        al::initNerveState(this, waterfallState, &NrvStageScene.DemoShineMainGet,
                           "メインシャインゲットデモ[滝ワールド]");
    } else if (cageShineState) {
        cageShineState->setState(mStateGetShineMain, mStateScenarioCamera, mStateRecoverLife,
                                 stateGetShineMainLast);
        al::initNerveState(this, cageShineState, &NrvStageScene.DemoShineMainGet,
                           "メインシャインゲットデモ[ケージシャイン]");
    } else {
        al::initNerveState(this, mStateGetShineMain, &NrvStageScene.DemoShineMainGet,
                           "メインシャインゲットデモ");
    }

    mStateGetShineMain->setScenarioCameraState(mStateScenarioCamera);

    al::initNerveState(this, mStateGetShineGrand, &NrvStageScene.DemoShineGrandGet,
                       "グランドシャインゲットデモ");
    al::initNerveState(this, mStateSkipDemo, &NrvStageScene.SkipDemo, "デモスキップ");
    al::initNerveState(this, mStateMiss, &NrvStageScene.Miss, "ミス");
    al::initNerveState(this, mStateCarryMeat, &NrvStageScene.DemoCarryMeat, "肉運びデモ");
    mStateCarryMeat->setState(mStateSkipDemo);

    if (mStateYukimaruRace)
        al::initNerveState(this, mStateYukimaruRace, &NrvStageScene.RaceYukimaru, "ユキマルレース");
    if (mStateYukimaruRaceTutorial)
        al::initNerveState(this, mStateYukimaruRaceTutorial, &NrvStageScene.RaceYukimaruTutorial,
                           "ユキマルレースチュートリアル");
    if (mStateRaceManRace)
        al::initNerveState(this, mStateRaceManRace, &NrvStageScene.RaceManRace, "かけっこレース");
    if (mStateTitleLogo)
        al::initNerveState(this, mStateTitleLogo, &NrvStageScene.DemoTitleLogo, "タイトルロゴ");
    al::initNerveState(this, mStagePauseMenu, &NrvStageScene.Pause, "ポーズ画面");
    al::initNerveState(this, mStateCheckpointWarp, &NrvStageScene.ArriveAtCheckpoint,
                       "中間ポイントワープ");
    if (mTimeBalloonNpc)
        al::initNerveState(this, mStateTimeBalloon, &NrvStageScene.TimeBalloon, "風船ゲーム");
    al::addNerveState(this, mStateScenarioCamera, &NrvStageScene.DemoScenarioCamera,
                      "シナリオ紹介カメラ");

    mStateScenarioCamera->setScenarioStartCameraHolder(mScenarioStartCameraHolder);
    mStateScenarioCamera->setScenarioStartLayout(mScenarioStartLayout);

    if (rs::isShopStatusBuyMaxLifeUpItem(mDemoShine))
        rs::requestStartDemoGetLifeMaxUpItem(mDemoShine);

    if (GameDataFunction::isWarpCheckpoint(mGameDataHolder)) {
        al::setNerve(this, &NrvStageScene.ArriveAtCheckpoint);
    } else if (GameDataFunction::isPlayDemoWorldWarpHole(this)) {
        al::setNerve(this, &NrvStageScene.AppearFromWorldWarpHole);
    } else {
        GameDataHolder* holder = mGameDataHolder;
        s32 currentWorldIdNoDevelop = GameDataFunction::getCurrentWorldIdNoDevelop(holder);
        if (!GameDataFunction::isUnlockedWorld(holder, currentWorldIdNoDevelop)) {
            al::setNerve(this, &NrvStageScene.StartStageBgm);
        } else if (hasOpeningDemo) {
            al::setNerve(this, &NrvStageScene.DemoStageStartOpening);
        } else if (hasWaterfallStartDemo) {
            rs::changeDemoCommonProc(this, mProjectItemDirector);
            al::setNerve(this, &NrvStageScene.DemoWorldIntroCameraBeforeAppearElectricDemo);
        } else if (mOpeningStageStartDemo && mOpeningStageStartDemo->isEnableStart()) {
            al::setNerve(this, &NrvStageScene.DemoStageStart);
        } else if (GameDataFunction::isPlayDemoWorldWarp(this) ||
                   mGameDataHolder->getGameDataFile()->getPlayDemoWorldWarpState() == 2) {
            rs::changeDemoCommonProc(this, mProjectItemDirector);
            al::setNerve(this, &NrvStageScene.DemoWorldIntroCamera);
        } else if (tryChangeDemo()) {
            // nerve already set by tryChangeDemo
        } else if (mStateScenarioCamera->tryStart()) {
            al::resetRequestCaptureScreenCover(this);
            al::setNerve(this, &NrvStageScene.DemoScenarioCamera);
        } else if (GameDataFunction::isRaceStartYukimaru(this)) {
            al::setNerve(this, &NrvStageScene.RaceYukimaru);
        } else if (GameDataFunction::isRaceStartYukimaruTutorial(this)) {
            al::setNerve(this, &NrvStageScene.RaceYukimaruTutorial);
        } else if (mStateRaceManRace) {
            al::setNerve(this, &NrvStageScene.RaceManRace);
        } else if (GameDataFunction::isPlayDemoPlayerDownForBattleKoopaAfter(this)) {
            al::setNerve(this, &NrvStageScene.DemoPlayerDown);
            GameDataFunction::disablePlayDemoPlayerDownForBattleKoopaAfter(
                this);
        } else if (CapManHeroDemoUtil::isExistTalkDemoStageStart(this)) {
            al::setNerve(this, &NrvStageScene.DemoStageStartCapManHeroTalk);
        } else if (CapManHeroDemoUtil::isExistTalkDemoAfterMoonRockBreakDemo(this)) {
            al::setNerve(this, &NrvStageScene.DemoCapManHeroTalkMoonRock);
        } else if (GameDataFunction::isEnterStageFirst(mGameDataHolder) &&
                   al::isExistSceneObj<GiantWanderBoss>(this)) {
            GiantWanderBoss* giantWanderBoss = al::getSceneObj<GiantWanderBoss>(this);
            if (giantWanderBoss->isOnSwitchAll()) {
                rs::requestStartDemoWithPlayer(this);
                rs::addDemoActor(giantWanderBoss, false);
                giantWanderBoss->startDemoWorldEnter();
                rs::changeDemoCommonProc(this, mProjectItemDirector);
            }
        }
    }

    if (al::isNerve(this, &NrvStageScene.DemoTalk) ||
        al::isNerve(this, &NrvStageScene.DemoReturnToHome))
        mIsUpdateKitAndGraphics = true;

    if (mPyramid) {
        if (rs::isInvalidChangeStage(mPyramid)) {
            mPyramid->resetAtCloseAndFly();
        } else if (mScenarioNo == 2 && al::isNerve(this, &NrvStageScene.DemoReturnToHome)) {
            mPyramid->resetAtOpenAndGround();
        } else if (mScenarioNo < 2 || (mScenarioNo & ~1) == 8) {
            s32 activeQuestNo = rs::getActiveQuestNo(this);
            if (activeQuestNo >= 2)
                mPyramid->resetAtOpenAndGround();
            else
                mPyramid->resetAtCloseAndGround();
        } else {
            mPyramid->resetAtOpenAndFly();
        }
    }

    if (mTimeBalloonNpc && mStateTimeBalloon->isAutoStart())
        al::setNerve(this, &NrvStageScene.TimeBalloon);

    if (!al::isNerve(this, &NrvStageScene.StartStageBgm)) {
        rs::offRouteGuideSystem(this);
        rs::endPlayTalkMsgTimeBalloonLayout(this);
        if (_408)
            _408->disableLayout();
    }

    if (GameDataFunction::isMissEndPrevStageForInit(mGameDataHolder))
        mIsExistShineOrCoinCollectInStage = false;

    GameDataFunction::enteredStage(mGameDataHolder);

    if (rs::isShopStatusBuyShineMany(mDemoShine))
        mStateGetShine->setShopShine10();
    if (rs::isShopStatusBuyItem(mDemoShine))
        mIsUpdateKitAndGraphics = true;
    rs::setShopStatusNone(mDemoShine);

    OpeningStageStartDemo* openingDemo = mOpeningStageStartDemo;
    if (al::isNerve(this, &NrvStageScene.DemoStageStart) ||
        al::isNerve(this, &NrvStageScene.DemoStageStartOpening)) {
        openingDemo->startDemo();
    } else if (al::isNerve(this, &NrvStageScene.DemoStageStartCapManHeroTalk)) {
        CapManHeroDemoUtil::startTalkDemoStageStart(this);
    } else if (al::isNerve(this, &NrvStageScene.DemoCapManHeroTalkMoonRock)) {
        CapManHeroDemoUtil::startTalkDemoAfterMoonRockBreakDemo(this);
    }

    s32 mapIconStageInfoMapNum = al::getStageInfoMapNum(this);
    for (s32 i = 0; i < mapIconStageInfoMapNum; i++) {
        const al::StageInfo* mapIconStageInfoMap = al::getStageInfoMap(this, i);
        al::PlacementInfo mapIconPlacementInfo;
        s32 mapIconCount = 0;
        al::tryGetPlacementInfoAndCount(&mapIconPlacementInfo, &mapIconCount, mapIconStageInfoMap,
                                        "MapIconList");
        for (s32 j = 0; j < mapIconCount; j++) {
            al::PlacementInfo mapIconObjInfo;
            al::getPlacementInfoByIndex(&mapIconObjInfo, mapIconPlacementInfo, j);
            const char* mapIconName = nullptr;
            al::getObjectName(&mapIconName, mapIconObjInfo);
            if (al::isEqualString(mapIconName, "MapIconShop")) {
                GameDataFunction::setShopNpcTrans(mGameDataHolder, mapIconObjInfo);
            } else if (al::isEqualString(mapIconName, "MapIconMiniGame")) {
                GameDataFunction::setMiniGameInfo(mGameDataHolder, mapIconObjInfo);
            } else if (al::isEqualString(mapIconName, "MapIconPoetter")) {
                sead::Vector3f poetterTrans = sead::Vector3f::zero;
                al::getTrans(&poetterTrans, mapIconObjInfo);
                GameDataFunction::setPoetterTrans(mGameDataHolder, poetterTrans);
            }
        }
    }

    mChromakeyDrawer = new al::ChromakeyDrawer(getLiveActorKit()->getGraphicsSystemInfo(),
                                               al::getSceneExecuteDirector(this),
                                               getLiveActorKit()->getEffectSystem());

    GameDataFunction::noPlayDemoWorldWarp(this);
}

bool StageScene::tryChangeDemo() {
    al::PlayerHolder* playerHolder = al::getScenePlayerHolder(this);
    al::LiveActor* playerActor = al::getPlayerActor(playerHolder, 0);

    if (rs::isPlayerDamageStopDemo(playerActor)) {
        al::setNerve(this, &NrvStageScene.DemoDamageStopPlayer);
        return true;
    }
    if (rs::isActiveDemoSceneStartPlayerWalk(this)) {
        rs::changeDemoCommonProc(this, mProjectItemDirector);
        al::setNerve(this, &NrvStageScene.DemoSceneStartPlayerWalk);
        return true;
    }
    if (rs::isActiveDemoBirdFindMeat(this)) {
        al::setNerve(this, &NrvStageScene.DemoCarryMeat);
        return true;
    }
    if (rs::isExistDemoReturnToHome(this) && rs::requestStartDemoReturnToHome(this)) {
        al::stopPadRumble(this);
        rs::changeDemoCommonProc(this, mProjectItemDirector);
        al::setNerve(this, &NrvStageScene.DemoReturnToHome);
        return true;
    }
    if (rs::isExistDemoRiseMapPartsInList(this) && rs::requestStartDemoRiseMapParts(this)) {
        rs::killTutorial(this);
        al::stopPadRumble(this);
        rs::changeDemoCommonProc(this, mProjectItemDirector);
        al::setNerve(this, &NrvStageScene.DemoRiseMapParts);
        return true;
    }
    if (rs::isEnableStartShineChipCompleteDemo(this) && rs::requestStartDemoShineAppear(this)) {
        rs::killTutorial(this);
        rs::addDemoActorShineChipWatcher(this);
        rs::changeDemoCommonProc(this, mProjectItemDirector);
        al::setNerve(this, &NrvStageScene.DemoShineChipComplete);
        return true;
    }

    if (!rs::isActiveDemo(this))
        return false;

    if (rs::isActiveDemoHackStart(this)) {
        if (rs::isActiveDemoHackStartFirst(this)) {
            rs::changeDemoCommonProc(this, mProjectItemDirector);
            al::setNerve(this, &NrvStageScene.DemoHackStartFirst);
            return true;
        }
        rs::killTutorial(this);
        al::setNerve(this, &NrvStageScene.DemoHackStart);
        return true;
    }

    rs::changeDemoCommonProc(this, mProjectItemDirector);

    if (rs::isActiveDemoLaunchBazookaElectric(this)) {
        al::setNerve(this, &NrvStageScene.DemoNormalBazookaElectric);
        return true;
    }
    if (rs::isActiveDemoTalk(this)) {
        al::pausePadRumble(this);
        al::setNerve(this, &NrvStageScene.DemoTalk);
        return true;
    }
    if (rs::isActiveDemoTitleLogo(this)) {
        al::setNerve(this, &NrvStageScene.DemoTitleLogo);
        return true;
    }
    if (rs::isActiveDemoGetLifeMaxUpItem(this)) {
        al::setNerve(this, &NrvStageScene.DemoGetLifeMaxUpItem);
        return true;
    }
    if (rs::isActiveDemoShineMainGet(this)) {
        rs::killTutorial(this);
        al::stopPadRumble(this);
        al::setNerve(this, &NrvStageScene.DemoShineMainGet);
        return true;
    }
    if (rs::isActiveDemoShineGet(this)) {
        rs::killTutorial(this);
        al::stopPadRumble(this);
        al::setNerve(this, &NrvStageScene.DemoShineGet);
        return true;
    }
    if (rs::isActiveDemoShineGrandGet(this)) {
        rs::killTutorial(this);
        al::stopPadRumble(this);
        al::setNerve(this, &NrvStageScene.DemoShineGrandGet);
        return true;
    }
    if (rs::isActiveDemoWithPlayerUseCoin(this)) {
        al::stopPadRumble(this);
        al::setNerve(this, &NrvStageScene.DemoWithPlayerUseCoin);
        return true;
    }
    if (rs::isActiveDemoWithPlayer(this) || rs::isActiveDemoWithPlayerKeepCarry(this) ||
        rs::isActiveDemoWithPlayerCinemaFrame(this)) {
        al::stopPadRumble(this);
        al::setNerve(this, &NrvStageScene.DemoWithPlayer);
        return true;
    }
    al::setNerve(this, &NrvStageScene.DemoNormal);
    return true;
}

void StageScene::appear() {
    static bool sToggle = false;
    sToggle ^= true;

    alSeFunction::startSituationWithAutoEnd(getAudioDirector(), "シーン開始フェードイン", 0, 5, 45);
    alSeFunction::tryStartDefaultAtmosphereSe(mSePlayArea);

    const char* stageName = mStageName.cstr();
    al::PlayerHolder* playerHolder = al::getScenePlayerHolder(this);
    const sead::Vector3f& playerPos = al::getPlayerPos(playerHolder, 0);
    s64 playTimeTotal = GameDataFunction::getPlayTimeTotal(GameDataHolderAccessor(this));
    s64 saveDataId = GameDataFunction::getSaveDataIdForPrepo(GameDataHolderAccessor(this));
    s64 playTimeAcrossFile = GameDataFunction::getPlayTimeAcrossFile(GameDataHolderAccessor(this));
    rs::trySavePrepoStartStage(stageName, playerPos, playTimeTotal, saveDataId, playTimeAcrossFile);

    al::Scene::appear();

    al::GameFrameworkNx* gameFramework = Application::instance()->getGameFramework();
    sead::GameFramework* framework = gameFramework;
    al::GameFrameworkNx* frameworkNx =
        framework->checkDerivedRuntimeTypeInfo(al::GameFrameworkNx::getRuntimeTypeInfoStatic()) ?
            gameFramework :
            nullptr;
    frameworkNx->disableClearRenderBuffer();

    if (rs::isModeDiverRom() || rs::isModeE3MovieRom() || rs::isModeMovieRom())
        alSeFunction::startSituation(getAudioDirector(), "プレゼン用環境SE下げ", -1);

    if (al::isPlayingBgm(this, "Boss"))
        al::stopBgm(this, "Boss", -1);
    if (al::isPlayingBgm(this, "GateKeeper"))
        al::stopBgm(this, "GateKeeper", -1);
    if (al::isPlayingBgm(this, "SlotGame"))
        al::stopBgm(this, "SlotGame", -1);
    if (al::isPlayingBgm(this, "LiveTestAll"))
        al::stopBgm(this, "LiveTestAll", -1);
    if (al::isPlayingBgm(this, "LiveTest"))
        al::stopBgm(this, "LiveTest", -1);
    if (al::isPlayingBgm(this, "SphinxRide"))
        al::stopBgm(this, "SphinxRide", -1);
    if (al::isPlayingBgm(this, "TimeBalloon"))
        al::stopBgm(this, "TimeBalloon", -1);
    if (al::isPlayingBgm(this, "Climax"))
        al::stopBgm(this, "Climax", -1);
    if (al::isPlayingBgm(this, "RaceResult"))
        al::stopBgm(this, "RaceResult", -1);
    if (al::isPlayingBgm(this, "DamageBall"))
        al::stopBgm(this, "DamageBall", -1);
    if (al::isPlayingBgm(this, "MiniGame"))
        al::stopBgm(this, "MiniGame", -1);
    if (al::isPlayingBgm(this, "RadioCassette"))
        al::stopBgm(this, "RadioCassette", -1);
    if (al::isPlayingBgm(this, "Invincible"))
        al::stopBgm(this, "Invincible", -1);
    if (al::isPlayingBgm(this, "HackDinosaur"))
        al::stopBgm(this, "HackDinosaur", -1);
    if (al::isPlayingBgm(this, "ChaseJango"))
        al::stopBgm(this, "ChaseJango", -1);

    if (al::isPlayingBgm(this, "Shop")) {
        if (!al::isEqualSubString(mStageName.cstr(), "ShopStage") &&
            !al::isEqualSubString(mStageName.cstr(), "ShopRoom") &&
            !al::isEqualString(mStageName.cstr(), "CityWorldShop01Stage") &&
            !al::isEqualString(mStageName.cstr(), "CapWorldHomeStage") &&
            !al::isEqualString(mStageName.cstr(), "WaterfallWorldHomeStage") &&
            !al::isEqualString(mStageName.cstr(), "ForestWorldHomeStage") &&
            !al::isEqualString(mStageName.cstr(), "SeaWorldHomeStage"))
            al::stopBgm(this, "Shop", -1);
    }

    if (al::isEqualString(mStageName.cstr(), "Special2WorldHomeStage") &&
        al::isPlayingBgm(this, "CollectBgm"))
        stopCollectBgm();

    al::startAudioEffect(this, "DistanceReverbSe");
    al::startAudioEffect(this, "DistanceReverbBgm");
    al::startAudioEffect(this, "BgmLevelDetection");

    if (!al::isNerve(this, &NrvStageScene.StartStageBgm))
        al::startAudioEffectDefaultArea(this);
}

void StageScene::control() {
    mSceneLayout->updatePlayGuideMenuText();
    mSceneLayout->updateKidsModeLayout();
    rs::isExistRequestShowHtmlViewer(this);

    if (rs::getRequestWipeCloseInfoName(this)) {
        mWipeHolder->startCloseByInfo(rs::getRequestWipeCloseInfoName(this));
        rs::requestWipeClose(this, nullptr);
    }

    if (rs::tryReceiveRequestWipeOpenImmidiate(this)) {
        if (mWipeHolder->get_18())
            mWipeHolder->startOpen(10);
    }

    if (!isIgnoreAddPlayTime()) {
        mGameDataHolder->getGameDataFile()->addPlayTime(1, this);
        mGameDataHolder->addPlayTimeAcrossFile();
    }

    if (PlayerCameraFunction::isTriggerCameraSubjective(mCheckpointWarpCapActor) &&
        PlayerCameraFunction::isFailureCameraSubjective(mCheckpointWarpCapActor)) {
        mSceneLayout->appearPlayGuideCamera();
    }

    mDemoSoundSynchronizer->update();
    mDemoSoundSynchronizer->trySync(mDemoSynchronizerTimer, mDemoSyncedEventKeeper);
    mAudioSystemPauseController->update();

    if (!al::isPlayingBgm(this, "CollectBgm"))
        mCollectBgmPlayer->reset();

    mCollectBgmRegister->update();
    mBgmAnimeSyncDirector->update();

    if (mSceneLayout->isActive() && !al::isNerve(this, &NrvStageScene.DemoHackStart) &&
        !al::isNerve(this, &NrvStageScene.DemoGetLifeMaxUpItem)) {
        mSceneLayout->control();
    }

    mSceneLayout->updateLifeCounter();

    if (al::isNerve(this, &NrvStageScene.Play))
        GameDataFunction::getPlayerHitPoint(mGameDataHolder);

    al::DemoDirector* demoDirector = getLiveActorKit()->getDemoDirector();
    if (demoDirector) {
        const char* activeDemoName = *(const char**)demoDirector->get_30();
        if (!activeDemoName)
            activeDemoName = demoDirector->getActiveDemoName();
        const char* prevDemoName = mActiveDemoName;
        mActiveDemoName = activeDemoName;

        if (demoDirector->isActiveDemo() && (activeDemoName || prevDemoName)) {
            if ((activeDemoName != nullptr && prevDemoName == nullptr) ||
                (activeDemoName == nullptr && prevDemoName != nullptr) ||
                !al::isEqualString(activeDemoName, prevDemoName)) {
                al::DemoSyncedEventKeeper* keeper = mDemoSyncedEventKeeper;
                if (prevDemoName) {
                    keeper->endDemo(true, false);
                    mDemoSoundSynchronizer->endSync();
                    keeper = mDemoSyncedEventKeeper;
                }
                keeper->startDemo(activeDemoName);
                mDemoSoundSynchronizer->startSync();
                mDemoSynchronizerTimer = 0;
            } else if (prevDemoName) {
                if (!demoDirector->isActiveDemo()) {
                    mDemoSyncedEventKeeper->endDemo(false, false);
                    mDemoSoundSynchronizer->endSync();
                }
            }
        } else if (prevDemoName) {
            if (!demoDirector->isActiveDemo()) {
                mDemoSyncedEventKeeper->endDemo(false, false);
                mDemoSoundSynchronizer->endSync();
            }
        }

        if (demoDirector->isActiveDemo() && mStateSkipDemo->isDead()) {
            mDemoSyncedEventKeeper->updateOnlyWhenDemoRun(mDemoSynchronizerTimer);
            ++mDemoSynchronizerTimer;
        }
    }

    al::DemoDirector* demoDirector2 = getLiveActorKit()->getDemoDirector();
    al::PlayerHolder* playerHolder = al::getScenePlayerHolder(this);
    al::LiveActor* playerActor = al::getPlayerActor(playerHolder, 0);
    bool isInvincible = rs::isPlayerActiveMarioAmiiboInvincible(playerActor);

    bool isValidNerve;
    if (al::isNerve(this, &NrvStageScene.Play))
        isValidNerve = true;
    else
        isValidNerve = al::isNerve(this, &NrvStageScene.DemoTalk) ||
                       al::isNerve(this, &NrvStageScene.CollectionList);

    if (al::isPlayingBgm(this, "Invincible")) {
        if (!isInvincible || !(isValidNerve | !demoDirector2->isActiveDemo()))
            al::stopBgm(this, "Invincible", 120);
    } else if (isInvincible && !demoDirector2->isActiveDemo() && isValidNerve) {
        al::BgmPlayingRequest request("Invincible");
        request._18 = 120;
        request._1c = 0;
        request._20 = 120;
        request._24 = 0;
        request._28 = 0;
        al::startBgm(this, request, false, false);
    }
}

bool StageScene::isIgnoreAddPlayTime() const {
    if (!mStateSnapShot->isDead())
        return true;
    return al::isNerve(this, &NrvStageScene.Pause);
}

void StageScene::kill() {
    al::Scene::kill();
    al::enableBgmLineChange(this);

    if (!GameDataFunction::isMissEndPrevStageForSceneDead(GameDataHolderAccessor(this))) {
        const char* stageName = mStageName.cstr();
        al::PlayerHolder* playerHolder = al::getScenePlayerHolder(this);
        const sead::Vector3f& playerPos = al::getPlayerPos(playerHolder, 0);
        s64 playTimeTotal = GameDataFunction::getPlayTimeTotal(GameDataHolderAccessor(this));
        s64 saveDataId = GameDataFunction::getSaveDataIdForPrepo(GameDataHolderAccessor(this));
        s64 playTimeAcrossFile =
            GameDataFunction::getPlayTimeAcrossFile(GameDataHolderAccessor(this));
        rs::trySavePrepoExitStage(stageName, playerPos, playTimeTotal, saveDataId,
                                  playTimeAcrossFile);
    }

    if (al::isPlayingBgm(this, "CollectBgm")) {
        if (GameDataFunction::isGoToCeremonyFromInsideHomeShip(GameDataHolderAccessor(this)))
            rs::stopCollectBgm(this, 90);
    }

    mDemoSoundSynchronizer->endSync();
    mProjectNfpDirector->stop();
}

bool StageScene::isGetGrandShineAndEnableWipeClose() const {
    if (isGetGrandShine())
        return mStateGetShineGrand->isEnableWipeClose();
    return false;
}

bool StageScene::isGetGrandShine() const {
    return al::isNerve(this, &NrvStageScene.DemoShineGrandGet);
}

bool StageScene::isVeilEnd() const {
    return isGetGrandShine() || GameDataFunction::getSessionEventProgress(mGameDataHolder) ==
                                    SessionEventProgress::TheCeremonyIsReady;
}

bool StageScene::isE3End() const {
    return isGetGrandShine() || rs::getActiveQuestNo(this) == 4;
}

bool StageScene::isDefeatKoopaLv1() const {
    if (mKoopaLv1)
        return al::isDead(mKoopaLv1);
    return false;
}

bool StageScene::isMissEnd() const {
    if (al::isNerve(this, &NrvStageScene.Miss))
        return true;
    if (mTimeBalloonNpc && mStateTimeBalloon->isMissNerve())
        return true;
    return false;
}

bool StageScene::isHackEnd() const {
    return rs::isPlayerHack(al::getPlayerActor(al::getScenePlayerHolder(this), 0)) &&
           !rs::isPlayerHackBazookaElectric(al::getPlayerActor(al::getScenePlayerHolder(this), 0));
}

bool StageScene::isLoadData() const {
    return mStagePauseMenu->isLoadData();
}

bool StageScene::isModeSelectEnd() const {
    return mStagePauseMenu->isModeSelectEnd();
}

bool StageScene::isNewGame() const {
    return mStagePauseMenu->isNewGame();
}

bool StageScene::isChangeLanguage() const {
    return mStagePauseMenu->isChangeLanguage();
}

const char* StageScene::getLanguage() const {
    return mStagePauseMenu->getLanguage();
}

bool StageScene::isEnableSave() const {
    if (al::isNerve(this, &NrvStageScene.DemoStageStartOpening))
        return true;
    if (rs::isInvalidSaveStage(mStageName.cstr()))
        return false;
    if (al::isNerve(this, &NrvStageScene.Pause))
        return true;
    return al::isNerve(this, &NrvStageScene.Play) && al::isGreaterEqualStep(this, 10);
}

bool StageScene::isPause() const {
    return !mStagePauseMenu->isDead();
}

// STP vs STR for ChromakeyDrawer physical area
void StageScene::drawMain() const {
    agl::DrawContext* drawContext = al::getSceneDrawContext(this);

    mLayoutTextureRenderer->drawToTexture();

    auto* moviePlayer = static_cast<MoviePlayer*>(al::tryGetSceneObj(this, SceneObjID_MoviePlayer));
    if (moviePlayer)
        moviePlayer->draw(drawContext);

    agl::RenderBuffer* frameBuffer = al::getSceneFrameBufferMain(this);
    sead::Viewport viewport(*frameBuffer);
    viewport.apply(drawContext, *frameBuffer);

    al::ViewRenderer* viewRenderer = getLiveActorKit()->getGraphicsSystemInfo()->getViewRenderer();

    const sead::LookAtCamera& lookAtCamera = al::getLookAtCamera(this, 0);
    sead::LookAtCamera cameraOnStack(lookAtCamera);

    const al::Projection& projection = al::getProjection(this, 0);
    al::Projection projectionCopy(projection);

    al::tryChangeShaderMode(drawContext, (agl::ShaderMode)4);

    bool isOddSpace = isEnableToDrawOddSpace();
    viewRenderer->set_50(isOddSpace);

    al::SubCameraRenderer* subCameraRenderer =
        getLiveActorKit()->getGraphicsSystemInfo()->getSubCameraRenderer();
    if (subCameraRenderer)
        subCameraRenderer->tryCapture();

    al::setRenderBuffer(getLayoutKit(), frameBuffer);

    StageSceneStatePauseMenu* stagePauseMenu = mStagePauseMenu;
    bool isDrawViewRenderer;
    if (!stagePauseMenu->isDead()) {
        isDrawViewRenderer = stagePauseMenu->isDrawViewRenderer();
    } else {
        StageSceneStateGetShineMain* stateGetShineMain = mStateGetShineMain;
        StageSceneStateGetShineGrand* stateGetShineGrand = mStateGetShineGrand;
        StageSceneStateShop* stateShop = mStateShop;
        StageSceneStateCloset* stateCloset = mStateCloset;

        if (!al::isNerve(this, &NrvStageScene.Shop)) {
            if ((!stateGetShineMain->isDead() && stateGetShineMain->isDrawChromakey()) ||
                (!stateGetShineGrand->isDead() && stateGetShineGrand->isDrawChromakey()) ||
                (!stagePauseMenu->isDead() && stagePauseMenu->isDrawChromakey()) ||
                al::isNerve(this, &NrvStageScene.Shop) ||
                al::isNerve(this, &NrvStageScene.Closet)) {
                if (!al::isNerve(this, &NrvStageScene.Shop))
                    if (!al::isNerve(this, &NrvStageScene.Closet))
                        isDrawViewRenderer = false;
                    else
                        isDrawViewRenderer = stateCloset->isDrawViewRenderer();
                else
                    isDrawViewRenderer = al::isLessEqualStep(this, 2);
            } else if (!al::isNerve(this, &NrvStageScene.CollectBgm) &&
                       !al::isNerve(this, &NrvStageScene.CollectionList)) {
                isDrawViewRenderer = true;
            } else {
                isDrawViewRenderer = al::isLessEqualStep(this, 1);
            }
        } else if (!al::isLessEqualStep(this, 2)) {
            isDrawViewRenderer = stateShop->isDrawViewRenderer();
        } else {
            isDrawViewRenderer = true;
        }
    }

    bool isDrawChromakey;
    if (!mStateGetShineMain->isDead() && mStateGetShineMain->isDrawChromakey())
        isDrawChromakey = true;
    else if (!mStateGetShineGrand->isDead() && mStateGetShineGrand->isDrawChromakey())
        isDrawChromakey = true;
    else if (!stagePauseMenu->isDead() && stagePauseMenu->isDrawChromakey())
        isDrawChromakey = true;
    else if (al::isNerve(this, &NrvStageScene.Shop))
        isDrawChromakey = true;
    else
        isDrawChromakey = al::isNerve(this, &NrvStageScene.Closet);

    bool isPauseOrOverlay;
    if (al::isNerve(this, &NrvStageScene.Pause))
        isPauseOrOverlay = true;
    else if (!stagePauseMenu->isDead())
        isPauseOrOverlay = true;
    else if (mStateTimeBalloon)
        isPauseOrOverlay = mStateTimeBalloon->isTimeBalloonNerve();
    else
        isPauseOrOverlay = false;

    if (isDrawViewRenderer) {
        PlayerHackStartTexKeeper* hackStartTexKeeper = nullptr;
        bool isHackStart = false;
        if (al::isExistSceneObj(this, SceneObjID_PlayerHackStartTexKeeper)) {
            hackStartTexKeeper = static_cast<PlayerHackStartTexKeeper*>(
                al::tryGetSceneObj(this, SceneObjID_PlayerHackStartTexKeeper));
            hackStartTexKeeper->clearHackStartTextureOnlyFirstTime(drawContext);
            hackStartTexKeeper->activateHackStartTexture(drawContext,
                                                         al::getSamplerLocationHackStart());
            viewRenderer->set_60(hackStartTexKeeper->getTextureData());
            isHackStart = hackStartTexKeeper->isEnabled();
        }

        al::DrawSystemInfo* drawSystemInfo = getDrawSystemInfo();
        bool isDrawForward;
        if (!mStateGetShine->isDead() && mStateGetShine->isDrawForward()) {
            isDrawForward = true;
        } else {
            isDrawForward = false;
            const al::PlayerHolder* playerHolder = al::getScenePlayerHolder(this);
            if (al::getPlayerNumMax(playerHolder)) {
                al::LiveActor* player = al::getPlayerActor(playerHolder, 0);
                isDrawForward = PlayerFunction::isPlayerDeadDrawForward(player);
            }
        }

        viewRenderer->drawView(0, drawSystemInfo, projectionCopy, cameraOnStack, frameBuffer,
                               viewport, true, isDrawForward, isHackStart);
        if (hackStartTexKeeper)
            hackStartTexKeeper->disable();
    }

    if (isDrawChromakey) {
        viewport.apply(drawContext, *frameBuffer);
        frameBuffer->bind(drawContext);

        const char* chromakeyName = "３Ｄ（クロマキーキャラクター）";
        if (!al::isNerve(this, &NrvStageScene.Shop) && stagePauseMenu->isDead())
            chromakeyName = "３Ｄ（クロマキープレイヤー）";

        const char* zPrepassName = "Ｚプリパス[ＮＰＣクロマキー]";
        if (!al::isNerve(this, &NrvStageScene.Shop) && stagePauseMenu->isDead())
            zPrepassName = "Ｚプリパス[プレイヤークロマキー]";

        if (!isDrawViewRenderer)
            frameBuffer->clear(drawContext, 3, sead::Color4f::cBlack, al::getDepthClearValue(), 0);

        if (!mStateShop->isDead())
            al::drawKitList(this, "２Ｄベース（メイン画面）", "２Ｄ");
        al::drawKit(this, "２Ｄバック（メイン画面）");

        const sead::BoundBox2f& physArea = frameBuffer->getPhysicalArea();
        mChromakeyDrawer->setPhysicalArea(physArea.getMax() - physArea.getMin());
        mChromakeyDrawer->drawChromakey(drawContext, projectionCopy, cameraOnStack, frameBuffer,
                                        viewport, chromakeyName, zPrepassName, nullptr);
    } else if (isPause()) {
        viewport.apply(drawContext, *frameBuffer);
        frameBuffer->bind(drawContext);
        frameBuffer->clear(drawContext, 3, sead::Color4f::cBlack, al::getDepthClearValue(), 0);
        al::drawKit(this, "２Ｄバック（メイン画面）");
    }

    f32 layoutWidth = al::getLayoutDisplayWidth();
    f32 layoutHeight = al::getLayoutDisplayHeight();
    sead::Viewport layoutViewport(0.0f, 0.0f, layoutWidth, layoutHeight);
    layoutViewport.apply(drawContext, *frameBuffer);
    frameBuffer->bind(drawContext);
    StageSceneStateTimeBalloon* stateTimeBalloon = mStateTimeBalloon;
    StageSceneStateScenarioCamera* stateScenarioCamera = mStateScenarioCamera;
    StageSceneStateGetShineMain* stateGetShineMain = mStateGetShineMain;
    StageSceneStateGetShineGrand* stateGetShineGrand = mStateGetShineGrand;
    StageSceneStateWorldIntroCamera* stateWorldIntroCamera = mStateWorldIntroCamera;

    if (rs::isModeE3MovieRom()) {
        al::drawKit(this, "２Ｄ（撮影用）");
    } else if (rs::isModeDiverOrJungleGymRom()) {
        // skip
    } else if (!mStateSnapShot->isDead()) {
        al::drawKit(this, "２Ｄ（スナップショット）");
    } else if (!mStateMiss->isDead()) {
        al::drawKit(this, "２Ｄ（ミス）");
        al::drawKitList(this, "２Ｄオーバー（メイン画面）", "２Ｄワイプ");
    } else if (isPauseOrOverlay || !mStateCollectionList->isDead() ||
               (mStateCollectBgm && !mStateCollectBgm->isDead()) ||
               (mStateMiniGameRanking && !mStateMiniGameRanking->isDead()) ||
               (mStateRaceManRace && mStateRaceManRace->isPause()) ||
               (mStateYukimaruRace && mStateYukimaruRace->isPause()) ||
               (mStateYukimaruRaceTutorial && mStateYukimaruRaceTutorial->isPause())) {
        if (mStateWorldMap && !mStateWorldMap->isDead())
            al::drawKit(this, "２Ｄ（ワールドマップ画面）");
        al::drawKit(this, "２Ｄオーバー（メイン画面）");
    } else if (!mStateShop->isDead()) {
        al::drawKit(this, "２Ｄオーバー（メイン画面）");
        al::drawKitList(this, "２Ｄベース（メイン画面）", "２Ｄカウンター");
    } else if (mStateWorldMap && !mStateWorldMap->isDead()) {
        al::drawKit(this, "２Ｄ（ワールドマップ画面）");
        al::drawKit(this, "２Ｄオーバー（メイン画面）");
    } else if (!mStateSkipDemo->isDead() || rs::isActiveDemoWithPlayerCinemaFrame(this) ||
               rs::isActiveDemoNormalWithCinemaFrame(this) || rs::isActiveDemoBirdFindMeat(this) ||
               rs::isActiveDemoHackStartFirst(this)) {
        al::drawKit(this, "２Ｄ（デモ画面）");
        al::drawKitList(this, "２Ｄベース（メイン画面）", "２Ｄカウンター");
        al::drawKitList(this, "２Ｄオーバー（メイン画面）", "２Ｄウィンドウ");
        al::drawKitList(this, "２Ｄオーバー（メイン画面）", "２Ｄウィンドウカウンター");
        al::drawKitList(this, "２Ｄオーバー（メイン画面）", "２Ｄワイプ");
    } else if (isDrawChromakey) {
        if (!al::isNerve(this, &NrvStageScene.Shop) && !isPause()) {
            if (mStateGetShineMain->isDead() && mStateGetShineGrand->isDead())
                al::drawKit(this, "２Ｄオーバー（メイン画面）");
            else {
                al::drawKit(this, "２Ｄ（ムーンゲット画面）");
                al::drawKit(this, "２Ｄオーバー（メイン画面）");
            }
        } else {
            al::drawKit(this, "２Ｄオーバー（メイン画面）");
        }
    } else if (al::isNerve(this, &NrvStageScene.DemoWithPlayerUseCoinNoFirst) &&
               al::isActive(mMapLayout)) {
        al::drawKit(this, "２Ｄオーバー（メイン画面）");
    } else if (al::isNerve(this, &NrvStageScene.DemoNormal) ||
               al::isNerve(this, &NrvStageScene.DemoNormalBazookaElectric) ||
               al::isNerve(this, &NrvStageScene.DemoWithPlayer) ||
               al::isNerve(this, &NrvStageScene.DemoWithPlayerUseCoinNoFirst) ||
               al::isNerve(this, &NrvStageScene.DemoWithPlayerUseCoin) ||
               al::isNerve(this, &NrvStageScene.DemoCountCoin) ||
               al::isNerve(this, &NrvStageScene.DemoShineChipComplete) ||
               al::isNerve(this, &NrvStageScene.DemoHackStartFirst) ||
               al::isNerve(this, &NrvStageScene.WaitWarpToCheckpoint) ||
               al::isNerve(this, &NrvStageScene.WarpToCheckpoint) ||
               al::isNerve(this, &NrvStageScene.ArriveAtCheckpoint) ||
               al::isNerve(this, &NrvStageScene.DemoShineGet) ||
               al::isNerve(this, &NrvStageScene.DemoTalk) || !stateScenarioCamera->isDead() ||
               !stateWorldIntroCamera->isDead() ||
               (al::isNerve(this, &NrvStageScene.TimeBalloon) &&
                stateTimeBalloon->isPauseNerve()) ||
               !stateGetShineMain->isDead() || !stateGetShineGrand->isDead()) {
        al::drawKit(this, "２Ｄ（デモ画面）");
        al::drawKitList(this, "２Ｄオーバー（メイン画面）", "２Ｄウィンドウ");
        al::drawKitList(this, "２Ｄオーバー（メイン画面）", "２Ｄワイプ");
    } else if (!rs::isPlayerHackFukankun(mDemoShine)) {
        OpeningStageStartDemo* openingDemo = mOpeningStageStartDemo;
        if (al::isNerve(this, &NrvStageScene.DemoStageStartOpening) &&
            !openingDemo->isDrawLayout()) {
            // skip layout draw
        } else {
            al::drawKit(this, "２Ｄベース（メイン画面）");
            al::drawKit(this, "２Ｄオーバー（メイン画面）");
            al::drawKit(this, "２Ｄ（スナップショット）");
        }
    } else {
        al::drawKitList(this, "２Ｄベース（メイン画面）", "２Ｄフィルターバルーン");
        al::drawKitList(this, "２Ｄベース（メイン画面）", "２Ｄフィルター");
        al::drawKitList(this, "２Ｄベース（メイン画面）", "２Ｄガイド");
        if (!mStateCollectionList->isDead() || al::isNerve(this, &NrvStageScene.RaceManRace))
            al::drawKit(this, "２Ｄオーバー（メイン画面）");
    }

    al::executeDrawEffect(getLayoutKit());
}

void StageScene::stopCollectBgm() {
    rs::stopCollectBgm(this, -1);
}

bool StageScene::isEnableToDrawOddSpace() const {
    al::LiveActor* player = al::getPlayerActor(al::getScenePlayerHolder(this), 0);
    if (player)
        return rs::isPlayerEnableToSeeOddSpace(player);
    return false;
}

bool StageScene::updatePlay() {
    if (rs::updateNormalStateExcludeGraphics(this))
        return true;

    al::LiveActor* player = al::getPlayerActor(al::getScenePlayerHolder(this), 0);
    al::AreaObj* area = al::tryFindAreaObj(player, "MissRestartArea", al::getTrans(player));
    if (area)
        mGameDataHolder->getGameDataFile()->setMissRestartInfo(*area->getPlacementInfo());

    mLocationNameCtrl->update();

    if (al::isNerve(this, &NrvStageScene.Miss))
        return false;
    if (mStateMiss->checkMiss()) {
        al::setNerve(this, &NrvStageScene.Miss);
        return true;
    }
    return false;
}

void StageScene::endDemoAndChangeNerve() {
    rs::endDemoCommonProc(this, mProjectItemDirector);
    if (rs::tryChangeNextStage(mGameDataHolder, this)) {
        kill();
        return;
    }
    if (rs::isActiveDemo(this)) {
        updateKitListPostAndChangeNextNerve(&NrvStageScene.Play);
    } else {
        al::activateAudioEventController(this);
        updateKitListPostAndChangeNextNerve(&NrvStageScene.StartStageBgm);
    }
}

void StageScene::updateKitListPostAndChangeNextNerve(const al::Nerve* nerve) {
    al::updateKitListPostOnNerveEnd(this);
    al::setNerve(this, nerve);
}

void StageScene::exeStartStageBgm() {
    updatePlay();
    al::updateKitListPostOnNerveEnd(this);
    al::activateAudioEventController(this);
    positionAudioControl(this);
    al::setNerve(this, &NrvStageScene.Play);
    if (isDefeatKoopaLv1())
        kill();
}

void StageScene::exePlay() {
    if (al::isFirstStep(this)) {
        mGameDataHolder->validateSaveForMoonGet();
        rs::onRouteGuideSystem(this);
        rs::startTalkMsgTimeBalloonLayout(this);

        if (_408)
            _408->enableLayout();

        rs::requestShowCapMessageGetMoon(this);
        mStateWarp->tryValidateEndEntranceCamera();

        if (mIsExistShineOrCoinCollectInStage) {
            if (mGameDataHolder->checkNeedTreasureMessageStage(mStageName.cstr())) {
                mIsExistShineOrCoinCollectInStage = false;

                if (GameDataFunction::isGameClear(mGameDataHolder) &&
                    rs::checkAlreadyVisitStage(mGameDataHolder, mStageName.cstr()) &&
                    mGameDataHolder->getGameDataFile()->calcGetShineNumByStageName(
                        mStageName.cstr()) >= 1) {
                    al::StringTmp<128> msgBuffer;

                    bool isExistCollectCoin;
                    if (al::isExistSceneObj(this, SceneObjID_CoinCollectWatcher)) {
                        CoinCollectWatcher* watcher = al::getSceneObj<CoinCollectWatcher>(this);
                        isExistCollectCoin =
                            (s32)watcher->getRegisteredCoins() > (s32)watcher->getCoinUpCount();
                    } else {
                        isExistCollectCoin = false;
                    }

                    GameDataFile* gameDataFile = mGameDataHolder->getGameDataFile();
                    if (gameDataFile->calcRestShineInStageWithWorldProgress(mStageName.cstr())) {
                        msgBuffer.format("AlreadyVisitStage_ExistShine");
                        if (isExistCollectCoin)
                            msgBuffer.appendWithFormat("_ExistCollectCoin");
                    } else if (isExistCollectCoin) {
                        msgBuffer.format("AlreadyVisitStage_ExistCollectCoin");
                    } else {
                        msgBuffer.format("AlreadyVisitStage_NoShine");
                    }

                    if (!GameDataFunction::isEnableCap(mGameDataHolder))
                        msgBuffer.appendWithFormat("_NoCap");

                    rs::showCapMessageEnterExStage(this, msgBuffer.cstr(), 90, 90);
                } else {
                    rs::visitStage(mGameDataHolder, mStageName.cstr());
                }
            }
        }
    }

    rs::tryShowCapMsgMoonGetSpecial1(this);
    rs::tryShowCapMsgMoonGetSpecial2(this);

    if (al::isStep(this, 2)) {
        if (!mSceneLayout->isWait() && !mIsPlayerHackFukankun && !mIsKoopaHackDemo)
            mSceneLayout->start();

        if (rs::isPlayerHackFukankun(mDemoShine)) {
            if (!mIsPlayerHackFukankun)
                mSceneLayout->end();
            mIsPlayerHackFukankun = true;
        } else {
            mIsPlayerHackFukankun = false;
        }
    }

    if (al::isStep(this, 90) && GameDataFunction::isHomeShipStage(mGameDataHolder))
        GameDataFunction::recoveryPlayerMax(mCheckpointWarpCapActor);

    if (!rs::isPlayerHackFukankun(mDemoShine) && mIsPlayerHackFukankun) {
        mSceneLayout->start();
        mIsPlayerHackFukankun = false;
    }

    updateStageSceneLayoutForKoopaHack();

    if (updatePlay()) {
        if (al::isStopScene(this))
            tryChangeSnapShot();
        al::updateKitListPostOnNerveEnd(this);
        return;
    }

    if (isEnableSave())
        mGameDataHolder->updateRequireSaveFrame();

    if (al::isExistSceneObj(this, SceneObjID_FukankunZoomObjHolder)) {
        FukankunZoomObjHolder* fukankunHolder = al::getSceneObj<FukankunZoomObjHolder>(this);
        fukankunHolder->execute();
    }

    if (rs::tryChangeNextStage(mGameDataHolder, this)) {
        kill();
        return;
    }

    al::PlayerHolder* playerHolder = al::getScenePlayerHolder(this);
    if (al::getPlayerNumMax(playerHolder) >= 1) {
        playerHolder = al::getScenePlayerHolder(this);
        al::LiveActor* player = al::getPlayerActor(playerHolder, 0);
        if (player) {
            al::IUseAreaObj* areaUser = player;
            const sead::Vector3f& trans = al::getTrans(player);
            al::AreaObj* areaObj = al::tryFindAreaObj(areaUser, "RestartArea", trans);
            if (areaObj) {
                al::PlacementInfo placementInfo;
                al::tryGetLinksInfo(&placementInfo, *areaObj->getPlacementInfo(),
                                    "PlayerRestartInfo");
                al::PlacementId placementId;
                placementId.init(placementInfo);
                GameDataFunction::setRestartPointId(mGameDataHolder, &placementId);
            }
        }
    }

    if (tryChangeDemo()) {
        al::updateKitListPostOnNerveEnd(this);
        return;
    }

    if (tryStartWarp()) {
        al::updateKitListPostOnNerveEnd(this);
        return;
    }

    if (tryChangeSnapShot()) {
        al::updateKitListPostOnNerveEnd(this);
        return;
    }

    if (!rs::isModeE3Rom() && !rs::isModeE3LiveRom())
        mHelpAmiiboDirector->execute();

    if (tryChangePause() || tryChangeList()) {
        al::updateKitListPostOnNerveEnd(this);
        return;
    }

    if (mSceneLayout->isWait())
        updateStageInfoLayout();

    if (mCollectionList && mCollectionList->isOpen()) {
        updateKitListPostAndChangeNextNerve(&NrvStageScene.CollectionList);
        return;
    }

    if (mStateCollectBgm && mStateCollectBgm->tryOpenCollectBgm()) {
        updateKitListPostAndChangeNextNerve(&NrvStageScene.CollectBgm);
        return;
    }

    if (mTimeBalloonNpc) {
        if (al::isFirstStep(this))
            mTimeBalloonDirector->requestDownload();
        mTimeBalloonDirector->execute();

        if (mStateTimeBalloon->isTimeBalloonNerve()) {
            updateKitListPostAndChangeNextNerve(&NrvStageScene.TimeBalloon);
            return;
        }
    }

    bool isTriggerAmiibo = AmiiboFunction::isTriggerTouchAmiiboAll(this);
    al::updateKitListPostOnNerveEnd(this);
    if (isTriggerAmiibo)
        al::setNerve(this, &NrvStageScene.Amiibo);
}

void StageScene::exeDemoGetLifeMaxUpItem() {
    if (tryUpdateAllForFirst())
        return;

    if (al::updateNerveState(this)) {
        rs::endDemoCommonProc(this, mProjectItemDirector);
        updateKitListPostAndChangeNextNerve(&NrvStageScene.Play);
        return;
    }

    al::updateKitListPostOnNerveEnd(this);
}

void StageScene::exeRaceManRace() {
    if (al::updateNerveState(this))
        kill();
    else
        al::updateKitListPostOnNerveEnd(this);
}

void StageScene::exeRaceYukimaru() {
    if (al::updateNerveState(this))
        kill();
    else
        al::updateKitListPostOnNerveEnd(this);
}

void StageScene::exeRaceYukimaruTutorial() {
    if (al::updateNerveState(this))
        kill();
    else
        al::updateKitListPostOnNerveEnd(this);
}

void StageScene::exeRadicon() {
    if (al::updateNerveState(this)) {
        if (mStateRadicon->isKill()) {
            kill();
            return;
        }
        updateKitListPostAndChangeNextNerve(&NrvStageScene.Play);
        return;
    }
    al::updateKitListPostOnNerveEnd(this);
}

void StageScene::exeSnapShot() {
    bool updated = al::updateNerveState(this);
    al::updateKitListPostOnNerveEnd(this);
    if (updated)
        al::setNerve(this, &NrvStageScene.Play);
}

void StageScene::exeCollectionList() {
    al::isFirstStep(this);
    if (al::updateNerveState(this)) {
        if (mCollectionList)
            mCollectionList->close();
        if (mStateCollectionList->getStateStageMap()->isEndCheckpointWarp()) {
            updateKitListPostAndChangeNextNerve(&NrvStageScene.WaitWarpToCheckpoint);
            return;
        }
        mStateCollectionList->getStateStageMap()->isEndClose();
        updateKitListPostAndChangeNextNerve(&NrvStageScene.Play);
        return;
    }
    al::updateKitListPostOnNerveEnd(this);
}

void StageScene::exeDemoCarryMeat() {
    if (al::isFirstStep(this))
        mSceneLayout->end();
    if (al::updateNerveState(this)) {
        mStateSkipDemo->tryEndForNoSkip();
        updateKitListPostAndChangeNextNerve(&NrvStageScene.Play);
    } else {
        al::updateKitListPostOnNerveEnd(this);
    }
}

void StageScene::exeDemoTitleLogo() {
    if (al::isFirstStep(this)) {
        rs::tryCloseHackTutorial(this);
        mSceneLayout->end();
        rs::endPlayTalkMsgTimeBalloonLayout(this);
        if (_408)
            _408->disableLayout();
    }
    if (al::updateNerveState(this))
        kill();
}

void StageScene::exeDemoShineMainGet() {
    if (al::isFirstStep(this) && !mSceneLayout->isEnd())
        mSceneLayout->end();
    if (al::updateNerveState(this)) {
        rs::endDemoShineGet(this);
        rs::endDemoCommonProc(this, mProjectItemDirector);
        updateKitListPostAndChangeNextNerve(&NrvStageScene.StartStageBgm);
    } else {
        al::updateKitListPostOnNerveEnd(this);
    }
}

void StageScene::exeDemoRiseMapParts() {
    if (al::isFirstStep(this))
        al::startCheckViewCtrlByLookAtPos(this);
    tryAppearDemoLayout();
    al::updateKitListPrev(this);
    rs::updateKitListDemoNormalWithPauseEffect(this);
    rs::updateKitListPostWithPauseNormalEffectAndPlayerEffect(this);
    if (rs::isActiveDemo(this)) {
        al::updateKitListPostOnNerveEnd(this);
        return;
    }
    al::startCheckViewCtrlByPlayerPos(this);
    endDemoAndChangeNerve();
}

void StageScene::exeDemoRisePyramid() {
    if (al::isFirstStep(this)) {
        al::startCheckViewCtrlByLookAtPos(this);
        mPyramid->tryStartRiseDemo();
    }
    tryAppearDemoLayout();
    al::updateKitListPrev(this);
    rs::updateKitListDemoNormalWithPauseEffect(this);
    al::updateKitListPostDemoWithPauseNormalEffect(this);
    if (mPyramid->isDemoPlaying()) {
        al::updateKitListPostOnNerveEnd(this);
        return;
    }
    al::startCheckViewCtrlByPlayerPos(this);
    mStateScenarioCamera->tryStart();
    updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoScenarioCamera);
}

void StageScene::exeDemoShineChipComplete() {
    if (al::isFirstStep(this))
        mSceneLayout->startShineChipCompleteAnim();
    updateStageInfoLayout();
    al::updateKitListPrev(this);
    rs::updateKitListDemoNormalWithPauseEffect(this);
    rs::updateKitListPostWithPauseNormalEffectAndPlayerEffect(this);
    if (mSceneLayout->isEndShineChipCompleteAnim())
        rs::tryStartAppearShineChipShine(this);
    if (rs::isAppearedShineChipShine(this))
        rs::requestEndDemoShineAppear(this);
    if (rs::isActiveDemo(this)) {
        al::updateKitListPostOnNerveEnd(this);
        return;
    }
    mSceneLayout->endShineChipCompleteAnim();
    endDemoAndChangeNerve();
}

void StageScene::exeDemoPlayerDown() {
    if (al::isFirstStep(this)) {
        rs::requestStartDemoWithPlayer(this);
        rs::startActionDemoPlayer(mCheckpointWarpCapActor, "DemoOpeningDown");
        al::invalidateEndEntranceCameraWithName(this, "[シーン]プレイヤー倒れデモ");
    }
    bool isEndDemo = false;
    if (rs::isTriggerUiCancel(this) || rs::isTriggerUiDecide(this)) {
        rs::requestEndDemoWithPlayer(this);
        isEndDemo = true;
    }
    al::updateKitListPrev(this);
    rs::updateKitListDemoPlayerWithPauseEffect(this);
    al::updateKitListPostDemoWithPauseNormalEffect(this);
    if (isEndDemo) {
        al::validateEndEntranceCamera(this);
        updateKitListPostAndChangeNextNerve(&NrvStageScene.StartStageBgm);
    } else {
        al::updateKitListPostOnNerveEnd(this);
    }
}

void StageScene::exeDemoScenarioCamera() {
    if (al::isFirstStep(this)) {
        if (al::isDisableBgmLineChange(this))
            al::enableBgmLineChange(this);
        rs::changeDemoCommonProc(this, mProjectItemDirector);
    }
    al::updateNerveState(this);
    if (rs::isActiveDemo(this)) {
        al::updateKitListPostOnNerveEnd(this);
    } else if (CapManHeroDemoUtil::isExistTalkDemoStageStart(this)) {
        CapManHeroDemoUtil::startTalkDemoStageStart(this);
        updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoStageStartCapManHeroTalk);
    } else {
        rs::endDemoCommonProc(this, mProjectItemDirector);
        updateKitListPostAndChangeNextNerve(&NrvStageScene.StartStageBgm);
    }
}

void StageScene::exeDemoHackStart() {
    bool isRadicon = !GameDataFunction::isMainStage(this) && rs::isPlayerHackRadiconNpc(mDemoShine);

    if (al::isFirstStep(this)) {
        if (isRadicon) {
            mSceneLayout->end();
            rs::setKidsModeLayoutDisable(this);
        }
        rs::killCoinCollectHintEffect(this);
    }

    al::updateKitListPrev(this);
    rs::updateKitListDemoPlayerWithPauseEffect(this);
    if (isRadicon)
        mSceneLayout->update();
    al::updateKitListPostDemoWithPauseNormalEffect(this);

    if (!rs::isActiveDemo(this)) {
        endDemoAndChangeNerve();
        if (isRadicon)
            al::setNerve(this, &NrvStageScene.Radicon);
        return;
    }

    al::updateKitListPostOnNerveEnd(this);
}

void StageScene::updateStageSceneLayoutForKoopaHack() {
    if (KoopaHackFunction::isStatusDemoForSceneKoopaHack(mDemoShine)) {
        if (!mIsKoopaHackDemo) {
            mIsKoopaHackDemo = true;
            mSceneLayout->end();
        }
    } else if (mIsKoopaHackDemo) {
        mSceneLayout->start();
        mIsKoopaHackDemo = false;
    }
}

bool StageScene::tryChangeSnapShot() {
    if (KoopaHackFunction::isStatusDemoForSceneKoopaHack(mDemoShine))
        return false;
    if (!isOpenEndWorldMapWipe())
        return false;
    if (rs::isModeE3Rom())
        return false;
    if (rs::isModeE3LiveRom())
        return false;
    if (!mStateSnapShot->tryStart())
        return false;
    if (*(u8*)((uintptr_t)mWipeHolder + 0x18))
        return false;
    if (!rs::isDemoFadeWipeStateEnableOpenMenuOrSnapShotMode(this))
        return false;
    al::setNerve(this, &NrvStageScene.SnapShot);
    return true;
}

bool StageScene::tryStartWarp() {
    if (!mStateWarp->tryStartWarp())
        return false;
    al::setNerve(this, &NrvStageScene.Warp);
    return true;
}

bool StageScene::tryChangePause() {
    bool isTrigger = rs::isTriggerUiPause(this);
    if (!isOpenEndWorldMapWipe())
        return false;
    if (rs::isPlayerPlayingSwitchOnAnim(mCheckpointWarpCapActor))
        return false;
    if (KoopaHackFunction::isStatusDemoForSceneKoopaHack(mDemoShine))
        return false;
    if (*(u8*)((uintptr_t)mWipeHolder + 0x18))
        return false;
    if (!mWipeHolder->isOpenEnd())
        return false;
    if (!(isTrigger & rs::isDemoFadeWipeStateEnableOpenMenuOrSnapShotMode(this)))
        return false;
    if (_408)
        *(u8*)((uintptr_t)_408 + 0x30) = 0;
    al::setNerve(this, &NrvStageScene.Pause);
    return true;
}

bool StageScene::tryChangeList() {
    if (!isOpenEndWorldMapWipe())
        return false;
    if (rs::isPlayerPlayingSwitchOnAnim(mCheckpointWarpCapActor))
        return false;
    if (KoopaHackFunction::isStatusDemoForSceneKoopaHack(mDemoShine))
        return false;
    if (*(u8*)((uintptr_t)mWipeHolder + 0x18))
        return false;
    if (!mWipeHolder->isOpenEnd())
        return false;
    if (!rs::isDemoFadeWipeStateEnableOpenMenuOrSnapShotMode(this))
        return false;
    if (!rs::isTriggerMapOpen(this))
        return false;
    if (rs::isHelpAmiiboMode(this))
        return false;
    al::setNerve(this, &NrvStageScene.CollectionList);
    return true;
}

void StageScene::updateStageInfoLayout() {
    mSceneLayout->updateCounterParts();
}

void StageScene::endPlay() {
    rs::resetHelpAmiiboState(this);
    if (!rs::isActiveDemoHackStart(this) || rs::isActiveDemoHackStartFirst(this)) {
        al::LiveActor* player = al::getPlayerActor(al::getScenePlayerHolder(this), 0);
        if (rs::isPlayerDamageStopDemo(player))
            return;
        rs::offRouteGuideSystem(this);
    }
}

void StageScene::exeAmiibo() {
    if (al::isFirstStep(this)) {
        if (!AmiiboFunction::isTriggerTouchAmiiboMario(this) &&
            !AmiiboFunction::isTriggerTouchAmiiboPeach(this))
            AmiiboFunction::isTriggerTouchAmiiboKoopa(this);
        al::startBgmSituation(this, "TouchAmiibo", 0);
    }
    mHelpAmiiboDirector->execute();
    al::updateKitListPrev(this);
    al::updateKitListPost(this);
    al::updateKitListPostOnNerveEnd(this);
    if (al::isGreaterEqualStep(this, 60)) {
        al::setNerve(this, &NrvStageScene.Play);
        al::endBgmSituation(this, "TouchAmiibo", 0);
    }
}

void StageScene::exeMiss() {
    if (al::isFirstStep(this)) {
        al::pausePadRumble(this);
        alPadRumbleFunction::startPadRumbleNo3D(getLiveActorKit()->getPadRumbleDirector(),
                                                "ドカ（中）", -1);
        alSeFunction::startSituation(getAudioDirector(), "ステージ終了切り替え", 90);
        const char* stageName = GameDataFunction::getCurrentStageName(mGameDataHolder);
        sead::Vector3f pos = al::getPlayerPos(al::getScenePlayerHolder(this), 0);
        GameDataHolderAccessor accessor(this);
        u64 playTimeTotal = GameDataFunction::getPlayTimeTotal(accessor);
        GameDataHolderAccessor accessor2(this);
        u64 saveDataId = GameDataFunction::getSaveDataIdForPrepo(accessor2);
        GameDataHolderAccessor accessor3(this);
        u64 playTimeAcrossFile = GameDataFunction::getPlayTimeAcrossFile(accessor3);
        rs::trySavePrepoMissEvent(stageName, pos, playTimeTotal, saveDataId, playTimeAcrossFile);
    }
    if (al::updateNerveState(this)) {
        if (mStateMiss->isResetCollectBgm())
            mCollectBgmPlayer->reset();
        if (isAlive()) {
            GameDataHolderWriter writer(this);
            GameDataFunction::missAndRestartStage(writer);
            GameDataFunction::tryWearCostumeRandomIfEnable(this);
            GameDataFunction::tryWearCapRandomIfEnable(this);
        }
        kill();
    }
    al::updateKitListPostOnNerveEnd(this);
}

void StageScene::exePause() {
    if (al::isFirstStep(this)) {
        mStagePauseMenu->startNormal();
        mSceneLayout->killShineCount();
    }
    getLiveActorKit()->getEffectSystem()->set_69(true);
    al::updateNerveState(this);
    if (mStagePauseMenu->checkNeedKillByHostAndEnd()) {
        kill();
    } else if (!isPause()) {
        getLiveActorKit()->getEffectSystem()->set_69(false);
        mSceneLayout->appearShineCountWait();
        updateKitListPostAndChangeNextNerve(&NrvStageScene.Play);
    } else {
        al::updateKitListPostOnNerveEnd(this);
    }
}

void StageScene::exeDemoShineGet() {
    if (al::isFirstStep(this)) {
        if (mStateShop->isEndBuyShine10())
            mStateGetShine->setShopShine10();
        if (tryUpdateAllForFirst())
            return;
    }
    if (al::updateNerveState(this)) {
        rs::tryShowCapMsgIntroducePowerStar(this, mStateGetShine);
        if (mStateShop->isEndBuyShine() || mStateShop->isEndBuyShine10()) {
            mStateShop->requestOpen();
            al::startAudioEffectWithAreaCheck(this);
            if (!al::isPlayingBgm(this, "Shop"))
                al::startAndStopBgmInCurPosition(this, 1);
            updateKitListPostAndChangeNextNerve(&NrvStageScene.Shop);
            return;
        }
        if (al::isNerve(this, &NrvStageScene.DemoShineGet)) {
            if (rs::isActiveDemo(this) && rs::isActiveDemoNormal(this)) {
                updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoNormal);
            } else {
                rs::endDemoCommonProc(this, mProjectItemDirector);
                updateKitListPostAndChangeNextNerve(&NrvStageScene.StartStageBgm);
            }
        }
    } else {
        if (mStateGetShine->isDemoEnd()) {
            if (tryChangeDemo()) {
                al::updateKitListPostOnNerveEnd(this);
                if (al::isNerve(this, &NrvStageScene.DemoShineGet)) {
                    mDemoSyncedEventKeeper->endDemo(0, 0);
                    mActiveDemoName = nullptr;
                }
                return;
            }
            if (tryChangeList() || tryChangePause()) {
                rs::endDemoCommonProc(this, mProjectItemDirector);
                rs::tryShowCapMsgIntroducePowerStar(this, mStateGetShine);
                al::updateKitListPostOnNerveEnd(this);
                return;
            }
        }
        al::updateKitListPostOnNerveEnd(this);
        if (mStateMiss->checkMiss())
            al::setNerve(this, &NrvStageScene.Miss);
    }
}

void StageScene::exeDemoStageStart() {
    if (al::isFirstStep(this)) {
        if (al::isNerve(this, &NrvStageScene.DemoStageStartElectricDemo)) {
            mCurrentNerve = &NrvStageScene.DemoStageStartElectricDemo;
            rs::setDemoInfoDemoName(this, "電線移動デモ");
        } else {
            mCurrentNerve = &NrvStageScene.DemoStageStart;
        }
        al::deactivateAudioEventController(this);
        if (al::isNerve(this, &NrvStageScene.DemoStageStartOpening))
            alSeFunction::tryStartDefaultAtmosphereSe(mSePlayArea);
        rs::killTutorial(this);
    }
    al::updateKitListPrev(this);
    if (al::isNerve(this, &NrvStageScene.DemoStageStartOpening))
        rs::updateKitListDemoCapWorldOpeninglWithPauseEffect(this);
    else if (al::isNerve(this, &NrvStageScene.DemoStageStartElectricDemo))
        rs::updateKitListDemoNormalWithPauseEffect(this);
    al::updateKitListPostDemoWithPauseNormalEffect(this);
    if (mStateSkipDemo->tryStartSkipDemo()) {
        updateKitListPostAndChangeNextNerve(&NrvStageScene.SkipDemo);
    } else {
        mStateSkipDemo->tryAppearSkipDemoLayout();
        if (mOpeningStageStartDemo->isDemoLastStep() &&
            !al::isNerve(this, &NrvStageScene.DemoStageStartOpening))
            al::requestCaptureScreenCover(mCheckpointWarpCapActor, 4);
        if (!mOpeningStageStartDemo->isEndDemo()) {
            al::updateKitListPostOnNerveEnd(this);
            return;
        }
        mStateSkipDemo->tryEndForNoSkip();
        if (al::isNerve(this, &NrvStageScene.DemoStageStartElectricDemo) &&
            mStateScenarioCamera->tryStart()) {
            updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoScenarioCamera);
        } else {
            al::activateAudioEventController(this);
            updateKitListPostAndChangeNextNerve(&NrvStageScene.StartStageBgm);
        }
    }
}

void StageScene::exeDemoStageStartCapManHeroTalk() {
    if (al::isFirstStep(this)) {
        rs::changeDemoCommonProc(this, mProjectItemDirector);
        al::updateKit(this);
        al::updateGraphicsPrev(this);
        return;
    }
    if (al::isStep(this, 2)) {
        al::updateKit(this);
        al::updateGraphicsPrev(this);
        return;
    }
    al::updateKitListPrev(this);
    rs::updateKitListDemoPlayerWithPauseEffectAndCalcAtEndClip(this);
    al::updateKitListPostDemoWithPauseNormalEffect(this);
    if (CapManHeroDemoUtil::isEndDemo(this))
        endDemoAndChangeNerve();
    else
        al::updateKitListPostOnNerveEnd(this);
}

void StageScene::tryAppearDemoLayout() {
    if (al::isFirstStep(this) && !al::isNerve(this, &NrvStageScene.DemoWithPlayerUseCoinNoFirst)) {
        if (al::isNerve(this, &NrvStageScene.DemoWithPlayerUseCoin)) {
            bool found = false;
            for (s32 i = 0; i < mShoppingWatcherGroup->getActorCount(); i++) {
                if (rs::getRequestActor(this) == mShoppingWatcherGroup->getActor(i)) {
                    found = true;
                    break;
                }
            }
            mSceneLayout->endWithoutCoin(found);
        } else if (!mSceneLayout->isEnd()) {
            mSceneLayout->end();
        }
    }
}

void StageScene::exeDemoNormalBazookaElectric() {
    if (mSceneLayout->isWait())
        mSceneLayout->end();
    tryAppearDemoLayout();
    al::updateKitListPrev(this);
    rs::updateKitListDemoNormalWithPauseEffect(this);
    al::updateKitList(this, "波");
    al::updateKitListPostDemoWithPauseNormalEffect(this);
    if (rs::tryChangeNextStage(mGameDataHolder, this))
        kill();
    else
        al::updateKitListPostOnNerveEnd(this);
}

void StageScene::exeDemoNormal() {
    bool isCinema =
        rs::isActiveDemoNormalWithCinemaFrame(this) || rs::isActiveDemoBirdFindMeat(this);
    if (al::isFirstStep(this)) {
        mCurrentNerve = &NrvStageScene.DemoNormal;
        rs::tryAppearCinemaCaption(this, mCinemaCaption);
    }
    if (mSceneLayout->isWait())
        mSceneLayout->end();
    tryAppearDemoLayout();
    al::updateKitListPrev(this);
    rs::updateKitListDemoNormalWithPauseEffect(this);
    rs::updateKitListPostWithPauseNormalEffectAndPlayerEffect(this);
    if (mStateMiniGameRanking && mStateMiniGameRanking->tryOpen()) {
        updateKitListPostAndChangeNextNerve(&NrvStageScene.MiniGameRanking);
        return;
    }
    if (isCinema) {
        if (mStateSkipDemo->tryStartSkipDemo()) {
            updateKitListPostAndChangeNextNerve(&NrvStageScene.SkipDemo);
            return;
        }
        mStateSkipDemo->tryAppearSkipDemoLayout();
    }
    if (rs::isActiveDemoShineGet(this)) {
        rs::killTutorial(this);
        al::stopPadRumble(this);
        updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoShineGet);
        return;
    }
    if (rs::isActiveDemo(this)) {
        if (rs::isActiveDemoGetLifeMaxUpItem(this)) {
            updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoGetLifeMaxUpItem);
            return;
        }
        if (rs::tryChangeNextStage(mGameDataHolder, this))
            kill();
        else
            al::updateKitListPostOnNerveEnd(this);
    } else {
        if (isCinema)
            mStateSkipDemo->tryEndForNoSkip();
        endDemoAndChangeNerve();
    }
}

void StageScene::exeDemoWithPlayer() {
    if (al::isFirstStep(this)) {
        mCurrentNerve = &NrvStageScene.DemoWithPlayer;
        rs::tryAppearCinemaCaption(this, mCinemaCaption);
    }
    tryAppearDemoLayout();
    al::updateKitListPrev(this);
    rs::updateKitListDemoPlayerWithPauseEffect(this);
    al::updateKitList(this, "ステージスイッチディレクター");
    al::updateKitListPostDemoWithPauseNormalEffect(this);
    if (mStateSkipDemo->tryStartSkipDemo()) {
        updateKitListPostAndChangeNextNerve(&NrvStageScene.SkipDemo);
        return;
    }
    mStateSkipDemo->tryAppearSkipDemoLayout();
    if (rs::tryChangeNextStage(mGameDataHolder, this) || isDefeatKoopaLv1()) {
        kill();
    } else if (rs::isActiveDemo(this)) {
        if (mStateCloset && mStateCloset->isOpenEnd()) {
            updateKitListPostAndChangeNextNerve(&NrvStageScene.Closet);
            return;
        }
        al::updateKitListPostOnNerveEnd(this);
    } else {
        mStateSkipDemo->tryEndForNoSkip();
        mCinemaCaption->tryEnd();
        endDemoAndChangeNerve();
    }
}

void StageScene::exeDemoWithPlayerUseCoin() {
    s32 coinNum = GameDataFunction::getCoinNum(mGameDataHolder);
    tryAppearDemoLayout();
    al::updateKitListPrev(this);
    rs::updateKitListDemoPlayerWithPauseEffect(this);
    al::updateKitListPostDemoWithPauseNormalEffect(this);
    updateStageInfoLayout();
    if (!rs::isActiveDemo(this)) {
        endDemoAndChangeNerve();
        return;
    }
    for (s32 i = 0; i < mShoppingWatcherGroup->getActorCount(); i++) {
        if (mShoppingWatcherGroup->getDeriveActor(i)->isShop()) {
            mStateShop->settingShopId(i);
            updateKitListPostAndChangeNextNerve(&NrvStageScene.Shop);
            return;
        }
    }
    s32 coinNumAfter = GameDataFunction::getCoinNum(mGameDataHolder);
    al::updateKitListPostOnNerveEnd(this);
    if (coinNumAfter != coinNum)
        al::setNerve(this, &NrvStageScene.DemoCountCoin);
}

void StageScene::exeDemoCountCoin() {
    if (al::isFirstStep(this)) {
        StageSceneLayout* layout = mSceneLayout;
        s32 coinNum = GameDataFunction::getCoinNum(mGameDataHolder);
        layout->startCoinCountAnim(coinNum);
    }
    al::updateKitListPrev(this);
    al::updateKitList(this, "サウンド制御");
    al::updateKitList(this, "シャドウマスク");
    al::updateKitList(this, "グラフィックス要求者");
    al::updateKitList(this, "２Ｄ");
    al::updateKitList(this, "２Ｄ（ポーズ無視）");
    al::updateKitListPost(this);
    bool isEnd = mSceneLayout->isEndCoinCountAnim();
    al::updateKitListPostOnNerveEnd(this);
    if (isEnd)
        al::setNerve(this, &NrvStageScene.DemoWithPlayerUseCoinNoFirst);
}

void StageScene::exeDemoHackStartFirst() {
    if (al::isFirstStep(this)) {
        rs::noticeDemoStartToDemoHackFirstDirector(this);
        mCurrentNerve = &NrvStageScene.DemoHackStartFirst;
    }
    tryAppearDemoLayout();
    if (_496) {
        _496 = true;
    } else {
        bool started = mSceneLayout->tryStartDemoGetLifeMaxUpItem(1);
        _496 = started;
        if (!started)
            goto skipEnd;
    }
    mStateSkipDemo->tryEndForNoSkip();
skipEnd:
    al::updateKitListPrev(this);
    rs::updateKitListDemoPlayerWithPauseEffect(this);
    al::updateKitListPostDemoWithPauseNormalEffect(this);
    if (!_496) {
        if (mStateSkipDemo->tryStartSkipDemo()) {
            updateKitListPostAndChangeNextNerve(&NrvStageScene.SkipDemo);
            return;
        }
        mStateSkipDemo->tryAppearSkipDemoLayout();
    }
    if (rs::isActiveDemo(this)) {
        al::updateKitListPostOnNerveEnd(this);
    } else {
        mStateSkipDemo->tryEndForNoSkip();
        endDemoAndChangeNerve();
    }
}

bool StageScene::tryUpdateAllForFirst() {
    if (!mIsUpdateKitAndGraphics)
        return false;
    al::updateKit(this);
    al::updateGraphicsPrev(this);
    mIsUpdateKitAndGraphics = false;
    return true;
}

void StageScene::exeShop() {
    al::updateKitListPrev(this);
    al::updateKitList(this, "カメラ");
    al::updateKitList(this, "NPCイベントディレクター");
    al::updateKitList(this, "サウンド制御");
    al::updateKitList(this, "プレイヤー[Movement]");
    al::updateDemoActorForPauseEffect(this);
    if (!al::updateNerveState(this)) {
        al::updateKitList(this, "２Ｄ");
        al::updateKitListPostDemoWithPauseNormalEffect(this);
        al::updateKitListPostOnNerveEnd(this);
        return;
    }
    al::updateKitListPostDemoWithPauseNormalEffect(this);
    if (mStateWorldMap) {
        al::LiveActor* homeSticker = mStateWorldMap->getHomeSticker();
        if (homeSticker)
            rs::setupHomeSticker(homeSticker);
    }
    if (mStateShop->isWear()) {
        kill();
        return;
    }
    if (mStateShop->isEndBuyShine()) {
        rs::setShopStatusBuyShine(mDemoShine);
        kill();
        return;
    }
    if (mStateShop->isEndBuyShine10()) {
        rs::setShopStatusBuyShineMany(mDemoShine);
        kill();
        return;
    }
    if (mStateShop->isEndBuyLifeMaxUpItem()) {
        rs::setShopStatusBuyMaxLifeUpItem(mDemoShine);
        kill();
        return;
    }
    updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoWithPlayerUseCoin);
}

void StageScene::exeCloset() {
    if (al::isFirstStep(this))
        mSceneLayout->startCloset();
    al::updateKitListPrev(this);
    al::updateKitList(this, "プレイヤー");
    al::updateKitList(this, "プレイヤー装飾");
    al::updateKitList(this, "プレイヤー装飾２");
    if (al::updateNerveState(this)) {
        if (mStateCloset->isChangedStage()) {
            GameDataFunction::changeNextStageWithCloset(mGameDataHolder);
            kill();
        } else {
            mSceneLayout->endCloset();
            mStateCloset->tryCloseDoor();
            updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoWithPlayer);
        }
    } else {
        al::updateKitList(this, "サウンド制御");
        al::updateKitList(this, "２Ｄ");
        al::updateKitListPost(this);
        mStateCloset->updateAfterMovement();
        al::updateKitListPostOnNerveEnd(this);
    }
}

void StageScene::exeCollectBgm() {
    al::updateKitListPrev(this);
    al::updateDemoActorForPauseEffect(this);
    al::updateKitList(this, "サウンド制御");
    al::updateKitList(this, "２Ｄ");
    al::updateKitListPostDemoWithPauseNormalEffect(this);
    if (!al::updateNerveState(this)) {
        al::updateKitListPostOnNerveEnd(this);
        return;
    }
    bool isPlaying = al::isPlayingBgm(this);
    al::updateKitListPostOnNerveEnd(this);
    if (isPlaying)
        al::setNerve(this, &NrvStageScene.Play);
    else
        al::setNerve(this, &NrvStageScene.StartStageBgm);
}

void StageScene::exeMiniGameRanking() {
    al::updateKitListPrev(this);
    al::updateDemoActorForPauseEffect(this);
    al::updateKitList(this, "サウンド制御");
    al::updateKitList(this, "２Ｄ");
    al::updateKitList(this, "２Ｄ（ポーズ無視）");
    al::updateKitListPostDemoWithPauseNormalEffect(this);
    al::updateKitListPostOnNerveEnd(this);
    al::updateNerveStateAndNextNerve(this, &NrvStageScene.DemoWithPlayer);
}

void StageScene::exeTimeBalloon() {
    if (al::isFirstStep(this)) {
        rs::endForceTalkMsgTimeBalloonLayout(this);
        if (_408)
            _408->disableLayout();
    }
    mTimeBalloonDirector->execute();
    if (al::updateNerveState(this))
        kill();
    if (mStateTimeBalloon->isReturnNervePlay()) {
        mStateTimeBalloon->kill();
        al::updateKitListPostOnNerveEnd(this);
        al::setNerve(this, &NrvStageScene.Play);
    } else {
        al::updateKitListPostOnNerveEnd(this);
    }
}

void StageScene::exeWaitStartWarpForSession() {
    updatePlay();
    if (rs::tryChangeNextStage(mGameDataHolder, this)) {
        kill();
    } else {
        if (tryStartWarp())
            ;
        al::updateKitListPostOnNerveEnd(this);
    }
}

void StageScene::exeDemoShineGrandGet() {
    if (al::isFirstStep(this)) {
        al::disableBgmLineChange(this);
        al::deactivateAudioEventController(this);
        al::banAudioEventActivation(this);
        alSeFunction::startSituation(getAudioDirector(), "グランドシャイン取得デモ", -1);
        if (mSceneLayout->isWait())
            mSceneLayout->end();
    }
    bool updated = al::updateNerveState(this);
    al::updateKitList(this, "ステージスイッチディレクター");
    if (updated) {
        al::clearBgmDataForStepOverScene(this);
        rs::tryChangeNextStage(mGameDataHolder, this);
        al::allowAudioEventActivation(this);
        al::activateAudioEventController(this);
        al::enableBgmLineChange(this);
        kill();
    } else {
        al::updateKitListPostOnNerveEnd(this);
    }
}

void StageScene::exeDemoDamageStopPlayer() {
    al::updateKitListPrev(this);
    al::updateKitList(this, "プレイヤー前処理");
    al::updateKitList(this, "プレイヤー[Movement]");
    al::updateKitList(this, "プレイヤー後処理");
    al::updateKitList(this, "帽子装着位置更新");
    al::updateKitList(this, "サウンド制御");
    al::updateKitList(this, "シャドウマスク");
    al::updateKitList(this, "グラフィックス要求者");
    al::updateEffectPlayer(this);
    updateStageInfoLayout();
    al::updateKitList(this, "２Ｄ");
    al::updateKitList(this, "２Ｄ（ポーズ無視）");
    al::updateKitListPostOnNerveEnd(this);
    al::LiveActor* player = al::getPlayerActor(al::getScenePlayerHolder(this), 0);
    if (!rs::isPlayerDamageStopDemo(player))
        al::setNerve(this, &NrvStageScene.Play);
}

void StageScene::exeDemoSceneStartPlayerWalk() {
    updatePlay();
    rs::updateDemoSystemOnly(this);
    if (rs::isActiveDemo(this)) {
        al::updateKitListPostOnNerveEnd(this);
    } else {
        rs::endDemoCommonProc(this, mProjectItemDirector);
        updateKitListPostAndChangeNextNerve(&NrvStageScene.StartStageBgm);
    }
}

void StageScene::exeSkipDemo() {
    if (al::updateNerveState(this)) {
        if (mStateSkipDemo->isCancelSkip()) {
            updateKitListPostAndChangeNextNerve(mCurrentNerve);
            mCurrentNerve = nullptr;
            mAudioSystemPauseController->resume(1);
        } else {
            if (isDefeatKoopaLv1())
                kill();
            if (mCurrentNerve == &NrvStageScene.DemoStageStartElectricDemo) {
                if (mStateScenarioCamera->tryStart()) {
                    updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoScenarioCamera);
                    mCurrentNerve = nullptr;
                }
            } else {
                endDemoAndChangeNerve();
                if (mStateSkipDemo->isDemoCancelStageScene()) {
                    if (!isAlive()) {
                        al::stopAllBgm(this, 0);
                        al::stopAllSe(this, 0);
                    }
                }
                mCurrentNerve = nullptr;
                mCinemaCaption->tryEnd();
            }
        }
    } else {
        al::updateKitListPostOnNerveEnd(this);
    }
}

void StageScene::exeDemoTalk() {
    if (al::isFirstStep(this)) {
        if (rs::isActiveDemoKeepHackTalk(this))
            mProjectItemDirector->tryAddDemoCountUpCoin();
        if (tryUpdateAllForFirst())
            return;
    }
    if (al::updateNerveState(this)) {
        al::endPausePadRumble(this);
        if (al::isExistSceneObj(this, 57)) {
            al::ISceneObj* sceneObj = al::getSceneObj(this, 57);
            SessionMusicianManager* manager = static_cast<SessionMusicianManager*>(sceneObj);
            if (manager->isJoinedMusician()) {
                mWipeSimple->startCloseEnd();
                if (tryStartWarp()) {
                    al::updateKitListPostOnNerveEnd(this);
                    return;
                }
                alSeFunction::startSituationWithAutoEnd(getAudioDirector(),
                                                        "シーン開始フェードイン", 0, 5, 7);
                updateKitListPostAndChangeNextNerve(&NrvStageScene.WaitStartWarpForSession);
                return;
            }
        }
        endDemoAndChangeNerve();
        if (mStateSkipDemo->isDemoCancelStageScene()) {
            if (!isAlive()) {
                al::stopAllBgm(this, 0);
                al::stopAllSe(this, 0);
            }
        }
        return;
    }
    if (rs::isExistRequestShowHtmlViewer(this)) {
        rs::clearRequestHtmlViewerRequester(this);
        HelpFunction::callHelp(mHtmlViewer, mGameDataHolder, false, true, false, nullptr);
    }
    al::updateKitListPostOnNerveEnd(this);
}

void StageScene::exeDemoReturnToHome() {
    if (al::isFirstStep(this) && tryUpdateAllForFirst())
        return;
    tryAppearDemoLayout();
    al::updateKitListPrev(this);
    rs::updateKitListDemoPlayerWithPauseEffect(this);
    al::updateKitListPostDemoWithPauseNormalEffect(this);
    if (rs::isActiveDemo(this)) {
        al::updateKitListPostOnNerveEnd(this);
        return;
    }
    if (mPyramid && mScenarioNo == 2) {
        updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoRisePyramid);
    } else if (mStateScenarioCamera->tryStart()) {
        updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoScenarioCamera);
    } else if (CapManHeroDemoUtil::isExistTalkDemoStageStart(this)) {
        CapManHeroDemoUtil::startTalkDemoStageStart(this);
        updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoStageStartCapManHeroTalk);
    } else {
        al::activateAudioEventController(this);
        updateKitListPostAndChangeNextNerve(&NrvStageScene.StartStageBgm);
    }
}

void StageScene::exeDemoAppearFromHome() {
    if (al::isLessStep(this, 2)) {
        al::updateKit(this);
        al::updateGraphicsPrev(this);
        return;
    }
    tryAppearDemoLayout();
    al::updateKitListPrev(this);
    rs::updateKitListDemoPlayerWithPauseEffect(this);
    al::updateKitListPostDemoWithPauseNormalEffect(this);
    bool existCamera = mStateScenarioCamera->isExistEnableCamera();
    bool isStep220 = al::isStep(this, 220);
    if (!existCamera && isStep220)
        mWipeHolder->startClose("FadeBlack", -1);
    if (!rs::isActiveDemo(this)) {
        if (!existCamera)
            mWipeHolder->startOpen(-1);
        if (al::isExistSceneObj(this, 20)) {
            al::ISceneObj* sceneObj = al::getSceneObj(this, 20);
            GiantWanderBoss* boss = static_cast<GiantWanderBoss*>(sceneObj);
            if (boss->isOnSwitchAll()) {
                rs::requestStartDemoWithPlayer(this);
                rs::addDemoActor(boss, false);
                boss->startDemoWorldEnter();
                rs::changeDemoCommonProc(this, mProjectItemDirector);
                updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoWithPlayer);
                return;
            }
        }
        if (CapManHeroDemoUtil::isExistTalkDemoMoonRockFind(this)) {
            CapManHeroDemoUtil::startTalkDemoMoonRockFind(this);
            updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoCapManHeroTalkMoonRock);
            al::activateAudioEventController(this);
            return;
        }
        if (mStateScenarioCamera->tryStart()) {
            updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoScenarioCamera);
            return;
        }
        endDemoAndChangeNerve();
    }
    if (!al::isLessStep(this, 2))
        al::updateKitListPostOnNerveEnd(this);
}

void StageScene::exeWarp() {
    if (al::updateNerveState(this)) {
        bool changed = tryChangeDemo();
        al::updateKitListPostOnNerveEnd(this);
        if (!changed)
            al::setNerve(this, &NrvStageScene.Play);
        return;
    }
    al::updateKitListPostOnNerveEnd(this);
}

void StageScene::exeArriveAtCheckpoint() {
    if (al::isFirstStep(this)) {
        rs::setDemoInfoDemoName(this, "チェックポイントワープ到着デモ");
        al::activateAudioEventController(this);
        positionAudioControl(this);
    }
    bool ended = al::updateNerveState(this);
    al::updateKitListPostOnNerveEnd(this);
    if (ended)
        al::setNerve(this, &NrvStageScene.StartStageBgm);
}

void StageScene::exeDemoWorldIntroCamera() {
    tryAppearDemoLayout();
    if (!al::updateNerveState(this)) {
        al::updateKitListPostOnNerveEnd(this);
        return;
    }
    if (al::isNerve(this, &NrvStageScene.DemoWorldIntroCameraBeforeAppearElectricDemo)) {
        mOpeningStageStartDemo->startDemo();
        al::updateKitListPostOnNerveEnd(this);
        al::setNerve(this, &NrvStageScene.DemoStageStartElectricDemo);
        return;
    }
    GameDataHolderAccessor accessor(this);
    if (GameDataFunction::isCrashHome(accessor)) {
        al::requestCancelCameraInterpole(this, 0);
        if (CapManHeroDemoUtil::isExistTalkDemoStageStart(this)) {
            CapManHeroDemoUtil::startTalkDemoStageStart(this);
            al::updateKitListPostOnNerveEnd(this);
            al::setNerve(this, &NrvStageScene.DemoStageStartCapManHeroTalk);
        } else {
            al::setNerve(this, &NrvStageScene.StartStageBgm);
        }
    } else if (rs::isExistDemoAppearFromHomeInList(this)) {
        rs::requestStartDemoAppearFromHome(this);
        updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoAppearFromHome);
    } else if (CapManHeroDemoUtil::isExistTalkDemoStageStart(this)) {
        CapManHeroDemoUtil::startTalkDemoStageStart(this);
        updateKitListPostAndChangeNextNerve(&NrvStageScene.DemoStageStartCapManHeroTalk);
    } else {
        updateKitListPostAndChangeNextNerve(&NrvStageScene.StartStageBgm);
    }
}

void StageScene::exeAppearFromWorldWarpHole() {
    if (al::isFirstStep(this)) {
        mWorldStartCountryLayout->appear();
        mWorldStartCountryLayout->setLifetime(120);
        mWorldStartRegionLayout->appear();
        mWorldStartRegionLayout->setLifetime(120);

        al::setPaneString(mWorldStartCountryLayout, "TxtWorld",
                          GameDataFunction::tryGetWorldNameCurrent(mWorldStartCountryLayout), 0);
        al::setPaneString(mWorldStartCountryLayout, "TxtWorldSh",
                          GameDataFunction::tryGetWorldNameCurrent(mWorldStartCountryLayout), 0);
        al::setPaneString(mWorldStartRegionLayout, "TxtRegion",
                          GameDataFunction::tryGetRegionNameCurrent(mWorldStartCountryLayout), 0);
        al::setPaneString(mWorldStartRegionLayout, "TxtRegionSh",
                          GameDataFunction::tryGetRegionNameCurrent(mWorldStartCountryLayout), 0);

        if (rs::getActiveQuestNum(this) >= 1) {
            mGameDataHolder->playScenarioStartCamera(rs::getActiveQuestNo(this));
            if (GameDataFunction::isCityWorldScenario2(this))
                mGameDataHolder->playScenarioStartCamera(2);
        }
    }

    updatePlay();
    rs::updateDemoSystemOnly(this);
    if (tryChangeDemo()) {
        mWorldStartCountryLayout->kill();
        mWorldStartRegionLayout->kill();
        al::updateKitListPostOnNerveEnd(this);
        return;
    }
    if (tryStartWarp()) {
        mWorldStartCountryLayout->kill();
        mWorldStartRegionLayout->kill();
        al::updateKitListPostOnNerveEnd(this);
        return;
    }
    if (rs::tryChangeNextStage(mGameDataHolder, this)) {
        kill();
        return;
    }
    if (al::isDead(mWorldStartCountryLayout)) {
        al::activateAudioEventController(this);
        updateKitListPostAndChangeNextNerve(&NrvStageScene.StartStageBgm);
        return;
    }
    alSeFunction::tryHoldStageStartSe(mStageStartAtmosSe);
    al::updateKitListPostOnNerveEnd(this);
}

void StageScene::exeWaitWarpToCheckpoint() {
    const sead::Vector3f& playerPos = al::getPlayerPos(al::getScenePlayerHolder(this), 0);
    mCheckpointWarpParabolicPathPos.y = playerPos.y;
    mCheckpointWarpParabolicPathPos.x = playerPos.x;
    mCheckpointWarpParabolicPathPos.z = playerPos.z;

    if (al::isFirstStep(this)) {
        mMapLayout->end();
        rs::changeDemoCommonProc(this, mProjectItemDirector);
        tryAppearDemoLayout();
        rs::requestStartDemoWarpToCheckpoint(this);

        PlayerDemoFunction::startCapCheckpointWarpMode(mCheckpointWarpCapActor);
        rs::hideDemoPlayer(mCheckpointWarpCapActor);
        rs::startActionDemoPlayer(mCheckpointWarpCapActor, "Wait");
        mCheckpointWarpCapActor->kill();
        al::startAction(mCheckpointWarpCapActor, "DemoWarpStart");
        rs::addDemoActor(mCheckpointWarpCapActor, false);

        const sead::Vector3f& headPos = rs::getPlayerHeadPos(mCheckpointWarpCapActor);
        al::resetPosition(mCheckpointWarpCapActor, headPos);

        const sead::Vector3f& cameraPos = al::getCameraPos(this, 0);
        const sead::Vector3f& cameraAt = al::getCameraAt(this, 0);
        sead::Vector3f cameraDir = cameraPos - cameraAt;
        cameraDir.normalize();

        const sead::Vector3f& cameraAt2 = al::getCameraAt(this, 0);
        mCheckpointWarpTargetPos.x = cameraDir.x + cameraAt2.x;
        mCheckpointWarpTargetPos.y = cameraDir.y + cameraAt2.y + 200.0f;
        mCheckpointWarpTargetPos.z = cameraDir.z + cameraAt2.z;

        bool isHome = mStateCollectionList->getStateStageMap()->isWarpHome();
        if (isHome) {
            const sead::Vector3f& homeTrans = GameDataFunction::getHomeTrans(mGameDataHolder);
            mCheckpointWarpParabolicPathEnd = homeTrans;
        } else {
            s32 idx = mStateCollectionList->getStateStageMap()->calcWarpCheckpointIndex();
            const sead::Vector3f& cpTrans =
                GameDataFunction::getCheckpointTransInWorld(mGameDataHolder, idx);
            mCheckpointWarpParabolicPathEnd = cpTrans;
        }

        sead::Vector3f diff = {mCheckpointWarpParabolicPathEnd.x - playerPos.x, 0.0f,
                               mCheckpointWarpParabolicPathEnd.z - playerPos.z};
        al::tryNormalizeOrZero(&diff);
        mGameDataHolder->setNextPlayerDir(diff);

        if (!al::isPlayingBgm(this, "CollectBgm")) {
            al::stopAllBgm(this, 120);
            mCollectBgmPlayer->reset();
        }
    }

    al::updateKitListPrev(this);
    rs::updateKitListDemoPlayerWithPauseEffectAndCalcAtEndClip(this);
    al::updateKitListPostDemoWithPauseNormalEffect(this);

    if (al::isFirstStep(this)) {
        s32 frameMax = (s32)al::getActionFrameMax(mCheckpointWarpCapActor, "DemoWarpStart");
        al::startCamera(this, mCheckpointWarpArriveCamera, frameMax);
    }

    bool actionEnd = al::isActionEnd(mCheckpointWarpCapActor);
    al::updateKitListPostOnNerveEnd(this);
    if (actionEnd)
        al::setNerve(this, &NrvStageScene.WarpToCheckpoint);
}

void StageScene::exeWarpToCheckpoint() {
    if (al::isFirstStep(this)) {
        al::startAction(mCheckpointWarpCapActor, "DemoWarpFly");
        mCheckpointWarpParabolicPath->initFromUpVector(al::getTrans(mCheckpointWarpCapActor),
                                                       mCheckpointWarpParabolicPathEnd,
                                                       sead::Vector3f::ey, 1500.0f);
    }

    sead::Vector3f pos = sead::Vector3f::zero;
    s32 pathTime = mCheckpointWarpParabolicPath->calcPathTimeFromAverageSpeed(30.0f);
    f32 speed = mCheckpointWarpParabolicPath->calcPathSpeedFromAverageSpeed(30.0f);
    s32 step = al::getNerveStep(this);

    f32 param;
    if (pathTime <= 239)
        param = al::calcNerveRate(this, 240);
    else
        param = speed * (f32)step;

    mCheckpointWarpParabolicPath->calcPosition(&pos, param);
    sead::Vector3f dir = sead::Vector3f::zero;
    mCheckpointWarpParabolicPath->calcDirection(&dir, param, 0.01f);

    sead::Quatf quat = sead::Quatf::unit;
    rs::calcQuatPoseCheckpointWarpCap(&quat, dir, this);
    al::resetQuatPosition(mCheckpointWarpCapActor, quat, pos);

    mCheckpointWarpParabolicPathPos = pos;

    al::updateKitListPrev(this);
    rs::updateKitListDemoPlayerWithPauseEffectAndCalcAtEndClip(this);
    al::updateKitListPostDemoWithPauseNormalEffect(this);

    if (al::isStep(this, 60) && mWipeHolder)
        mWipeHolder->startCloseByInfo("FadeBlack");

    if (al::isGreaterEqualStep(this, 120)) {
        bool isHome = mStateCollectionList->getStateStageMap()->isWarpHome();
        GameDataHolder* gdh = mGameDataHolder;
        if (isHome) {
            GameDataFunction::changeNextSceneByHome(gdh);
        } else {
            s32 idx = mStateCollectionList->getStateStageMap()->calcWarpCheckpointIndex();
            GameDataFunction::changeNextSceneByGotCheckpoint(gdh, idx);
        }
        kill();
    } else {
        al::updateKitListPostOnNerveEnd(this);
    }
}

bool StageScene::isOpenEndWorldMapWipe() const {
    return !mStateWorldMap || mStateWorldMap->isOpenEndWipe();
}

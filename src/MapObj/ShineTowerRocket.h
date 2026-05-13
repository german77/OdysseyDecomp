#pragma once

#include <basis/seadTypes.h>
#include <math/seadMatrix.h>
#include <math/seadQuat.h>
#include <math/seadVector.h>

#include "Library/Event/IEventFlowEventReceiver.h"
#include "Library/LiveActor/LiveActor.h"

#include "Demo/IUseDemoSkip.h"
#include "MapObj/LinkIdPair.h"

namespace al {
struct ActorInitInfo;
class AreaObj;
class EventFlowExecutor;
class HitSensor;
class SensorMsg;
template <class T>
class DeriveActorGroup;
class AddDemoInfo;
class CameraTicket;
class IUseCamera;
class IUseSceneObjHolder;
class RateParamV3f;
class RumbleCalculator;
class WipeSimple;
class AreaShapeCube;
class LiveActorGroup;
class MtxConnector;
}  // namespace al

class CapHanger;
class DoorAreaChange;
class CapTargetInfo;
class DokanInfo;
class DokanPuppetController;
class GoalMark;
class LinkObjAliveDeadCtrl;
class Bird;
class ShineTowerBackDoor;
class ShineTowerCommonKeeper;
class ShineTowerLight;
class ShineTowerRocket;
class ShineTowerGlobeAnimCtrl;
class DamageModel;
class DemoShine;

class IUsePlayerPuppet;

class ShineTowerRocket : public al::LiveActor,
                         public al::IEventFlowEventReceiver,
                         public IUseDemoSkip {
public:
    ShineTowerRocket(const char*);
    void init(const al::ActorInitInfo& info) override;
    void onSwitchDither();
    void offSwitchDither();
    void makeActorDead() override;
    void makeActorAlive() override;
    void initAfterPlacement() override;
    void startClipped() override;
    void control() override;
    bool isActiveDirtyModel() const;
    void calcAnim() override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;
    void attackSensor(al::HitSensor* self, al::HitSensor* other) override;
    bool receiveEvent(const al::EventFlowEventData* event) override;
    void tryStartEntranceCamera(s32);
    bool isFirstDemo() const override;
    bool isEnableSkipDemo() const override;
    void skipDemo() override;
    void exeWait();
    void updateParts();
    bool isNearPlayerEntrance() const;
    void exeReaction();
    void exeDemoPrepare();
    bool tryStartDemo();
    void tryEndEntranceCamera();
    void exeDemoWalkPlayerToPoint();
    void calcPlayerPoseForPayDemo();
    void tryStartHitReactionDemoStart();
    void exeDemoAppearShine();
    void exeDemoWaitAfterAppearShine();
    bool tryLevelUp();
    void exeDemoWaitBeforeScaleUpDirect();
    void calcCameraMtxMeterUpPrev();
    void exeDemoScaleUp();
    void exeDemoMeterRotate();
    void calcCameraMtx();
    void setupRotateMeter();
    void exeDemoMeterUpPrev();
    void exeDemoMeterUp();
    void exeDemoMeterUpPost();
    void exeDemoTutorialShine();
    void exeDemoSelectGoOtherWorld();
    void exeDemoAwardMoon();
    void exeDemoUpLevelCamera();
    void calcCameraMtxLevelUp();
    void exeDemoUpLevel();
    void exeDemoInformPowerUp();
    void exeDemoInformPowerUpMessage();
    void exeDemoKoopaShip();
    void exeDemoKoopaShipFade();
    void exeDemoUpLevelCloseFade();
    void exeDemoUpLevelWaitFade();
    void exeDemoUpLevelOpenFade();
    void exeDemoInformNewHome();
    void exeDemoInformNewHomeMessage();
    void exeDemoInformPeachCastleCap();
    void exeDemoInformRepairHome();
    void exeDemoInformNewItem();
    void exeDemoInformCompleteShineFadeIn();
    void exeDemoInformCompleteShineFadeWait();
    void exeDemoInformCompleteShineFadeOut();
    void exeDemoInformCompleteShine();
    void exeDemoWarpWorld();
    void exeWaitDemo();
    void exeDemoAppearPlayerFromHome();
    bool isActiveDamageModel() const;
    void exeDemoAppearPlayerFromHomeAfter();
    void exeDemoReturnToHome();
    bool isActiveDirtyOrClashModel() const;
    void exeDemoWorldTakeoff();
    void exeDemoWorldTakeoffNext();
    void setupWorldTakeoffPose(bool);
    void exeDemoAppearFromEntrance();
    void exeDemoWorldTakeoffForDebug();
    void exeNoStart();
    void exeNoStartEarth();
    void exeNoStartEnter();
    void exeBackDoor();
    void exeNoStartAndCoin();
    void exeGoToWorldMapWithCamera();
    void setupWorldMapCameraParam();
    void exeGoToWorldMapWithFade();
    void exeWorldMap();
    void cancelWorldMap();
    void decideWorldMap(s32);
    bool isWorldMap() const;
    void startDemoAppearPlayerFromHome();
    void startDemoReturnToHome();
    void updatePartsByDamage();
    bool isActiveClashModel() const;

    DoorAreaChange* getDoorAreaChange() const { return mDoorAreaChange; }

    DamageModel* getDamageModel() const { return mDamageModel; }

    ShineTowerLight* getShineTowerLight() const { return mShineTowerLight; }

    al::WipeSimple* getWorldMapWipe() const { return mWorldMapWipe; }

    al::WipeSimple* getDemoWipe() const { return mDemoWipe; }

    al::EventFlowExecutor* getEventFlowExecutor() const { return mEventFlowExecutor; }

    al::EventFlowExecutor* getDemoEventFlowExecutor() const { return mDemoEventFlowExecutor; }

    al::CameraTicket* getDemoAppearFromHomeCameraTicket() const {
        return mDemoAppearFromHomeCameraTicket;
    }

    const sead::Vector3f& getDemoReturnPlayerTrans() const { return mDemoReturnPlayerTrans; }

    const sead::Quatf& getDemoReturnPlayerQuat() const { return mDemoReturnPlayerQuat; }

    ShineTowerCommonKeeper* getShineTowerCommonKeeper() const { return mShineTowerCommonKeeper; }

    al::IUseCamera* getIUseCamera() { return this; }

    const al::IUseSceneObjHolder* getSceneObjHolderBase() const { return this; }

private:
    CapTargetInfo* mCapTargetInfo = nullptr;
    al::DeriveActorGroup<DemoShine>* mDemoShineGroup = nullptr;
    bool mIsDemoShineInEffectEmitted = false;
    s32 mDemoShineNum = 0;
    s32 mDemoShineIndex = 0;
    bool mIsWorldMap = false;
    bool mIsWorldMapCamera = false;
    s32 mWorldId = 0;
    al::CameraTicket* mWorldMapCameraTicket = nullptr;
    sead::Vector3f mWorldMapCameraPos = sead::Vector3f::zero;
    sead::Vector3f mWorldMapCameraAt = sead::Vector3f::zero;
    al::CameraTicket* mDemoAppearFromHomeCameraTicket = nullptr;
    sead::Matrix34f mDemoPlayerMtx = sead::Matrix34f::ident;
    f32 mMeterRotateDegree = 0.0f;
    sead::Matrix34f mScaleRootMtx = sead::Matrix34f::ident;
    ShineTowerCommonKeeper* mShineTowerCommonKeeper = nullptr;
    al::MtxConnector* mMtxConnector = nullptr;
    sead::Vector3f mDemoReturnPlayerTrans = sead::Vector3f::zero;
    sead::Quatf mDemoReturnPlayerQuat = sead::Quatf::unit;
    al::EventFlowExecutor* mDemoEventFlowExecutor = nullptr;
    al::EventFlowExecutor* mDemoEventFlowExecutorSub = nullptr;
    al::EventFlowExecutor* mEventFlowExecutor = nullptr;
    DoorAreaChange* mDoorAreaChange = nullptr;
    al::LiveActor* mHomeMeterActor = nullptr;
    ShineTowerBackDoor* mHomeActor = nullptr;
    al::LiveActor* mDirtyModel = nullptr;
    al::LiveActor* mClashModel = nullptr;
    DamageModel* mDamageModel = nullptr;
    al::LiveActor* mShineTowerRock = nullptr;
    al::LiveActor* mWaterfallWorldDemoStepActor = nullptr;
    al::LiveActor* mWaterfallWorldHomeRockBreakActor = nullptr;
    al::WipeSimple* mDemoWipe = nullptr;
    al::WipeSimple* mWorldMapWipe = nullptr;
    al::CameraTicket* mWorldMapFadeCameraTicket = nullptr;
    sead::Vector3f mPayCameraPos = sead::Vector3f::zero;
    sead::Vector3f mPayCameraAt = sead::Vector3f::zero;
    f32 mPayCameraAngle = 0.0f;
    sead::Vector3f mDemoPayPlayerTrans = sead::Vector3f::zero;
    sead::Quatf mDemoPayPlayerQuat = sead::Quatf::unit;
    al::LiveActor* mHomeSubActor = nullptr;
    al::CameraTicket* mDemoReturnToHomeCameraTicket = nullptr;
    sead::Matrix34f mDemoReturnToHomeMtx = sead::Matrix34f::ident;
    sead::Vector3f mPlayerRestartTrans = sead::Vector3f::zero;
    sead::Quatf mPlayerRestartQuat = sead::Quatf::unit;
    s32 mPlayerRestartId = 0;
    bool mIsTryStartDemoStarted = false;
    f32 mDemoMeterUpStartFrame = 0.0f;
    f32 mDemoMeterUpEndFrame = 0.0f;
    bool mIsDemoWaitingToEnd = false;
    al::RumbleCalculator* mDemoShineRumbleCalculator = nullptr;
    sead::Vector3f mPlayerPuppetScale = sead::Vector3f::ones;
    sead::Vector3f mCapTargetOffset = sead::Vector3f::zero;
    IUsePlayerPuppet* mPlayerPuppet = nullptr;
    GoalMark* mBackDoor = nullptr;
    al::LiveActor* mPoseCopyActor = nullptr;
    al::LiveActor* mDemoPlayerActor = nullptr;
    al::LiveActor* mDemoPlayerActor2 = nullptr;
    al::LiveActor* mDemoMarioCapActor = nullptr;
    al::LiveActor* mCapManHeroEyesActor = nullptr;
    al::CameraTicket* mEntranceCameraTicket = nullptr;
    al::RateParamV3f* mWorldMapCameraAtRateParam = nullptr;
    al::RateParamV3f* mWorldMapCameraPosRateParam = nullptr;
    sead::Vector3f mEntranceCameraFront = sead::Vector3f::zero;
    LinkIdPair mPlayerRestartLinkId;
    char _39e[0x2];  //???
    al::AreaObj* mDokanActor = nullptr;
    DokanPuppetController* mDokanPuppetController = nullptr;
    DokanInfo* mDokanInfo = nullptr;
    al::LiveActor* mDokanDemoActor = nullptr;
    LinkIdPair mDokanDemoLinkId;
    char _3c2[0x6];  //???
    al::LiveActor* mDemoPeachCastleCapActor = nullptr;
    bool mIsCompleteShine = false;
    bool mIsDemoPeachCastleCap = false;
    bool mIsFirstPayNotEnough = false;
    bool mIsPlayDemoAwardSpecial = false;
    al::DeriveActorGroup<Bird>* mBirdGroup = nullptr;
    al::DeriveActorGroup<CapHanger>* mHomeDemoActorGroup = nullptr;
    LinkObjAliveDeadCtrl* mLinkObjAliveDeadCtrl = nullptr;
    bool mIsAppearCoin = false;
    s32 mMeterReactionCoolTime = 0;
    sead::Vector3f mHomeGlobeCapPoint = sead::Vector3f::zero;
    sead::Vector3f mClippingOffset = sead::Vector3f::zero;
    al::AreaShapeCube* mEntranceCameraAreaShape = nullptr;
    sead::Matrix34f mEntranceCameraAreaMtx = sead::Matrix34f::ident;
    al::CameraTicket* mFixDoorwayCameraTicket = nullptr;
    bool mIsFixDoorwayCamera = false;
    ShineTowerGlobeAnimCtrl* mShineTowerGlobeAnimCtrl = nullptr;
    f32 mRopeRootRotateDegree = 0.0f;
    ShineTowerLight* mShineTowerLight = nullptr;
    al::AddDemoInfo* mAddDemoInfo = nullptr;
    sead::Matrix34f mDokanDemoMtx = sead::Matrix34f::ident;
    sead::Vector3f mCameraRotateEastTrans = sead::Vector3f::zero;
    sead::Vector3f mCameraRotateWestTrans = sead::Vector3f::zero;
    sead::Vector3f mCameraRotateEastRot = sead::Vector3f::zero;
    sead::Vector3f mCameraRotateWestRot = sead::Vector3f::zero;
    bool mIsStartHitReactionDemoStart = false;
};

static_assert(sizeof(ShineTowerRocket) == 0x4e0);

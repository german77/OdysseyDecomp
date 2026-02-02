#include "Layout/MapLayout.h"

#include "Library/Area/AreaObj.h"
#include "Library/Area/AreaObjUtil.h"
#include "Library/Base/StringUtil.h"
#include "Library/Camera/CameraUtil.h"
#include "Library/Layout/LayoutActionFunction.h"
#include "Library/Layout/LayoutActorUtil.h"
#include "Library/Layout/LayoutInitInfo.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Matrix/MatrixUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Play/Layout/SimpleLayoutAppearWaitEnd.h"
#include "Library/Player/PlayerHolder.h"
#include "Library/Player/PlayerUtil.h"
#include "Library/Scene/SceneObjUtil.h"
#include "Library/Se/SeFunction.h"

#include "Layout/DecideIconLayout.h"
#include "Layout/MapTerrainLayout.h"
#include "Layout/TalkMessage.h"
#include "Scene/SceneObjFactory.h"
#include "Sequence/GameSequenceInfo.h"
#include "System/GameDataFunction.h"
#include "System/MapDataHolder.h"
#include "Util/PlayerUtil.h"
#include "Util/StageLayoutFunction.h"

namespace {
NERVE_IMPL(MapLayout, Appear)
NERVE_IMPL_(MapLayout, AppearWithHint, Appear)
NERVE_IMPL_(MapLayout, AppearMoonRockDemo, Appear)
NERVE_IMPL_(MapLayout, HintAppearNpc, HintAppear)
NERVE_IMPL_(MapLayout, HintAppearMoonRock, HintAppear)
NERVE_IMPL_(MapLayout, HintAppearAmiibo, HintAppear)
NERVE_IMPL_(MapLayout, ChangeIn, Appear)
NERVE_IMPL(MapLayout, Wait)
NERVE_IMPL_(MapLayout, HintInitWaitAmiibo, HintInitWait)
NERVE_IMPL_(MapLayout, HintInitWaitNpc, HintInitWait)
NERVE_IMPL_(MapLayout, HintInitWaitMoonRock, HintInitWait)
NERVE_IMPL(MapLayout, HintAppear)
NERVE_IMPL_(MapLayout, HintDecideIconAppearNpc, HintDecideIconAppear)
NERVE_IMPL_(MapLayout, HintDecideIconAppearAmiibo, HintDecideIconAppear)
NERVE_IMPL_(MapLayout, HintDecideIconAppearMoonRock, HintDecideIconAppear)
NERVE_IMPL(MapLayout, HintDecideIconWait)
NERVE_IMPL(MapLayout, HintPressDecide)
NERVE_IMPL(MapLayout, End)
NERVE_IMPL(MapLayout, ChangeOut)

NERVES_MAKE_NOSTRUCT(MapLayout, HintAppear, HintPressDecide, ChangeOut)
NERVES_MAKE_STRUCT(MapLayout, Appear, HintInitWaitAmiibo, HintInitWaitNpc, HintInitWaitMoonRock,
                   End, ChangeIn, Wait, AppearWithHint, AppearMoonRockDemo, HintAppearNpc,
                   HintAppearMoonRock, HintAppearAmiibo, HintDecideIconAppearNpc,
                   HintDecideIconAppearAmiibo, HintDecideIconAppearMoonRock, HintDecideIconWait)
}  // namespace

static const char* sPicImage[8] = {"PicImage00", "PicImage01", "PicImage02", "PicImage03",
                                   "PicImage04", "PicImage05", "PicImage06", "PicImage07"};

static const char* sPanelNames[21] = {"TxtCaption00",
                                      "TxtRegionCaption",
                                      "TxtData01",
                                      "TxtData02",
                                      "TxtTitle01",
                                      "TxtContents01",
                                      "TxtCaption01",
                                      "TxtTitle02",
                                      "TxtContents02",
                                      "TxtCaption02",
                                      "TxtTitle03"
                                      "TxtContents03",
                                      "TxtCaption03",
                                      "TxtTitle04",
                                      "TxtContents04",
                                      "TxtCaption04",
                                      "TxtTitle05",
                                      "TxtContents05",
                                      "TxtCaption05",
                                      "TxtCaption06",
                                      "TxtContents07"};

static const char* sIconType[19] = {
    "Flag",     "FlagDisable", "Home",  "HomeDisable", "Hint",     "Hint",         "Shop",
    "ShopSold", "Race",        "Start", "Goal",        "Scenario", "ScenarioHome", "HintRock",
    "HintRock", "Cap",         "Luigi", "Poet",        "Scenario"};

inline f32 modDegree(f32 value) {
    return al::modf(value + 360.0f, 360.0f) + 0.0f;
}

MapLayout::MapLayout(const al::LayoutInitInfo& initInfo, const al::PlayerHolder* playerHolder,
                     s32 worldId)
    : al::LayoutActor("マップ"), mPlayerHolder(playerHolder) {
    al::initLayoutActor(this, initInfo, "Map", nullptr);
    initNerve(&NrvMapLayout.Appear, 0);
    al::startAction(this, "Appear", nullptr);

    HintAmiibo* hint = new HintAmiibo[3];
    if (hint != nullptr) {
        mHintAmiiboSizer = 3;
        mHintAmiibo = hint;
    }

    mWaitEndMapBg =
        new al::SimpleLayoutAppearWaitEnd("[マップ]背景", "MapBG", initInfo, nullptr, false);
    mWaitEndMapPlayer = new al::SimpleLayoutAppearWaitEnd("[マップ]プレイヤー位置", "MapPlayer",
                                                          initInfo, nullptr, false);
    mWaitEndMapCursor = new al::SimpleLayoutAppearWaitEnd("[マップ]カーソル", "MapCursor", initInfo,
                                                          nullptr, false);
    mWaitEndMapGuide =
        new al::SimpleLayoutAppearWaitEnd("[マップ]ガイド", "MapGuide", initInfo, nullptr, false);
    mWaitEndMapLine = new al::SimpleLayoutAppearWaitEnd("[マップ]カーソルライン", "MapLine",
                                                        initInfo, nullptr, false);

    mTalkMessage = new TalkMessage("[マップ]会話メッセージ");
    mTalkMessage->initLayoutOver(initInfo, "StageMap");
    mDecideIconLayout = new DecideIconLayout("[マップ]決定ボタン", initInfo);

    s32 checkpointNumMax = GameDataFunction::getCheckpointNumMaxInWorld(this);
    s32 hintNumMax = GameDataFunction::getHintNumMax(this);
    s32 shopNpcIconNumMax = GameDataFunction::getShopNpcIconNumMax(this);
    s32 miniGameNumMax = GameDataFunction::getMiniGameNumMax(this);
    s32 mainScenarioNumMax = GameDataFunction::getMainScenarioNumMax(this);
    s32 hintMoonRockNumMax = GameDataFunction::getHintMoonRockNumMax(this);

    mCheckpointNumMaxInWorld = checkpointNumMax;
    mHintNumMax = hintNumMax;

    s32 mapIconSizeNumMax = checkpointNumMax + hintNumMax + shopNpcIconNumMax + miniGameNumMax +
                            mainScenarioNumMax + hintMoonRockNumMax * 2 + 7;
    MapIconInfo* iconInfo = new MapIconInfo[mapIconSizeNumMax];
    for (s32 i = 0; i < mapIconSizeNumMax; i++) {
        iconInfo[i] = {
            .iconLayout = nullptr,
            .isActive = false,
            .position = sead::Vector3f::zero,
            .iconType = IconType::Scenario2,
        };
    }
    mMapIconInfo = iconInfo;
    mMapIconInfoSize = mapIconSizeNumMax;
    mMainScenarioNumMax = mainScenarioNumMax;

    mMapIconLayoutA = new MapIconLayout(0, 0, nullptr);
    mMapIconLayoutB = new MapIconLayout(0, 0, nullptr);
    mMapIconLayoutC = new MapIconLayout(0, 0, nullptr);
    mMapIconLayoutD = new MapIconLayout(0, 0, nullptr);

    setPanelName(this, GameDataFunction::getWorldDevelopName(this, worldId), worldId);
    mMapTerrainLayout = new MapTerrainLayout("[マップ]地形");
    al::initLayoutPartsActor(mMapTerrainLayout, this, initInfo, "ParMap", nullptr);

    al::startAction(mMapTerrainLayout, "Wait", nullptr);
    if (GameDataFunction::isWorldSnow(this)) {
        mArray.allocBuffer(5, nullptr);
        for (s32 i = 0; i < 5; i++) {
            al::LayoutActor* layActor = new al::LayoutActor("[マップ]アイコンライン");
            al::initLayoutActor(layActor, initInfo, "MapIconLine", nullptr);
            mArray.pushBack(layActor);
        }
    }

    changePrintWorld(worldId);
    loadTexture();
    appear();
}

void MapLayout::appear() {
    s32 size = mMapIconInfoSize;
    MapIconInfo* iconInfo = mMapIconInfo;
    for (s32 i = 0; i < size; i++) {
        iconInfo->isActive = false;
        iconInfo->iconType = IconType::MaxValue;
        iconInfo->position = sead::Vector3f::zero;
        iconInfo->action = 0;
        iconInfo->position2 = sead::Vector2f::zero;
        iconInfo->value = false;
        iconInfo++;
    }

    if (mIsPrintWorldChanged) {
        sead::Vector2f prevLocalSize = mPanelLocalScale;
        sead::Vector2f prevPanelSize = mPanelSize;
        reset();

        if (!mIsResetTransform) {
            mPanelLocalScale = prevLocalSize;
            mPanelSize = prevPanelSize;
        }
        mIsResetTransform = false;

        al::LiveActor* playerActor = al::tryGetPlayerActor(mPlayerHolder, 0);
        sead::Vector3f position = {0.0f, 0.0f, 0.0f};

        if (playerActor == nullptr) {
            moveFocusLayout(position, sead::Vector2f::zero);
        } else {
            position.set(al::getTrans(playerActor));
            al::AreaObj* areaObj =
                al::tryFindAreaObj(playerActor, "InvalidateStageMapArea", position);
            if (areaObj != nullptr) {
                al::getLinksQT(nullptr, &position, *areaObj->getPlacementInfo(), "PlayerPoint");
            } else {
                mScrollPosition = sead::Vector2f::zero;
                if (!GameDataFunction::isMainStage(this))
                    position.set(GameDataFunction::getStageMapPlayerPos(playerActor));
            }
        }

        moveFocusLayout(position, sead::Vector2f::zero);
        updateST();
    }

    mMapTerrainLayout->appear();
    al::LayoutActor::appear();
    al::setNerve(this, &NrvMapLayout.Appear);
    al::startAction(this, "Appear", nullptr);
    al::startAction(this, "LeftIn", "Change");
    al::setActionFrame(this, al::getActionFrameMax(this, "LeftIn", "Change"), "Change");

    if (mIsHelp)
        mWaitEndMapBg->appear();
}

void MapLayout::control() {
    if (al::isActive(mWaitEndMapLine))
        updateLine(mMapIconLayout->layout);
}

const char* MapLayout::getSceneObjName() const {
    return "マップレイアウト";
}

void setPanelName(al::LayoutActor* layoutActor, const char* message, s32 id) {
    al::setPaneString(layoutActor, "TxtWorld", GameDataFunction::tryGetWorldName(layoutActor, id),
                      0);
    al::setPaneString(layoutActor, "TxtRegion",
                      GameDataFunction::tryGetRegionNameCurrent(layoutActor), 0);
    for (s32 i = 0; i < 21; i++) {
        al::StringTmp<64> stageMapMessage = {"%s_%s", sPanelNames[i], message};
        rs::trySetPaneSystemMessageIfExist(layoutActor, sPanelNames[i], "StageMapMessage",
                                           stageMapMessage.cstr());
    }
}

void MapLayout::changePrintWorld(s32 worldId) {
    mIsPrintWorldChanged = mMapTerrainLayout->tryChangePrintWorld(worldId);
    mWorldId = worldId;
    reset();
}

void MapLayout::loadTexture() {
    const char* worldDevelopName = GameDataFunction::getWorldDevelopName(this, mWorldId);
    for (s32 i = 0; i < 8; i++) {
        al::StringTmp<128> texturePath = {"ObjectData/TextureMapLayout%s", worldDevelopName};
        al::StringTmp<128> textureFolder = {"TextureMapLayout%s", worldDevelopName};
        al::StringTmp<128> textureName = {"%s%s", worldDevelopName, sPicImage[i]};
        nn::ui2d::TextureInfo* textureInfo =
            al::createTextureInfo(texturePath.cstr(), textureFolder.cstr(), textureName.cstr());
        al::setPaneTexture(this, sPicImage[i], textureInfo);
    }
}

void MapLayout::reset() {
    mPanelLocalScale.y = 0.4f;
    mPanelLocalScale.x = 0.4f;
    f32 map = mMapTerrainLayout->getPaneSize();
    mPanelSize.x = map * mPanelLocalScale.x;
    mPanelSize.y = map * mPanelLocalScale.y;
    mScrollPosition = sead::Vector2f::zero;
}

// TODO: Move to sead
inline void mul(sead::Vector2f& o, const sead::Matrix44f& m, const sead::Vector3f& a) {
    const sead::Vector3f tmp = a;
    o.x = m.m[0][0] * tmp.x + m.m[0][1] * tmp.y + m.m[0][2] * tmp.z + m.m[0][3];
    o.y = m.m[1][0] * tmp.x + m.m[1][1] * tmp.y + m.m[1][2] * tmp.z + m.m[1][3];
}

void MapLayout::moveFocusLayout(const sead::Vector3f& pos3D, const sead::Vector2f& pos2D) {
    if (!mIsPrintWorldChanged)
        return;

    MapData* mapData = mMapTerrainLayout->getMapData();
    const sead::Matrix44f& mtx = mapData->viewProjMatrix;
    sead::Vector2f proj2;
    mul(proj2, mtx, pos3D);

    f32 scale = mMapTerrainLayout->getPaneSize() * 0.5f;
    sead::Vector2f proj = proj2 * scale;

    bool isYClamped = false;
    if (proj.y > 955.0f) {
        proj.y = 955.0f;
        isYClamped = true;
    } else if (proj.y < -985.0f) {
        proj.y = -985.0f;
        isYClamped = true;
    }

    if (proj.x > 985.0f) {
        proj.x = 985.0f;
        isYClamped = true;
    } else if (proj.x < -955.0f) {
        proj.x = -955.0f;
        isYClamped = true;
    }

    sead::Vector3f direction = sead::Vector3f::zero;
    if (isYClamped) {
        f32 angle = al::calcAngleDegree(sead::Vector2f::ey, sead::Vector2f(-proj.x, -proj.y));
        al::rotateVectorDegreeZ(&direction, modDegree(angle));
    }

    // TODO: Implement -vec2f
    mScrollPosition.x = (-proj.x - pos2D.x) - (direction.x * 2.5f);
    mScrollPosition.y = (-proj.y - pos2D.y) - (direction.y * 2.5f);
}

inline sead::Vector2f getOffset(s32 i) {
    if (i == 0)
        return {50.0f, 50.0f};
    if (i == 1)
        return {-50.0f, 50.0f};

    return i == 4 ? sead::Vector2f{50.0f, 0.0f} : sead::Vector2f::zero;
}

void MapLayout::updateST() {
    if (!mIsPrintWorldChanged)
        return;

    al::setPaneLocalScale(this, "All", mPanelLocalScale);
    sead::Vector2f localTrans(mScrollPosition.x * mPanelLocalScale.x,
                              mScrollPosition.y * mPanelLocalScale.y);
    al::setPaneLocalTrans(this, "All", localTrans);

    MapData* mapData = mMapTerrainLayout->getMapData();
    if (al::tryGetPlayerActor(mPlayerHolder, 0))
        updatePlayerPosLayout();

    for (s32 i = 0; i < mMapIconInfoSize; i++) {
        if (!mMapIconInfo[i].isActive)
            continue;

        if (mMapIconInfo[i].value) {
            sead::Vector3f projectedPos = sead::Vector3f::zero;
            calcSeaOfTreeIconPos(&projectedPos);
            al::setLocalTrans(mMapIconInfo[i].iconLayout->layout, projectedPos);
        } else {
            sead::Vector2f projectedPos;
            rs::calcTransOnMap(&projectedPos, mMapIconInfo[i].position, mapData->viewProjMatrix,
                               mScrollPosition, mPanelLocalScale.x, 2.0f);
            al::setLocalTrans(mMapIconInfo[i].iconLayout->layout, projectedPos);
        }
    }

    setPanelFont(mPanelLocalScale.x, &mCurrentFontType, this, mCurrentFontType);

    s32 size = mArray.size();
    for (s32 i = 0; i < size; i++)
        if (al::isActive(mArray[i]))
            updateIconLine(mArray[i], mAAA, getOffset(i));
}

HintAmiibo& getHintAmiibo(s32 i, u32 maxSize, HintAmiibo* mHintAmiibo) {
    if (maxSize > (u32)i)
        return mHintAmiibo[i];
    return mHintAmiibo[0];
}

void MapLayout::addAmiiboHint() {
    getHintAmiibo(mHintDecideIconAmiiboSize, mHintAmiiboSizer, mHintAmiibo).position =
        GameDataFunction::getLatestHintTrans(this);
    getHintAmiibo(mHintDecideIconAmiiboSize, mHintAmiiboSizer, mHintAmiibo).isValid =
        GameDataFunction::checkLatestHintSeaOfTree(this);
    mHintDecideIconAmiiboSize++;
}

void MapLayout::appearAmiiboHint() {
    if (-1 < mWorldId) {
        s32 size = mMapIconInfoSize;
        for (s32 i = 0; i < size; i++) {
            MapIconInfo& iconInfo = mMapIconInfo[i];
            iconInfo.isActive = false;
            iconInfo.iconType = IconType::MaxValue;
            iconInfo.position = sead::Vector3f::zero;
            iconInfo.action = 0;
            iconInfo.position2 = sead::Vector2f::zero;
            iconInfo.value = false;
        }
        f32 map = mMapTerrainLayout->getPaneSize();
        reset();
        mPanelSize = {map * 0.4f, map * 0.4f};
        mPanelLocalScale.y = StageMapFunction::getStageMapScaleMin();
        mPanelLocalScale.x = StageMapFunction::getStageMapScaleMin();
        updateST();
        al::LayoutActor::appear();
        al::setNerve(this, &NrvMapLayout.HintInitWaitAmiibo);
        al::startAction(this, "Appear", nullptr);
        al::startAction(this, "LeftIn", "Change");
        al::setActionFrame(this, al::getActionFrameMax(this, "LeftIn", "Change"), "Change");
        mWaitEndMapBg->appear();
        MapData* mapData = mMapTerrainLayout->getMapData();
        mIsFreezeAction = trySetBalloon(mWaitEndMapPlayer, al::getPlayerActor(mPlayerHolder, 0),
                                        mapData->viewProjMatrix);
        updatePlayerPosLayout();
        mWaitEndMapPlayer->startWait();
        mIsResetTransform = true;
        return;
    }
    end();
}

void MapLayout::end() {
    al::setNerve(this, &NrvMapLayout.End);
}

inline f32 getV(f32 w, const sead::Vector3f& a, const sead::Vector3f& b) {
    return (w + b.x * a.x + b.y * a.y + b.z * a.z) * 1024.0f;
}

bool trySetBalloon(al::LayoutActor* layoutActor, const al::LiveActor* actor,
                   const sead::Matrix44f& matrix) {
    sead::Vector3f position = al::getTrans(actor);

    sead::Vector3f position2 = al::getTrans(actor);
    f32 ax =
        getV(matrix(0, 3), {matrix(0, 0), matrix(0, 1), matrix(0, 2)}, position2) * 1024.0f * 0.4f +
        0.0f;
    f32 ay =
        getV(matrix(1, 3), {matrix(1, 0), matrix(1, 1), matrix(1, 2)}, position2) * 1024.0f * 0.4f +
        0.0f;
    if ((ax < -382.0f || 394.0f < ax) || (ay < -394.0f || 382.0f < ay)) {
        f32 fvar11 =
            getV(matrix(0, 3), {matrix(0, 0), matrix(0, 1), matrix(0, 2)}, position) * 0.4f + 0.0f;
        f32 fvar12 =
            getV(matrix(1, 3), {matrix(1, 0), matrix(1, 1), matrix(1, 2)}, position) * 0.4f + 0.0f;

        sead::Vector2f pos = {sead::Mathf::clamp(fvar11, -382.0f, 394.0f),
                              sead::Mathf::clamp(fvar12, -394.0f, 382.0f)};

        al::startFreezeAction(layoutActor, "Outside", 0.0f, "Icon");
        f32 angle = modDegree(al::calcAngleDegree(sead::Vector2f::ey, pos * -0.4f));
        f32 frameMax = al::getActionFrameMax(layoutActor, "State", "Balloon");
        al::startFreezeAction(layoutActor, "State", angle * (frameMax / 360.0f), "Balloon");
        return true;
    }
    return false;
}

void MapLayout::updatePlayerPosLayout() {
    al::LiveActor* player = al::getPlayerActor(mPlayerHolder, 0);
    f32 localScale = mPanelLocalScale.x;
    MapData* mapData = mMapTerrainLayout->getMapData();
    bool freeze = mIsFreezeAction;
    bool bVar10;
    bool bVar11;

    if (GameDataFunction::isSeaOfTreeStage(this)) {
        sead::Vector3f position;
        calcSeaOfTreeIconPos(&position);
        al::startFreezeAction(mWaitEndMapPlayer, "Stay", 0.0f, "Icon");
        al::setLocalTrans(mWaitEndMapPlayer, position);
        return;
    }

    sead::Vector2f position = sead::Vector2f::zero;
    sead::Vector3f playerPos = al::getTrans(player);

    if (!GameDataFunction::isMainStage(player))
        playerPos = GameDataFunction::getStageMapPlayerPos(this);

    al::AreaObj* areaObj = al::tryFindAreaObj(player, "InvalidateStageMapArea", playerPos);
    if (areaObj == nullptr) {
        position.x =
            localScale *
                (mapData->viewProjMatrix(3, 0) + playerPos.x * mapData->viewProjMatrix(0, 0) +
                 playerPos.y * mapData->viewProjMatrix(1, 0) +
                 playerPos.z * mapData->viewProjMatrix(2, 0)) *
                1024.0f +
            localScale * mScrollPosition.x;
        position.y =
            localScale * mScrollPosition.y +
            localScale *
                (mapData->viewProjMatrix(3, 1) + playerPos.x * mapData->viewProjMatrix(0, 1) +
                 playerPos.y * mapData->viewProjMatrix(1, 1) +
                 playerPos.z * mapData->viewProjMatrix(2, 1)) *
                1024.0f;
        if (!freeze) {
            bVar10 = false;
            bVar11 = false;
        } else {
            f32 fVar23 =
                localScale *
                    (mapData->viewProjMatrix(3, 0) + playerPos.x * mapData->viewProjMatrix(0, 0) +
                     playerPos.y * mapData->viewProjMatrix(1, 0) +
                     playerPos.z * mapData->viewProjMatrix(2, 0)) *
                    1024.0f +
                localScale * 0.0f;
            f32 fVar21 = localScale * 0.0f + localScale *
                                                 (mapData->viewProjMatrix(3, 1) +
                                                  playerPos.x * mapData->viewProjMatrix(0, 1) +
                                                  playerPos.y * mapData->viewProjMatrix(1, 1) +
                                                  playerPos.z * mapData->viewProjMatrix(2, 1)) *
                                                 1024.0f;
            f32 fVar22 = localScale * 985.0f;
            f32 fVar24 = localScale * 955.0f;
            if ((fVar24 < fVar21) || (fVar24 = localScale * -985.0f, fVar21 < fVar24))
                position.y = position.y - (fVar21 - fVar24);
            if ((fVar22 < fVar23) || (fVar22 = localScale * -955.0f, fVar23 < fVar22))
                position.x = position.x - (fVar23 - fVar22);
            bVar11 = false;
            bVar10 = true;
        }
    } else {
        sead::Vector3f pospos = sead::Vector3f::zero;
        al::getLinksQT(nullptr, &pospos, *areaObj->getPlacementInfo(), "PlayerPoint");
        position.x = localScale *
                         (mapData->viewProjMatrix(3, 0) + pospos.x * mapData->viewProjMatrix(0, 0) +
                          pospos.y * mapData->viewProjMatrix(1, 0) +
                          pospos.z * mapData->viewProjMatrix(2, 0)) *
                         1024.0f +
                     localScale * mScrollPosition.x;
        position.y = localScale * mScrollPosition.y +
                     localScale *
                         (mapData->viewProjMatrix(3, 1) + pospos.x * mapData->viewProjMatrix(0, 1) +
                          pospos.y * mapData->viewProjMatrix(1, 1) +
                          pospos.z * mapData->viewProjMatrix(2, 1)) *
                         1024.0f;
        al::startFreezeAction(mWaitEndMapPlayer, "Stay", 0.0f, "Icon");
        bVar10 = false;
        bVar11 = true;
    }
    al::setLocalTrans(mWaitEndMapPlayer, position);
    al::setLocalScale(mWaitEndMapPlayer, StageMapFunction::getStageMapScaleMax());

    sead::Vector3f pospos = sead::Vector3f::zero;
    rs::tryCalcMapNorthDir(&pospos, mWaitEndMapPlayer);

    sead::Vector3f plapplap = {0.0f, 0.0f, 0.0f};
    f32 angle;
    if (GameDataFunction::isMainStage(this)) {
        rs::calcPlayerFrontDir(&plapplap, player);
        angle = modDegree(al::calcAngleOnPlaneDegree(pospos, plapplap, sead::Vector3f::ey));
    } else {
        angle = 0.0f;
        al::startFreezeAction(mWaitEndMapPlayer, "Stay", 0.0f, "Icon");
        bVar11 = true;
    }

    al::startFreezeAction(
        mWaitEndMapPlayer, "State",
        angle * (al::getActionFrameMax(mWaitEndMapPlayer, "State", "Direct") / 360.0f), "Direct");

    sead::Vector3f cameraDif =
        al::getCameraAt(mWaitEndMapPlayer, 0) - al::getCameraPos(mWaitEndMapPlayer, 0);
    f32 popo = modDegree(al::calcAngleOnPlaneDegree(pospos, cameraDif, sead::Vector3f::ey));
    f32 max = al::getActionFrameMax(mWaitEndMapPlayer, "State", "View");
    al::startFreezeAction(mWaitEndMapPlayer, "State", popo * (max / 360.0f), "View");

    if (!bVar10 && !bVar11)
        al::startFreezeAction(mWaitEndMapPlayer, "Normal", 0.0f, "Icon");
}

void MapLayout::appearWithHint() {
    if (-1 < mWorldId) {
        s32 size = mMapIconInfoSize;
        for (s32 i = 0; i < size; i++) {
            MapIconInfo& iconInfo = mMapIconInfo[i];
            iconInfo.isActive = false;
            iconInfo.iconType = IconType::MaxValue;
            iconInfo.position = sead::Vector3f::zero;
            iconInfo.action = 0;
            iconInfo.position2 = sead::Vector2f::zero;
            iconInfo.value = false;
        }
        f32 map = mMapTerrainLayout->getPaneSize();
        reset();
        mPanelSize = {map * 0.4f, map * 0.4f};
        mPanelLocalScale.y = StageMapFunction::getStageMapScaleMin();
        mPanelLocalScale.x = StageMapFunction::getStageMapScaleMin();
        updateST();
        al::LayoutActor::appear();
        al::setNerve(this, &NrvMapLayout.HintInitWaitNpc);
        al::startAction(this, "Appear", nullptr);
        al::startAction(this, "LeftIn", "Change");
        al::setActionFrame(this, al::getActionFrameMax(this, "LeftIn", "Change"), "Change");
        mWaitEndMapBg->appear();
        MapData* mapData = mMapTerrainLayout->getMapData();
        mIsFreezeAction = trySetBalloon(mWaitEndMapPlayer, al::getPlayerActor(mPlayerHolder, 0),
                                        mapData->viewProjMatrix);
        updatePlayerPosLayout();
        mWaitEndMapPlayer->startWait();
        mIsResetTransform = true;
        return;
    }
    end();
}

void MapLayout::appearMoonRockDemo(s32 sworldId) {
    s32 size = mMapIconInfoSize;
    for (s32 i = 0; i < size; i++) {
        MapIconInfo& iconInfo = mMapIconInfo[i];
        iconInfo.isActive = false;
        iconInfo.iconType = IconType::MaxValue;
        iconInfo.position = sead::Vector3f::zero;
        iconInfo.action = 0;
        iconInfo.position2 = sead::Vector2f::zero;
        iconInfo.value = false;
    }
    changePrintWorld(sworldId);
    f32 map = mMapTerrainLayout->getPaneSize();
    reset();
    mPanelLocalScale.y = StageMapFunction::getStageMapScaleMin();
    mPanelLocalScale.x = StageMapFunction::getStageMapScaleMin();
    mPanelSize = {map * StageMapFunction::getStageMapScaleMin(),
                  map * StageMapFunction::getStageMapScaleMin()};
    updateST();
    al::LayoutActor::appear();
    al::setNerve(this, &NrvMapLayout.HintInitWaitMoonRock);
    al::startAction(this, "Appear", nullptr);
    al::startAction(this, "LeftIn", "Change");
    al::setActionFrame(this, al::getActionFrameMax(this, "LeftIn", "Change"), "Change");
    mWaitEndMapBg->appear();
    MapData* mapData = mMapTerrainLayout->getMapData();
    mIsFreezeAction = trySetBalloon(mWaitEndMapPlayer, al::getPlayerActor(mPlayerHolder, 0),
                                    mapData->viewProjMatrix);
    updatePlayerPosLayout();
    mWaitEndMapPlayer->startWait();
    mIsResetTransform = true;
    return;
}

void MapLayout::appearCollectionList() {
    mIsHelp = false;
    appear();
}

bool MapLayout::isEnd() const {
    return !isAlive();
}

bool MapLayout::isEnableCheckpointWarp() const {
    al::LiveActor* actor = al::tryGetPlayerActor(mPlayerHolder, 0);
    if (actor != nullptr) {
        if (!GameDataFunction::isInInvalidOpenMapStage(actor) && rs::isEnableOpenMap(actor) &&
            !GameDataFunction::isRaceStartYukimaru(actor) && GameDataFunction::isMeetCap(actor) &&
            !GameDataFunction::isRemovedCapByJango(actor)) {
            return !rs::isSceneStatusBossBattle(actor);
        }
    }
    return false;
}

void MapLayout::changeOut(bool isLeftOut) {
    al::startAction(this, isLeftOut ? "LeftOut" : "RightOut", "Change");
    al::startAction(mWaitEndMapCursor, "ChangeOut", nullptr);
    mWaitEndMapPlayer->kill();
    mWaitEndMapGuide->kill();
    s32 size = mArray.size();
    for (s32 i = 0; i < size; i++)
        mArray[i]->kill();
    for (s32 i = 0; i < mMapIconInfoSize; i++)
        if (mMapIconInfo[i].isActive && al::killLayoutIfActive(mMapIconInfo[i].iconLayout->layout))
            mMapIconInfo[i].isActive = false;
    al::setNerve(this, &ChangeOut);
}

void MapLayout::changeIn(bool isRightIn) {
    appearCollectionList();
    al::startAction(this, "Wait", nullptr);
    mWaitEndMapCursor->appear();
    mIsHelp = false;
    al::startAction(mWaitEndMapCursor, "ChangeIn", nullptr);
    al::startAction(this, isRightIn ? "RightIn" : "LeftIn", "Change");
    al::setNerve(this, &NrvMapLayout.ChangeIn);
}

void MapLayout::updateLine(al::LayoutActor* layoutActor) {
    al::Matrix43f panel = al::getPaneMtx(mWaitEndMapCursor, "LinePos");
    sead::Vector3f panelV = {panel.m[0][3], panel.m[1][3], panel.m[2][3]};

    al::Matrix43f panel2 = al::getPaneMtx(layoutActor, "LinePos");
    sead::Vector3f panelV2 = {panel2.m[0][3], panel2.m[1][3], panel2.m[2][3]};
    sead::Vector3f panelV3 = panelV2 - panelV;

    f32 angle = 0.0f;
    if (!al::isNearZero(panelV3.length(), 0.001f)) {
        sead::Vector3f direction = panelV3;
        angle = al::calcAngleOnPlaneDegree(direction, -sead::Vector3f::ex, sead::Vector3f::ez);
        angle = (360.0f - modDegree(angle)) * (3.2f / 9.0f);
    }

    al::startFreezeAction(mWaitEndMapLine, "State", angle, "Direct");

    f32 size = (panelV - panelV2).length() / 20.0f;
    al::startFreezeAction(mWaitEndMapLine, "State", size, "Length");
    sead::Vector3f newPosition = (panelV + panelV2) * 0.5f;
    al::setLocalTrans(mWaitEndMapLine, newPosition);
}

void MapLayout::appearParts(bool) {}

void MapLayout::startNumberAction() {
    for (s32 i = 0; i < mMapIconInfoSize; i++) {
        if (!mMapIconInfo[i].isActive)
            continue;
        if (mMapIconInfo[i].action == 1) {
            al::startAction(mMapIconInfo[i].iconLayout->layout, "Off", "OnOff");
        } else {
            al::startAction(mMapIconInfo[i].iconLayout->layout, "On", "OnOff");
            al::WStringTmp<32> iconType = {u"%d", mMapIconInfo[i].action};
            al::setPaneString(mMapIconInfo[i].iconLayout->layout, "TxtNumber", iconType.cstr(), 0);
        }
    }
}

void MapLayout::calcSeaOfTreeIconPos(sead::Vector3f* position) {
    *position = al::getPaneLocalTrans(mMapTerrainLayout, "UnclearPos");
    position->x *= mPanelLocalScale.x;
    position->y *= mPanelLocalScale.y;
    position->x += mScrollPosition.x * mPanelLocalScale.x;
    position->y += mScrollPosition.y * mPanelLocalScale.y;
}

const char* getIconName(const IconType iconType) {
    if (iconType < IconType::MaxValue)
        return sIconType[(s32)iconType];
    return "";
}

void MapLayout::setLocalTransAndAppear(MapIconLayout* iconLayout, MapIconInfo* iconInfo,
                                       const sead::Vector3f& position, IconType iconType,
                                       bool isbool) {
    al::SimpleLayoutAppearWaitEnd* layout = iconLayout->layout;
    if (iconType == 0xe || iconType == 0x5)
        al::startAction(layout, "HintNew", "Main");

    al::setLocalTrans(iconLayout->layout, position);
    al::startAction(iconLayout->layout, getIconName(iconType), "Icon");
    iconInfo[iconLayout->fieldA].isActive = true;
    iconInfo[iconLayout->fieldA].position = sead::Vector3f::zero;
    iconInfo[iconLayout->fieldA].iconType = iconType;
    iconInfo[iconLayout->fieldA].action = 1;
    iconInfo[iconLayout->fieldA].position2 = sead::Vector2f::zero;
    iconInfo[iconLayout->fieldA].value = isbool;
    iconLayout->layout->appear();
}

void MapLayout::calcMapTransAndAppear(MapIconLayout*, MapIconInfo*, const sead::Vector3f&, IconType,
                                      bool) {}

void MapLayout::scroll(const sead::Vector2f& scrollDistance) {
    if (!mIsPrintWorldChanged)
        return;

    f32 scale = scrollDistance.length() / mPanelLocalScale.x;
    mScrollPosition += scrollDistance * scale;

    mScrollPosition.x =
        sead::Mathf::clamp(mScrollPosition.x, mMinScrollPosition.x, mMaxScrollPosition.x);
    mScrollPosition.y =
        sead::Mathf::clamp(mScrollPosition.y, mMinScrollPosition.y, mMaxScrollPosition.y);
}

void MapLayout::addSize(const sead::Vector2f& size) {
    mPanelSize += size;

    f32 prevLocalScaleX = mPanelLocalScale.x;
    f32 maxSize = mMapTerrainLayout->getPaneSize();
    f32 minSize = maxSize * StageMapFunction::getStageMapScaleMin();

    mPanelSize.x = sead::Mathf::clamp(mPanelSize.x, minSize, maxSize);
    mPanelSize.y = sead::Mathf::clamp(mPanelSize.y, minSize, maxSize);
    mPanelLocalScale.x = (StageMapFunction::getStageMapScaleMax() / maxSize) * mPanelSize.x;
    mPanelLocalScale.y = (StageMapFunction::getStageMapScaleMax() / maxSize) * mPanelSize.y;
    mPanelLocalScale.x =
        sead::Mathf::clamp(mPanelLocalScale.x, StageMapFunction::getStageMapScaleMin(),
                           StageMapFunction::getStageMapScaleMax());
    mPanelLocalScale.y =
        sead::Mathf::clamp(mPanelLocalScale.y, StageMapFunction::getStageMapScaleMin(),
                           StageMapFunction::getStageMapScaleMax());

    if (!al::isNearZero(mPanelLocalScale.x - prevLocalScaleX, 0.001f))
        al::holdSeWithParam(this, "Zoom", mPanelLocalScale.x, "スケール");

    setPanelFont(mPanelLocalScale.x, &mCurrentFontType, this, mCurrentFontType);
    updateST();
}

void setPanelFont(f32 scale, s32* newType, al::LayoutActor* layoutActor, s32 currentType) {
    const char* fontName;
    if (0.45f > scale) {
        if (currentType == 2)
            return;
        *newType = 2;
        fontName = "nintendo_udsg-r_std_003_10.fcpx";
    } else if (0.7f > scale) {
        if (currentType == 1)
            return;
        *newType = 1;
        fontName = "nintendo_udsg-r_std_003_40.fcpx";
    } else {
        if (currentType == 0)
            return;
        *newType = 0;
        fontName = "nintendo_udsg-r_std_003_80.fcpx";
    }
    al::setTextBoxPaneFont(layoutActor, "TxtCaption00", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtRegionCaption", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtData01", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtData02", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtTitle01", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtContents01", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtCaption01", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtTitle02", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtContents02", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtCaption02", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtTitle03", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtContents03", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtCaption03", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtTitle04", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtContents04", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtCaption04", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtTitle05", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtContents05", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtCaption05", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtCaption06", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtContents07", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtTitle07", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtElement01", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtElement02", fontName);
    al::setTextBoxPaneFont(layoutActor, "TxtIcon", fontName);
}

bool MapLayout::isAppear() const {
    return al::isNerve(this, &NrvMapLayout.Appear);
}

const sead::Matrix44f& MapLayout::getViewProjMtx() const {
    return mMapTerrainLayout->getMapData()->viewProjMatrix;
}

const sead::Matrix44f& MapLayout::getProjMtx() const {
    return mMapTerrainLayout->getMapData()->projMatrix;
}

void MapLayout::updateIconLine(al::LayoutActor* layoutActor, const sead::Vector3f& vector3,
                               const sead::Vector2f& vector2) {
    MapData* mapData = mMapTerrainLayout->getMapData();
    f32 scale = mPanelLocalScale.x;

    sead::Vector2f panelV = {
        scale *
                (mapData->viewProjMatrix(3, 0) + vector3.x * mapData->viewProjMatrix(0, 0) +
                 vector3.y * mapData->viewProjMatrix(1, 0) +
                 vector3.z * mapData->viewProjMatrix(2, 0)) *
                1024.0f +
            scale * mScrollPosition.x,

        scale *
                (mapData->viewProjMatrix(3, 1) + vector3.x * mapData->viewProjMatrix(0, 1) +
                 vector3.y * mapData->viewProjMatrix(1, 1) +
                 vector3.z * mapData->viewProjMatrix(2, 1)) *
                1024.0f +
            scale * mScrollPosition.y};

    sead::Vector2f panelV2 = {mPanelLocalScale.x * vector2.x + panelV.x,
                              mPanelLocalScale.x * vector2.y + panelV.y};

    f32 angle = al::calcAngleDegree((panelV - panelV2), sead::Vector2f::ex);
    angle = (360.0f - modDegree(angle)) * (3.2f / 9.0f);
    al::startFreezeAction(layoutActor, "State", angle, "Direct");

    f32 size = (panelV - panelV2).length() / 20.0f;
    al::startFreezeAction(layoutActor, "State", size, "Length");

    sead::Vector3f newPosition = {(panelV.x + panelV2.x) * 0.5f, (panelV.y + panelV2.y) * 0.5f,
                                  0.0f};
    al::setLocalTrans(layoutActor, newPosition);
}

void MapLayout::focusIcon(const MapIconInfo* mapIconInfo) {
    al::startAction(mapIconInfo->iconLayout->layout, "Select", "Main");
    al::startAction(mWaitEndMapCursor, "Select", nullptr);

    switch (mapIconInfo->iconType) {
    case IconType::Flag: {
        const char* checkpointObjId =
            GameDataFunction::getCheckpointObjIdInWorld(this, mapIconInfo->iconLayout->fieldB);
        const char* subStrA = al::searchSubString(checkpointObjId, "(");
        const char* subStrB = al::searchSubString(checkpointObjId, "[");
        const char* mainStageName;
        const char* clabel;
        if (subStrB == nullptr) {
            clabel =
                al::StringTmp<128>("%s_%s", rs::getCheckpointLabelPrefix(), checkpointObjId).cstr();
            mainStageName = GameDataFunction::tryGetCurrentMainStageName(this);
        } else {
            char* objName;
            s32 size = subStrB - checkpointObjId;
            al::extractString(objName, checkpointObjId, size, 0x80);
            objName[size] = '\0';

            char* stageName;
            u32 size2 = subStrA - (subStrB + 1);
            al::extractString(stageName, subStrB + 1, size2, 0x80);
            stageName[size2] = '\0';

            clabel = al::StringTmp<128>("%s_%s", rs::getCheckpointLabelPrefix(), objName).cstr();
            mainStageName = stageName;
        }
        rs::trySetPaneStageMessageIfExist(mWaitEndMapGuide, "TxtFlagName", clabel, mainStageName);
        al::setPaneSystemMessage(mWaitEndMapGuide, "TxtGuide", "StageMap", "CheckpointGuide");
        mWaitEndMapGuide->appear();
        al::startAction(mWaitEndMapGuide, "On", "OnOff");
        break;
    }
    default:
        break;
    case IconType::FlagDisable:
        rs::trySetPaneSystemMessageIfExist(mWaitEndMapGuide, "TxtFlagName", "GlossaryObject",
                                           "Home");
        al::setPaneSystemMessage(mWaitEndMapGuide, "TxtGuide", "StageMap", "CheckpointGuide");
        mWaitEndMapGuide->appear();
        al::startAction(mWaitEndMapGuide, "On", "OnOff");
        break;

    case IconType::ShopSold:
        if (al::isEqualSubString("ShopSoldout", "TitleYukimaruRace")) {
            rs::trySetPaneSystemMessageIfExist(mWaitEndMapGuide, "TxtFlagName", "StageMap",
                                               "TitleYukimaruRace");
        } else {
            rs::trySetPaneSystemMessageIfExist(mWaitEndMapGuide, "TxtFlagName", "StageMap",
                                               "ShopSoldout");
        }
        mWaitEndMapGuide->appear();
        al::startAction(mWaitEndMapGuide, "Off", "OnOff");
        break;
    case IconType::HintRock2:
        if (al::isEqualSubString("Cap", "TitleYukimaruRace")) {
            rs::trySetPaneSystemMessageIfExist(mWaitEndMapGuide, "TxtFlagName", "StageMap",
                                               "TitleYukimaruRace");
        } else {
            rs::trySetPaneSystemMessageIfExist(mWaitEndMapGuide, "TxtFlagName", "StageMap", "Cap");
        }
        mWaitEndMapGuide->appear();
        al::startAction(mWaitEndMapGuide, "Off", "OnOff");
        break;
    case IconType::Luigi:
        if (al::isEqualSubString("TimeBalloon", "TitleYukimaruRace")) {
            rs::trySetPaneSystemMessageIfExist(mWaitEndMapGuide, "TxtFlagName", "StageMap",
                                               "TitleYukimaruRace");
        } else {
            rs::trySetPaneSystemMessageIfExist(mWaitEndMapGuide, "TxtFlagName", "StageMap",
                                               "TimeBalloon");
        }
        mWaitEndMapGuide->appear();
        al::startAction(mWaitEndMapGuide, "Off", "OnOff");
        break;
    case IconType::Poet:
        if (al::isEqualSubString("Poetter", "TitleYukimaruRace")) {
            rs::trySetPaneSystemMessageIfExist(mWaitEndMapGuide, "TxtFlagName", "StageMap",
                                               "TitleYukimaruRace");
        } else {
            rs::trySetPaneSystemMessageIfExist(mWaitEndMapGuide, "TxtFlagName", "StageMap",
                                               "Poetter");
        }
        mWaitEndMapGuide->appear();
        al::startAction(mWaitEndMapGuide, "Off", "OnOff");
        break;
    case IconType::Scenario2:
        if (al::isEqualSubString("MoonRock", "TitleYukimaruRace")) {
            rs::trySetPaneSystemMessageIfExist(mWaitEndMapGuide, "TxtFlagName", "StageMap",
                                               "TitleYukimaruRace");
        } else {
            rs::trySetPaneSystemMessageIfExist(mWaitEndMapGuide, "TxtFlagName", "StageMap",
                                               "MoonRock");
        }
        mWaitEndMapGuide->appear();
        al::startAction(mWaitEndMapGuide, "Off", "OnOff");
        break;
    }
}

void MapLayout::lostFocusIcon(MapIconLayout* mapIconLayout) {
    al::startAction(mWaitEndMapCursor, "Wait", nullptr);
    al::startAction(mapIconLayout->layout, "Wait", "Main");
    al::killLayoutIfActive(mWaitEndMapGuide);
    al::startAction(mWaitEndMapGuide, "End", nullptr);
}

bool MapLayout::tryCalcNorthDir(sead::Vector3f* northDir) {
    MapData* mapData = mMapTerrainLayout->getMapData();
    if (mapData == nullptr)
        return false;

    const sead::Vector3f ey = sead::Vector3f::ey;
    northDir->x = mapData->viewMatrix(0, 0) * ey.x + mapData->viewMatrix(1, 0) * ey.y +
                  mapData->viewMatrix(2, 0) * ey.z;
    northDir->y = mapData->viewMatrix(0, 1) * ey.x + mapData->viewMatrix(1, 1) * ey.y +
                  mapData->viewMatrix(2, 1) * ey.z;
    northDir->z = mapData->viewMatrix(0, 2) * ey.x + mapData->viewMatrix(1, 2) * ey.y +
                  mapData->viewMatrix(2, 2) * ey.z;
    al::normalize(northDir);
    return true;
}

void MapLayout::exeAppear() {
    if (al::isFirstStep(this)) {
        sead::FixedSafeString<0x100> stringA;
        sead::FixedSafeString<0x100> stringB;
        bool isB = false;

        al::startAction(mWaitEndMapCursor, "On", "Scenario");
        if (GameDataFunction::isTimeBalloonSequence(this)) {
            al::startAction(mWaitEndMapCursor, "Off", "Scenario");
        } else if (GameDataFunction::isRaceStart(this)) {
            al::startAction(mWaitEndMapCursor, "Off", "Scenario");
        } else if (GameDataFunction::isUnlockedCurrentWorld(this) &&
                   rs::tryGetMapMainScenarioLabel(&stringA, &stringB, &isB, this)) {
            if (isB) {
                rs::trySetPaneStageMessageIfExist(mWaitEndMapCursor, "TxtScenario", stringB.cstr(),
                                                  stringB.cstr());
            } else {
                rs::trySetPaneSystemMessageIfExist(mWaitEndMapCursor, "TxtScenario", stringA.cstr(),
                                                   stringA.cstr());
            }
        }
        al::startAction(mWaitEndMapCursor, "Off", "Scenario");
        if (isAppear())
            mWaitEndMapCursor->appear();
        al::startHitReaction(this, "マップオープン", nullptr);
    }

    /*if (al::isGreaterEqualStep(this,4)&& !field_0x296 &&
       (this_00 = *(ScalableFontMgr **)(*(long *)(layoutActor).sceneInfo + 0x58),
       this_00[0x55] == (ScalableFontMgr)0x0)) {
      GameDataFunction::getWorldDevelopName(this,mWorldId;);
      eui::ScalableFontMgr::getFont(this_00,"nintendo_udsg-r_std_003_10.fcpx");
      pSVar6 = eui::ScalableFontMgr::getFont(this_00,"nintendo_udsg-r_std_003_40.fcpx");
      pSVar7 = eui::ScalableFontMgr::getFont(this_00,"nintendo_udsg-r_std_003_80.fcpx");
      lVar8 = (**(code **)((layoutActor).vtable + 0x48))(this);
      plVar14 = *(long **)(*(long *)(lVar8 + 0x10) + 0x18);
      lVar8 = 0;
      do {
        pPVar9 = (Pane *)(**(code **)(*plVar14 + 0x60))
                                   (plVar14,*(undefined8 *)
                                             ((long)&PTR_s_TxtCaption00_7101cc8648 + lVar8),1);
        pTVar10 = nn::ui2d::DynamicCast<>(pPVar9);
        eui::ScalableFontMgr::registerGlyphs
                  (this_00,*(wchar16 **)(pTVar10 + 0xe0),(uint)*(ushort *)(pTVar10 + 0x11a),pSVar6,
                   *(FontMgr **)(layoutActor).sceneInfo,-1);
        eui::ScalableFontMgr::registerGlyphs
                  (this_00,*(wchar16 **)(pTVar10 + 0xe0),(uint)*(ushort *)(pTVar10 + 0x11a),pSVar7,
                   *(FontMgr **)(layoutActor).sceneInfo,-1);
        lVar8 = lVar8 + 8;
      } while (lVar8 != 0xa8);
      pPVar9 = (Pane *)(**(code **)(*plVar14 + 0x60))(plVar14,"TxtTitle07",1);
      pTVar10 = nn::ui2d::DynamicCast<>(pPVar9);
      eui::ScalableFontMgr::registerGlyphs
                (this_00,*(wchar16 **)(pTVar10 + 0xe0),(uint)*(ushort *)(pTVar10 + 0x11a),pSVar6,
                 *(FontMgr **)(layoutActor).sceneInfo,-1);
      eui::ScalableFontMgr::registerGlyphs
                (this_00,*(wchar16 **)(pTVar10 + 0xe0),(uint)*(ushort *)(pTVar10 + 0x11a),pSVar7,
                 *(FontMgr **)(layoutActor).sceneInfo,-1);
      pPVar9 = (Pane *)(**(code **)(*plVar14 + 0x60))(plVar14,"TxtElement01",1);
      pTVar10 = nn::ui2d::DynamicCast<>(pPVar9);
      eui::ScalableFontMgr::registerGlyphs
                (this_00,*(wchar16 **)(pTVar10 + 0xe0),(uint)*(ushort *)(pTVar10 + 0x11a),pSVar6,
                 *(FontMgr **)(layoutActor).sceneInfo,-1);
      eui::ScalableFontMgr::registerGlyphs
                (this_00,*(wchar16 **)(pTVar10 + 0xe0),(uint)*(ushort *)(pTVar10 + 0x11a),pSVar7,
                 *(FontMgr **)(layoutActor).sceneInfo,-1);
      pPVar9 = (Pane *)(**(code **)(*plVar14 + 0x60))(plVar14,"TxtElement02",1);
      pTVar10 = nn::ui2d::DynamicCast<>(pPVar9);
      eui::ScalableFontMgr::registerGlyphs
                (this_00,*(wchar16 **)(pTVar10 + 0xe0),(uint)*(ushort *)(pTVar10 + 0x11a),pSVar6,
                 *(FontMgr **)(layoutActor).sceneInfo,-1);
      eui::ScalableFontMgr::registerGlyphs
                (this_00,*(wchar16 **)(pTVar10 + 0xe0),(uint)*(ushort *)(pTVar10 + 0x11a),pSVar7,
                 *(FontMgr **)(layoutActor).sceneInfo,-1);
      pPVar9 = (Pane *)(**(code **)(*plVar14 + 0x60))(plVar14,"TxtIcon",1);
      pTVar10 = nn::ui2d::DynamicCast<>(pPVar9);
      eui::ScalableFontMgr::registerGlyphs
                (this_00,*(wchar16 **)(pTVar10 + 0xe0),(uint)*(ushort *)(pTVar10 + 0x11a),pSVar6,
                 *(FontMgr **)(layoutActor).sceneInfo,-1);
      eui::ScalableFontMgr::registerGlyphs
                (this_00,*(wchar16 **)(pTVar10 + 0xe0),(uint)*(ushort *)(pTVar10 + 0x11a),pSVar7,
                 *(FontMgr **)(layoutActor).sceneInfo,-1);
      field_0x296 = 1;
    }*/
    if (al::isActionEnd(this, nullptr)) {
        if (isAppear() || al::isNerve(this, &NrvMapLayout.ChangeIn)) {
            al::setNerve(this, &NrvMapLayout.Wait);
            appearParts(true);
            startNumberAction();
            return;
        }
        if (al::isNerve(this, &NrvMapLayout.AppearWithHint)) {
            al::setNerve(this, &NrvMapLayout.HintInitWaitNpc);
            appearParts(true);
            startNumberAction();
            return;
        }
        if (al::isNerve(this, &NrvMapLayout.AppearMoonRockDemo)) {
            al::setNerve(this, &NrvMapLayout.HintInitWaitMoonRock);
            appearParts(false);
        }
        startNumberAction();
    }
}

void MapLayout::exeWait() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "Wait", nullptr);
        s32 size = mArray.size();
        for (s32 i = 0; i < size; i++)
            if (al::isActive(mArray[i]))
                al::startAction(mArray[i], "Wait", nullptr);
    }
}

void MapLayout::exeHintInitWait() {
    if (al::isFirstStep(this))
        al::showPaneRoot(this);

    if (al::isGreaterEqualStep(this, 30)) {
        if (al::isNerve(this, &NrvMapLayout.HintInitWaitNpc))
            al::setNerve(this, &NrvMapLayout.HintAppearNpc);
        else if (al::isNerve(this, &NrvMapLayout.HintInitWaitMoonRock))
            al::setNerve(this, &NrvMapLayout.HintAppearMoonRock);
        else if (al::isNerve(this, &NrvMapLayout.HintInitWaitAmiibo))
            al::setNerve(this, &NrvMapLayout.HintAppearAmiibo);
    }
}

void MapLayout::exeHintAppear() {
    if (al::isFirstStep(this)) {
        if (al::isNerve(this, &NrvMapLayout.HintAppearNpc)) {
            s32 hintNum = GameDataFunction::calcHintNum(this);
            if (!GameDataFunction::checkLatestHintSeaOfTree(this)) {
                calcMapTransAndAppear(&mHintDecideIconLayout[hintNum - 1], mMapIconInfo,
                                      GameDataFunction::getLatestHintTrans(this), IconType::Hint1,
                                      false);
                al::startAction(mHintDecideIconLayout[hintNum - 1].layout, "HintNew", nullptr);
            } else {
                sead::Vector3f position;
                calcSeaOfTreeIconPos(&position);
                setLocalTransAndAppear(&mHintDecideIconLayout[hintNum - 1], mMapIconInfo, position,
                                       IconType::Hint1, true);
                al::startAction(mHintDecideIconLayout[hintNum - 1].layout, "HintNew", nullptr);
            }
        } else if (al::isNerve(this, &NrvMapLayout.HintAppearAmiibo)) {
            s32 hintNum = GameDataFunction::calcHintNum(this);
            MapIconLayout* iconLayout = nullptr;
            for (s32 i = 0; i < mHintDecideIconAmiiboSize; i++) {
                HintAmiibo& hintamiibo = getHintAmiibo(i, mHintDecideIconAmiiboSize, mHintAmiibo);
                if (!hintamiibo.isValid) {
                    for (s32 e = 0; e < i; e++) {
                        sead::Vector3f diff = mMapIconInfo[e].position - hintamiibo.position;
                        if (diff.length() < 10.0f) {
                            mMapIconInfo[e].action++;
                            goto skip2;
                        }
                    }
                    calcMapTransAndAppear(&mHintDecideIconLayout[i], mMapIconInfo,
                                          hintamiibo.position, IconType::Hint1, false);
                    al::startAction(mHintDecideIconLayout[i].layout, "HintNew", nullptr);
                } else if (iconLayout == nullptr) {
                    sead::Vector3f position = sead::Vector3f::zero;
                    calcSeaOfTreeIconPos(&position);
                    iconLayout = &mHintDecideIconLayout[i];
                    setLocalTransAndAppear(iconLayout, mMapIconInfo, position, IconType::Hint1,
                                           true);
                } else {
                    mMapIconInfo[iconLayout->fieldA].action++;
                }
            skip2:
                continue;
            }

        } else if (al::isNerve(this, &NrvMapLayout.HintAppearMoonRock)) {
            s32 rockNum = GameDataFunction::calcHintMoonRockNum(this);
            for (s32 i = 0; i < rockNum; i++) {
                sead::Vector3f rockPosition = GameDataFunction::calcHintMoonRockTrans(this, i);
                for (s32 e = 0; e < i; e++) {
                    sead::Vector3f diff = mMapIconInfo[e].position - rockPosition;
                    if (diff.length() < 10.0f) {
                        mMapIconInfo[e].action++;
                        goto skip;
                    }
                }
                calcMapTransAndAppear(&mMoonRockLayout[i], mMapIconInfo, rockPosition,
                                      IconType::HintRock1, false);
                al::startAction(mMoonRockLayout[i].layout, "HintNew", nullptr);
            skip:
                continue;
            }
        }
        startNumberAction();
    }
    if (al::isGreaterEqualStep(this, 150)) {
        if (al::isNerve(this, &NrvMapLayout.HintAppearNpc))
            al::setNerve(this, &NrvMapLayout.HintDecideIconAppearNpc);
        else if (al::isNerve(this, &NrvMapLayout.HintAppearAmiibo))
            al::setNerve(this, &NrvMapLayout.HintDecideIconAppearAmiibo);
        else if (al::isNerve(this, &NrvMapLayout.HintAppearMoonRock))
            al::setNerve(this, &NrvMapLayout.HintDecideIconAppearMoonRock);
    }
}

void MapLayout::exeHintDecideIconAppear() {
    if (al::isFirstStep(this)) {
        mDecideIconLayout->appear();
        if (al::isNerve(this, &NrvMapLayout.HintDecideIconAppearMoonRock)) {
            s32 moonRockNum = GameDataFunction::calcHintMoonRockNum(this);
            for (s32 i = 0; i < moonRockNum; i++) {
                al::startAction(mMoonRockLayout[i].layout, "Wait", nullptr);
                startNumberAction();
            }
        } else if (al::isNerve(this, &NrvMapLayout.HintDecideIconAppearNpc)) {
            al::startAction(mHintDecideIconLayout[GameDataFunction::calcHintNum(this) - 1].layout,
                            "Wait", nullptr);
            startNumberAction();
        } else if (al::isNerve(this, &NrvMapLayout.HintDecideIconAppearAmiibo)) {
            s32 hintNum = GameDataFunction::calcHintNum(this);
            for (s32 i = 0; i < mHintDecideIconAmiiboSize; i++) {
                al::startAction(mHintDecideIconLayout[hintNum - (i + 1)].layout, "Wait", nullptr);
                startNumberAction();
            }
            mHintDecideIconAmiiboSize = 0;
        }
    }
    mDecideIconLayout->updateNerve();
    if (mDecideIconLayout->isWait())
        al::setNerve(this, &NrvMapLayout.HintDecideIconWait);
}

void MapLayout::exeHintDecideIconWait() {
    mDecideIconLayout->updateNerve();
    if (mDecideIconLayout->isDecide())
        al::setNerve(this, &HintPressDecide);
}

void MapLayout::exeHintPressDecide() {
    mDecideIconLayout->updateNerve();
    if (mDecideIconLayout->isEnd())
        end();
}

void MapLayout::exeEnd() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "End", nullptr);
        al::startAction(mWaitEndMapPlayer, "End", nullptr);
        al::startAction(mWaitEndMapCursor, "End", nullptr);
        al::startAction(mWaitEndMapGuide, "End", nullptr);

        if (al::isActive(mWaitEndMapLine))
            al::startAction(mWaitEndMapLine, "End", nullptr);

        s32 size = mArray.size();
        for (s32 i = 0; i < size; i++)
            al::startAction(mArray[i], "End", nullptr);
        for (s32 i = 0; i < mMapIconInfoSize; i++)
            if (mMapIconInfo[i].isActive &&
                al::killLayoutIfActive(mMapIconInfo[i].iconLayout->layout))
                mMapIconInfo[i].isActive = false;
        al::startHitReaction(this, "マップクローズ", nullptr);
    }

    if (al::isActionEnd(this, nullptr)) {
        s32 size = mArray.size();
        for (s32 i = 0; i < size; i++)
            mArray[i]->kill();
        mWaitEndMapPlayer->kill();
        mWaitEndMapCursor->kill();
        mWaitEndMapGuide->kill();
        mWaitEndMapLine->kill();
        if (mIsHelp)
            mWaitEndMapBg->kill();
        mIsHelp = true;
        kill();
    }
}

void MapLayout::exeChangeOut() {
    if (al::isFirstStep(this)) {
        for (s32 i = 0; i < mMapIconInfoSize; i++)
            if (mMapIconInfo[i].isActive &&
                al::killLayoutIfActive(mMapIconInfo[i].iconLayout->layout))
                mMapIconInfo[i].isActive = false;
    }

    if (al::isActionEnd(this, "Change")) {
        mWaitEndMapPlayer->kill();
        mWaitEndMapCursor->kill();
        mWaitEndMapGuide->kill();
        mWaitEndMapLine->kill();
        if (mIsHelp)
            mWaitEndMapBg->kill();
        mIsHelp = true;
        kill();
    }
}

namespace rs {
void calcTransOnMap(sead::Vector2f* out, const sead::Vector3f& v1, const sead::Matrix44f& m,
                    const sead::Vector2f& v2, f32 f1, f32 f2) {
    const sead::Vector3f tmp = v1;

    f64 d_m00 = m.m[0][0];
    f64 d_m01 = m.m[0][1];
    f64 d_m02 = m.m[0][2];
    f64 d_m03 = m.m[0][3];

    f64 d_m10 = m.m[1][0];
    f64 d_m11 = m.m[1][1];
    f64 d_m12 = m.m[1][2];
    f64 d_m13 = m.m[1][3];

    f64 d_v1x = (f64)tmp.x;
    f64 d_v1y = (f64)tmp.y;
    f64 d_v1z = (f64)tmp.z;

    f64 accx = (d_v1x * d_m00 + d_v1y * d_m01 + d_v1z * d_m02) + d_m03;
    f64 accy = (d_v1x * d_m10 + d_v1y * d_m11 + d_v1z * d_m12) + d_m13;
    sead::Vector2<f64> scaled = {accx, accy};
    scaled *= f2 * 0.5;
    scaled *= f1;

    f64 d_v2x = v2.x * f1 + scaled.x;
    f64 d_v2y = __builtin_fma(scaled.y, 1.0, v2.y * f1);

    out->set(d_v2x, d_v2y);
}

bool tryCalcMapNorthDir(sead::Vector3f* direction, const al::IUseSceneObjHolder* objHolder) {
    MapLayout* mapLayout = al::getSceneObj<MapLayout>(objHolder);
    return mapLayout->tryCalcNorthDir(direction);
}

const sead::Matrix44f& getMapViewProjMtx(const al::IUseSceneObjHolder* objHolder) {
    MapLayout* mapLayout = al::getSceneObj<MapLayout>(objHolder);
    return mapLayout->getViewProjMtx();
}

const sead::Matrix44f& getMapProjMtx(const al::IUseSceneObjHolder* objHolder) {
    MapLayout* mapLayout = al::getSceneObj<MapLayout>(objHolder);
    return mapLayout->getProjMtx();
}

void appearMapWithHint(const al::IUseSceneObjHolder* objHolder) {
    MapLayout* mapLayout = al::getSceneObj<MapLayout>(objHolder);
    mapLayout->appearWithHint();
}

void addAmiiboHintToMap(const al::IUseSceneObjHolder* objHolder) {
    MapLayout* mapLayout = al::getSceneObj<MapLayout>(objHolder);
    mapLayout->addAmiiboHint();
}

void appearMapWithAmiiboHint(const al::IUseSceneObjHolder* objHolder) {
    MapLayout* mapLayout = al::getSceneObj<MapLayout>(objHolder);
    mapLayout->appearAmiiboHint();
}

void appearMapMoonRockDemo(const al::IUseSceneObjHolder* objHolder, s32 value) {
    MapLayout* mapLayout = al::getSceneObj<MapLayout>(objHolder);
    mapLayout->appearMoonRockDemo(value);
}

void endMap(const al::IUseSceneObjHolder* objHolder) {
    MapLayout* mapLayout = al::getSceneObj<MapLayout>(objHolder);
    mapLayout->end();
}

bool isEndMap(const al::IUseSceneObjHolder* objHolder) {
    MapLayout* mapLayout = al::getSceneObj<MapLayout>(objHolder);
    return mapLayout->isEnd();
}

bool isEnableCheckpointWarp(const al::IUseSceneObjHolder* objHolder) {
    MapLayout* mapLayout = al::getSceneObj<MapLayout>(objHolder);
    return mapLayout->isEnableCheckpointWarp();
}
}  // namespace rs

namespace StageMapFunction {
f32 getStageMapScaleMin() {
    return 0.3f;
}

f32 getStageMapScaleMax() {
    return 1.0f;
}
}  // namespace StageMapFunction

#include "Layout/MapLayout.h"

#include "Library/Base/StringUtil.h"
#include "Library/Layout/LayoutActionFunction.h"
#include "Library/Layout/LayoutActorUtil.h"
#include "Library/Layout/LayoutInitInfo.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Matrix/MatrixUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Play/Layout/SimpleLayoutAppearWaitEnd.h"
#include "Library/Player/PlayerHolder.h"
#include "Library/Player/PlayerUtil.h"
#include "Library/Se/SeFunction.h"

#include "Layout/DecideIconLayout.h"
#include "Layout/MapTerrainLayout.h"
#include "Sequence/GameSequenceInfo.h"
#include "System/GameDataFunction.h"
#include "System/MapDataHolder.h"
#include "Util/PlayerUtil.h"
#include "Util/StageLayoutFunction.h"

namespace {
NERVE_IMPL(MapLayout, Appear)
NERVE_IMPL_(MapLayout, AppearWithHint, Appear)
NERVE_IMPL_(MapLayout, AppearMoonRockDemo, Appear)
NERVE_IMPL_(MapLayout, HintAppearNpc, Appear)
NERVE_IMPL_(MapLayout, HintAppearMoonRock, Appear)
NERVE_IMPL_(MapLayout, HintAppearAmiibo, Appear)
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
                   HintDecideIconAppearMoonRock, HintDecideIconAppearAmiibo, HintDecideIconWait)
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

inline f32 modDegree(f32 value) {
    return al::modf(value + 360.0f, 360.0f) + 0.0f;
}

MapLayout::MapLayout(const al::LayoutInitInfo&, const al::PlayerHolder*, s32)
    : al::LayoutActor("マップ") {}

void MapLayout::appear() {}

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
    isPrintWorldChanged = mMapTerrainLayout->tryChangePrintWorld(worldId);
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
    PanelSize.x = map * mPanelLocalScale.x;
    PanelSize.y = map * mPanelLocalScale.y;
    mScrollPosition = sead::Vector2f::zero;
}

void MapLayout::moveFocusLayout(const sead::Vector3f&, const sead::Vector2f&) {}

void MapLayout::updateST() {
    std::cos(0.0f);
}

void MapLayout::addAmiiboHint() {}

void MapLayout::appearAmiiboHint() {}

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

void MapLayout::updatePlayerPosLayout() {}

void MapLayout::appearWithHint() {}

void MapLayout::appearMoonRockDemo(int) {}

void MapLayout::appearCollectionList() {
    this->help = false;
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

void MapLayout::changeOut(bool) {}

void MapLayout::changeIn(bool isRightIn) {
    appearCollectionList();
    al::startAction(this, "Wait", nullptr);
    mWaitEndMapCursor->appear();
    help = false;
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

void MapLayout::startNumberAction() {}

void MapLayout::calcSeaOfTreeIconPos(sead::Vector3f* position) {
    *position = al::getPaneLocalTrans(mMapTerrainLayout, "UnclearPos");
    position->x *= mPanelLocalScale.x;
    position->y *= mPanelLocalScale.y;
    position->x += mScrollPosition.x * mPanelLocalScale.x;
    position->y += mScrollPosition.y * mPanelLocalScale.y;
}

void MapLayout::setLocalTransAndAppear(MapIconLayout*, MapIconInfo*, const sead::Vector3f&,
                                       IconType, bool) {}

void MapLayout::calcMapTransAndAppear(MapIconLayout*, MapIconInfo*, const sead::Vector3f&, IconType,
                                      bool) {}

void MapLayout::scroll(const sead::Vector2f& scrollDistance) {
    if (!this->isPrintWorldChanged)
        return;

    f32 scale = scrollDistance.length() / mPanelLocalScale.x;
    mScrollPosition += scrollDistance * scale;

    mScrollPosition.x =
        sead::Mathf::clamp(mScrollPosition.x, minScrollPosition.x, maxScrollPosition.x);
    mScrollPosition.y =
        sead::Mathf::clamp(mScrollPosition.y, minScrollPosition.y, maxScrollPosition.y);
}

void MapLayout::addSize(const sead::Vector2f& size) {
    PanelSize += size;

    f32 prevLocalScaleX = mPanelLocalScale.x;
    f32 maxSize = mMapTerrainLayout->getPaneSize();
    f32 minSize = maxSize * 0.3f;

    PanelSize.x = sead::Mathf::clamp(PanelSize.x, minSize, maxSize);
    PanelSize.y = sead::Mathf::clamp(PanelSize.y, minSize, maxSize);
    mPanelLocalScale.x = (1.0f / maxSize) * PanelSize.x;
    mPanelLocalScale.y = (1.0f / maxSize) * PanelSize.y;
    mPanelLocalScale.x = sead::Mathf::clamp(mPanelLocalScale.x, 0.3f, 1.0f);
    mPanelLocalScale.y = sead::Mathf::clamp(mPanelLocalScale.y, 0.3f, 1.0f);

    if (!al::isNearZero(mPanelLocalScale.x - prevLocalScaleX, 0.001f))
        al::holdSeWithParam(this, "Zoom", mPanelLocalScale.x, "スケール");

    setPanelFont(mPanelLocalScale.x, &currentFontType, this, currentFontType);
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

void MapLayout::focusIcon(const MapIconInfo*) {}

void MapLayout::lostFocusIcon(MapIconLayout* mapIconLayout) {
    al::startAction(mWaitEndMapCursor, "Wait", nullptr);
    al::startAction(mapIconLayout->layout, "Wait", "Main");
    al::killLayoutIfActive(mWaitEndMapGuide);
    al::startAction(mWaitEndMapGuide, "End", nullptr);
}

bool MapLayout::tryCalcNorthDir(sead::Vector3f* northDir) {
    MapData* mapData = mMapTerrainLayout->getMapData();
    if (mapData != nullptr) {
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
    return false;
}

void MapLayout::exeAppear() {}

void MapLayout::exeWait() {}

void MapLayout::exeHintInitWait() {}

void MapLayout::exeHintAppear() {}

void MapLayout::exeHintDecideIconAppear() {}

void MapLayout::exeHintDecideIconWait() {
    mDecideIconLayout->updateNerve();
    if (mDecideIconLayout->isDecide())
        al::setNerve(this, &HintPressDecide);
}

void MapLayout::exeHintPressDecide() {
    mDecideIconLayout->updateNerve();
    if (mDecideIconLayout->isEnd())
        al::setNerve(this, &NrvMapLayout.End);
}

void MapLayout::exeEnd() {}

void MapLayout::exeChangeOut() {}

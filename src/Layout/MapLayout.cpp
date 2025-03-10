#include "Layout/MapLayout.h"

#include "Library/Layout/LayoutActionFunction.h"
#include "Library/Layout/LayoutActorUtil.h"
#include "Library/Layout/LayoutInitInfo.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Play/Layout/SimpleLayoutAppearWaitEnd.h"
#include "Library/Player/PlayerHolder.h"
#include "Library/Layout/LayoutActorUtil.h"
#include "Library/Player/PlayerUtil.h"
#include "Library/Se/SeFunction.h"

#include "Layout/DecideIconLayout.h"
#include "Layout/MapTerrainLayout.h"
#include "Sequence/GameSequenceInfo.h"
#include "System/GameDataFunction.h"
#include "System/MapDataHolder.h"
#include "Util/PlayerUtil.h"

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

MapLayout::MapLayout(const al::LayoutInitInfo&, const al::PlayerHolder*, s32)
    : al::LayoutActor("マップ") {}

void MapLayout::appear() {}

void MapLayout::control() {
    if (al::isActive(mWaitEndMapLine))
        updateLine(nullptr);
}

const char* MapLayout::getSceneObjName() const {
    return "マップレイアウト";
}

void MapLayout::changePrintWorld(s32 worldId) {
    isPrintWorldChanged = mMapTerrainLayout->tryChangePrintWorld(worldId);
    mWorldId = worldId;
    reset();
}

void MapLayout::loadTexture() {}

void MapLayout::reset() {
    mPanelLocalScale.y = 0.4f;
    mPanelLocalScale.x = 0.4f;
    f32 map = mMapTerrainLayout->getPaneSize();
    PanelSize.x = map * mPanelLocalScale.x;
    PanelSize.y = map * mPanelLocalScale.y;
    panele = sead::Vector2f::zero;
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

void MapLayout::updateLine(al::LayoutActor* layoutActor) {}

void MapLayout::appearParts(bool) {}

void MapLayout::startNumberAction() {}

void MapLayout::calcSeaOfTreeIconPos(sead::Vector3f*) {}

void MapLayout::setLocalTransAndAppear(MapIconLayout*, MapIconInfo*, const sead::Vector3f&,
                                       IconType, bool) {}

void MapLayout::calcMapTransAndAppear(MapIconLayout*, MapIconInfo*, const sead::Vector3f&, IconType,
                                      bool) {}

void MapLayout::scroll(const sead::Vector2f&) {}

void MapLayout::addSize(const sead::Vector2f& size) {
  PanelSize += size;
  
  f32 prevLocalScaleX=mPanelLocalScale.x;
  f32 maxSize = mMapTerrainLayout->getPaneSize();
  f32 minSize = maxSize * 0.3f;
  
  PanelSize.x= sead::Mathf::clamp(PanelSize.x,minSize,maxSize);
  PanelSize.y = sead::Mathf::clamp(PanelSize.y,minSize,maxSize);
  mPanelLocalScale.x = (1.0f / maxSize) * PanelSize.x;
  mPanelLocalScale.y = (1.0f / maxSize) * PanelSize.y;
  mPanelLocalScale.x =sead::Mathf::clamp(mPanelLocalScale.x, 0.3f, 1.0f);
  mPanelLocalScale.y =sead::Mathf::clamp(mPanelLocalScale.y, 0.3f, 1.0f);
  
  if (!al::isNearZero(mPanelLocalScale.x - prevLocalScaleX,0.001f)) {
    al::holdSeWithParam(this,"Zoom",mPanelLocalScale.x,"スケール");
  }
  
  setPanelFont(mPanelLocalScale.x,&currentFontType,this,currentFontType);
  updateST();
}

void setPanelFont(f32 fontSize, s32* newType, al::LayoutActor* layoutActor, s32 currentType) {
    const char* fontName;
    if (0.45f > fontSize) {
        if (currentType == 2)
            return;
        *newType = 2;
        fontName = "nintendo_udsg-r_std_003_10.fcpx";
    } else if (0.7f > fontSize) {
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

void MapLayout::updateIconLine(al::LayoutActor*, const sead::Vector3f&, const sead::Vector2f&) {}

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

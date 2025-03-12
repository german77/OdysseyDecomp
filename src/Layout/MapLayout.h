#pragma once

#include <math/seadMatrix.h>
#include <math/seadVector.h>

#include "Library/Layout/LayoutActor.h"
#include "Library/Scene/ISceneObj.h"

namespace al {
class IUseSceneObjHolder;
class LayoutInitInfo;
class LiveActor;
class PlayerHolder;
class SimpleLayoutAppearWaitEnd;
}  // namespace al

class MapIconInfo;
class DecideIconLayout;
class MapTerrainLayout;
class TalkMessage;
enum IconType : u32;

struct MapIconLayout {
    void* filler;
    al::SimpleLayoutAppearWaitEnd* layout = nullptr;
};

class MapLayout : public al::LayoutActor, public al::ISceneObj {
public:
    MapLayout(const al::LayoutInitInfo&, const al::PlayerHolder*, s32);

    void appear() override;
    void control() override;
    const char* getSceneObjName() const override;

    void changePrintWorld(s32 worldId);
    void loadTexture();
    void reset();
    void moveFocusLayout(const sead::Vector3f&, const sead::Vector2f&);
    void updateST();
    void addAmiiboHint();
    void appearAmiiboHint();
    void end();
    // sub_71001F1B04
    void updatePlayerPosLayout();
    void appearWithHint();
    void appearMoonRockDemo(int);
    void appearCollectionList();
    bool isEnd() const;
    bool isEnableCheckpointWarp() const;
    void changeOut(bool);
    void changeIn(bool isRightIn);
    void updateLine(al::LayoutActor*);
    void appearParts(bool);
    void startNumberAction();
    void calcSeaOfTreeIconPos(sead::Vector3f*);
    void setLocalTransAndAppear(MapIconLayout*, MapIconInfo*, const sead::Vector3f&, IconType,
                                bool);
    void calcMapTransAndAppear(MapIconLayout*, MapIconInfo*, const sead::Vector3f&, IconType, bool);
    void scroll(const sead::Vector2f& scrollDistance);
    void addSize(const sead::Vector2f& size);
    bool isAppear() const;
    const sead::Matrix44f& getViewProjMtx() const;
    const sead::Matrix44f& getProjMtx() const;
    void updateIconLine(al::LayoutActor*, const sead::Vector3f&, const sead::Vector2f&);
    void focusIcon(const MapIconInfo*);
    void lostFocusIcon(MapIconLayout*);
    bool tryCalcNorthDir(sead::Vector3f* northDir);

    void exeAppear();
    void exeWait();
    void exeHintInitWait();
    void exeHintAppear();
    void exeHintDecideIconAppear();
    void exeHintDecideIconWait();
    void exeHintPressDecide();
    void exeEnd();
    void exeChangeOut();

private:
    al::SimpleLayoutAppearWaitEnd* mWaitEndMapBg = nullptr;
    al::SimpleLayoutAppearWaitEnd* mWaitEndMapCursor = nullptr;
    al::SimpleLayoutAppearWaitEnd* mWaitEndMapPlayer = nullptr;
    al::SimpleLayoutAppearWaitEnd* mWaitEndMapGuide = nullptr;
    al::SimpleLayoutAppearWaitEnd* mWaitEndMapLine = nullptr;
    DecideIconLayout* mDecideIconLayout = nullptr;
    MapTerrainLayout* mMapTerrainLayout = nullptr;
    TalkMessage* mTalkMessage = nullptr;
    char filler[0x58];
    MapIconLayout* mMapIconLayout = nullptr;
    char filler2[0x68];
    al::PlayerHolder* mPlayerHolder = nullptr;
    bool isPrintWorldChanged;
    bool help;
    sead::Vector2f mScrollPosition;
    sead::Vector2f minScrollPosition;
    sead::Vector2f maxScrollPosition;
    sead::Vector2f PanelSize;
    sead::Vector2f mPanelLocalScale;
    s32 mWorldId;
    s32 currentFontType;
};

void setPanelName(al::LayoutActor* layoutActor, const char* name, s32 id);
bool trySetBalloon(al::LayoutActor* layoutActor, const al::LiveActor* actor,
                   const sead::Matrix44f& matrix);
void setPanelFont(f32 scale, s32* newType, al::LayoutActor* layoutActor, s32 currentType);

namespace rs {
void calcTransOnMap(sead::Vector2f*, const sead::Vector3f&, const sead::Matrix44f&,
                    const sead::Vector2f&, f32, f32);
bool tryCalcMapNorthDir(sead::Vector3f*, const al::IUseSceneObjHolder*);
const sead::Matrix44f& getMapViewProjMtx(const al::IUseSceneObjHolder*);
const sead::Matrix44f& getMapProjMtx(const al::IUseSceneObjHolder*);
void appearMapWithHint(const al::IUseSceneObjHolder*);
void addAmiiboHintToMap(const al::IUseSceneObjHolder*);
void appearMapWithAmiiboHint(const al::IUseSceneObjHolder*);
void appearMapMoonRockDemo(const al::IUseSceneObjHolder*, s32);
void endMap(const al::IUseSceneObjHolder*);
bool isEndMap(const al::IUseSceneObjHolder*);
bool isEnableCheckpointWarp(const al::IUseSceneObjHolder*);
}  // namespace rs

namespace StageMapFunction {
f32 getStageMapScaleMin();
f32 getStageMapScaleMax();
}  // namespace StageMapFunction

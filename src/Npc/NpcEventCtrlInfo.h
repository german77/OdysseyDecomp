#pragma once

#include <basis/seadTypes.h>

#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace al {
class EventFlowNode;
class LiveActor;
class EventFlowChoiceInfo;
class SceneEventFlowMsg;
class EventFlowExecutorHolder;
}  // namespace al

class NpcEventSceneInfo;
class NpcEventSceneConstData;
class EventFlowSceneExecuteCtrl;
class NpcEventBalloonInfo;
class NpcEventTalkInfo;
class TalkMessage;
class BalloonOrderGroupHolder;

class NpcEventCtrlInfo : public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_NpcEventCtrlInfo;

    NpcEventCtrlInfo(const NpcEventSceneInfo&, const NpcEventSceneConstData&,
                     EventFlowSceneExecuteCtrl*);

    bool isCloseTalk() const;
    void popBalloonInfo(NpcEventBalloonInfo*);
    void popTalkInfo(NpcEventTalkInfo*);
    void requestShowBalloonMessage(const NpcEventBalloonInfo&);
    void requestShowTalkMessage(const al::EventFlowNode*, const NpcEventTalkInfo&);
    void requestCloseTalkMessage(const al::LiveActor*);
    void requestCloseWipeFadeBlack(al::EventFlowNode*, s32);
    void requestOpenWipeFadeBlack(al::EventFlowNode*, s32);
    void setBalloonFilterOnlyMiniGame(const al::LiveActor*);
    void resetBalloonFilter(const al::LiveActor*);
    void startChoice(const al::EventFlowNode*, al::EventFlowChoiceInfo*);
    u32 getChoiceMessageNum() const;
    TalkMessage* getChoiceMessage(s32) const;
    u32 getChoiceCancelIndex() const;
    TalkMessage* tryGetChoiceTalkMessage() const;
    void endChoice(s32);
    bool isEnableCancelChoice() const;

    const char* getSceneObjName() const override { return "NPCイベント操作情報"; }

    al::EventFlowExecutorHolder* getExecutorHolder() { return mExecutorHolder; }

private:
    const NpcEventSceneInfo& mSceneInfo;
    const NpcEventSceneConstData& mSceneConstData;
    al::SceneEventFlowMsg* mMsg = nullptr;
    EventFlowSceneExecuteCtrl* mSceneExecutorCtrl;
    al::EventFlowExecutorHolder* mExecutorHolder = nullptr;
    BalloonOrderGroupHolder* mBalloonOrderGroupHolder = nullptr;
    s8 filler[0xA0];
};

static_assert(sizeof(NpcEventCtrlInfo) == 0xD8);

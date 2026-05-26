#pragma once

#include <basis/seadTypes.h>
#include <math/seadVector.h>

namespace al {
struct ActorInitInfo;
class EventFlowExecutor;
class LiveActor;
class MessageTagDataHolder;
class IEventFlowQueryJudge;
class HitSensor;
}  // namespace al


class TalkNpcActionAnimInfo;
class TalkNpcParam;

namespace rs {

TalkNpcParam* initTalkNpcParam(al::LiveActor*, const char*);
void initEventQueryJudge(al::EventFlowExecutor*, const al::IEventFlowQueryJudge*);
void initEventCharacterName(al::EventFlowExecutor*, const al::ActorInitInfo&, const char*);
void initEventParam(al::EventFlowExecutor*, const TalkNpcParam*, const char*);
void tryRegisterSphinxQuizRouteKillSensorAfterPlacement(al::HitSensor*);

al::EventFlowExecutor* initEventFlow(al::LiveActor*, const al::ActorInitInfo&, const char*,
                                     const char*);
al::EventFlowExecutor* initEventFlowSuffix(al::LiveActor*, const al::ActorInitInfo&, const char*,
                                           const char*, const char*);
bool isDefinedEventCamera(const al::EventFlowExecutor*, const char*);
bool checkTriggerDecideWithRequestIcon(al::LiveActor*, const sead::Vector3f&, f32);
void startEventFlow(al::EventFlowExecutor*, const char*);
bool updateEventFlow(al::EventFlowExecutor*);
void initEventMessageTagDataHolder(al::EventFlowExecutor*, const al::MessageTagDataHolder*);
void initEventCameraObject(al::EventFlowExecutor* flowExecutor, const al::ActorInitInfo& initInfo,
                           const char* name);
void initEventCameraObjectAfterKeepPose(al::EventFlowExecutor* flowExecutor,
                                        const al::ActorInitInfo& initInfo, const char* name);
void setEventBalloonFilterOnlyMiniGame(const al::LiveActor*);
void resetEventBalloonFilter(const al::LiveActor*);
void requestSwitchTalkNpcEventAfterDoorSnow(al::LiveActor* actor, s32 doorIndex);
void requestSwitchTalkNpcEventVolleyBall(al::LiveActor*, s32);
bool checkTriggerDecideWithRequestIcon(al::LiveActor*, const sead::Vector3f&, f32);
}  // namespace rs

#pragma once

#include <basis/seadTypes.h>
#include <math/seadQuat.h>
#include <math/seadVector.h>

namespace al {
class ActorInitInfo;
class LiveActor;
class HitSensor;
class SensorMsg;
class MtxConnector;
}  // namespace al

class TalkNpcParam;
class NpcJointLookAtController;
class NpcStateReactionParam;
class PlayerEyeSensorHitHolder;

namespace rs {
bool tryApplyNpcMaterialAnimPresetFromPlacementInfo(al::LiveActor*, const al::ActorInitInfo&,
                                                    const TalkNpcParam*);
void setNpcMaterialAnimFromPlacementInfo(al::LiveActor*, const al::ActorInitInfo&);
bool tryUpdateNpcEyeLineAnim(al::LiveActor*, const TalkNpcParam*);
bool tryUpdateNpcEyeLineAnimToTarget(al::LiveActor*, const TalkNpcParam*, const sead::Vector3f&,
                                     bool);
void resetNpcEyeLineAnim(al::LiveActor*);
bool tryUpdateNpcFacialAnim(al::LiveActor*, const TalkNpcParam*);
bool trySwitchDepthToSelfShadow(al::LiveActor*);
bool tryReceiveMsgPlayerDisregard(const al::SensorMsg*, al::HitSensor*, const TalkNpcParam*);
void attackSensorNpcCommon(al::HitSensor*, al::HitSensor*);
void receiveMsgNpcCommonNoReaction(const al::SensorMsg*, al::HitSensor*, al::HitSensor*);
void calcNpcPushVecBetweenSensors(sead::Vector3f*, al::HitSensor*, al::HitSensor*);
void createAndAppendNpcJointLookAtController(al::LiveActor*, const TalkNpcParam*);
bool tryCreateAndAppendNpcJointLookAtController(al::LiveActor*, const TalkNpcParam*);
bool tryCreateAndAppendNpcJointLookAtController(al::LiveActor*, const TalkNpcParam*, f32);
void setPlayerEyeSensorHitHolder(NpcJointLookAtController*, const PlayerEyeSensorHitHolder*);
void getNpcJointLookAtControlJointNum(const TalkNpcParam*);
void updateNpcJointLookAtController(NpcJointLookAtController*);
void cancelUpdateNpcJointLookAtController(NpcJointLookAtController*);
void updateNpcJointLookAtControllerLookAtDistance(NpcJointLookAtController*, f32);
void invalidateNpcJointLookAtController(NpcJointLookAtController*);
void validateNpcJointLookAtController(NpcJointLookAtController*);
void requestLookAtTargetTrans(NpcJointLookAtController*, const sead::Vector3f&);
void initCapWorldNpcTail(al::LiveActor*);
bool tryStartForestManFlowerAnim(al::LiveActor*);
bool tryUpdateMaterialCodeByFloorCollisionOnArrow(al::LiveActor*);
bool tryAttachConnectorToCollisionTFSV(al::LiveActor*, al::MtxConnector*, sead::Quatf*);
bool tryConnectToCollisionTFSV(al::LiveActor*, al::MtxConnector const*, sead::Quatf*);
bool tryGetSubActorCityMayorFace(al::LiveActor const*);
void syncActionCityMayorFace(al::LiveActor*);
void syncMtsAnimCityMayorFace(al::LiveActor*);
bool isExistFaceAnim(al::LiveActor*, const char*);
void animateCityMayorFace(al::LiveActor*, const char*, f32);
void createNpcStateReaction(al::LiveActor*, const TalkNpcParam*, const NpcStateReactionParam*);
bool isInvalidTrampleSensor(al::HitSensor const*, const TalkNpcParam*);
}  // namespace rs

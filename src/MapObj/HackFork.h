#pragma once

#include <basis/seadTypes.h>
#include <container/seadPtrArray.h>
#include <math/seadMatrix.h>
#include <math/seadQuat.h>
#include <math/seadVector.h>

#include "Library/LiveActor/LiveActor.h"

namespace al {
class ActorInitInfo;
class CameraTargetBase;
class CameraTicket;
class EventFlowExecutor;
class HitSensor;
class MtxConnector;
class SensorMsg;
}  // namespace al

class IUsePlayerHack;
class CapTargetInfo;
class PlayerHackStartShaderCtrl;

class HackFork : public al::LiveActor {
public:
    HackFork(const char* name);
    void init(const al::ActorInitInfo& initInfo) override;
    void attackSensor(al::HitSensor* self, al::HitSensor* other) override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;
    void initAfterPlacement() override;
    void control() override;
    void calcAnim() override;

    void initBasicPoseInfo();
    void tryTouch(float, char const*);
    void resetCapMtx(al::HitSensor*);
    bool isNerveHackable() const;
    bool isHack() const;
    void controlSpring();
    void checkSwing();
    void trySwingJump();
    void updateInput(sead::Vector3f*, sead::Vector3f);
    f32 getJumpRange() const;

    void bendAndTwist(const sead::Vector3f&, const sead::Vector3f&);
    void shoot();
    void updateCapMtx();
    void calcHackDir(al::HitSensor*);

    void exeWait();
    void exeHackStartWait();
    void exeDamping();
    void exeHackStart();
    void exeHackWait();
    void exeHackBend();
    void exeHackShoot();

private:
    IUsePlayerHack* mPlayerHack;
    CapTargetInfo* mCapTargetInfo;
    const char* mJointName;
    sead::Matrix34f matrix1;
    sead::Matrix34f matrix2;
    sead::Matrix34f matrix3;
    sead::Matrix34f matrix4;
    al::CameraTicket* mCameraTicket;
    al::CameraTargetBase* mCameraTargetBase;
    sead::Matrix34f matrix5;
    char lol[16];
    f32 damping2;
    sead::Vector3f leJump;
    f32 damping;
    s32 lol2;
    sead::PtrArray<int> ptrArray;
    f32 touchForce;
    bool isHackSwing;
    s32 hackDelay;
    bool isShoot;
    char llol3[147];
    bool isSensor;
    sead::Quatf quat2;
    char llol32[10];
    sead::Vector3f newJump;
    sead::Vector3f newDir;
    sead::Quatf quat;
    char llol3s[16];
    bool unk;
    bool isLongJump;
    s32 delay;
    s32 airVel;
    al::MtxConnector* mMtxConnector;
    al::EventFlowExecutor* mEventFlowExecutor;
    PlayerHackStartShaderCtrl* mHackStartShaderCtrl;
    sead::Vector3f leControl;
};

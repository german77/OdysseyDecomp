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
    IUsePlayerHack* mPlayerHack=nullptr;
    CapTargetInfo* mCapTargetInfo=nullptr;
    const char* mJointName=nullptr;
    sead::Matrix34f matrix1=sead::Matrix34<float>::ident;
    sead::Matrix34f matrix2=sead::Matrix34<float>::ident;
    sead::Matrix34f matrix3=sead::Matrix34<float>::ident;
    sead::Matrix34f matrix4=sead::Matrix34<float>::ident;
    al::CameraTicket* mCameraTicket=nullptr;
    al::CameraTargetBase* mCameraTargetBase=nullptr;
    sead::Matrix34f matrix5=sead::Matrix34<float>::ident;
    sead::Vector3f zeControl={0.0f,0.0f,0.0f};
    bool zeMessage=false;
    f32 damping2=0.0f;
    sead::Vector3f leJump=sead::Vector3f::ez;
    f32 damping=0.0f;
    s32 lol2;
    sead::PtrArray<int> ptrArray;
    f32 touchForce=0.0f;
    bool isHackSwing=false;
    s32 hackDelay=0;
    bool isShoot=false;
    bool isSheep=false;
    sead::Vector3f* vptr=nullptr;
    s32 inputA=10;
    s32 inputB=0;
    s32 inputC=0;
    sead::Vector3f inputArray[10];
    char llol3[4];
    bool isSensor=false;
    sead::Quatf quat2= sead::Quatf::unit;
    sead::Vector3f hack={0.0f,0.0f,0.0f};
    sead::Vector3f newJump={0.0f,0.0f,0.0f};
    sead::Vector3f newDir{0.0f,0.0f,0.0f};
    sead::Quatf quat = sead::Quatf::unit;
    sead::Quatf quat3= sead::Quatf::unit;
    bool isJump=false;
    bool isLongJump=false;
    s32 delay=0;
    s32 airVel=0;
    al::MtxConnector* mMtxConnector=nullptr;
    al::EventFlowExecutor* mEventFlowExecutor=nullptr;
    PlayerHackStartShaderCtrl* mHackStartShaderCtrl=nullptr;
    sead::Vector3f leControl={0.0f,0.0f,0.0f};
};

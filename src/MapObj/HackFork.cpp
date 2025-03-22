#include "MapObj/HackFork.h"

#include "Library/Camera/CameraTicket.h"
#include "Library/Camera/CameraUtil.h"
#include "Library/Collision/PartsConnector.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Joint/JointLocalAxisRotator.h"
#include "Library/Controller/PadRumbleFunction.h"
#include "Library/Se/SeFunction.h"

#include "Player/IUsePlayerHack.h"
#include "Player/PlayerHackStartShaderCtrl.h"
#include "Scene/GuidePosInfoHolder.h"
#include "Util/Hack.h"
#include "Util/NpcEventFlowUtil.h"
#include "Util/PlayerUtil.h"
#include "Util/SensorMsgFunction.h"

namespace {
NERVE_IMPL(HackFork, Wait);
NERVE_IMPL(HackFork, HackStartWait);
NERVE_IMPL(HackFork, Damping);
NERVE_IMPL(HackFork, HackStart);
NERVE_IMPL(HackFork, HackWait);
NERVE_IMPL(HackFork, HackBend);
NERVE_IMPL(HackFork, HackShoot);

NERVES_MAKE_STRUCT(HackFork, Wait, HackStartWait, Damping, HackStart, HackWait, HackBend,
                   HackShoot);
}  // namespace

HackFork::HackFork(const char* name) : al::LiveActor(name), vptr(&inputArray[0]) {}

void HackFork::init(const al::ActorInitInfo& initInfo) {}

void HackFork::attackSensor(al::HitSensor* self, al::HitSensor* other) {
    if (al::isSensorName(self, "Push") && !al::sendMsgPush(other, self) && !isSensor) {
        const sead::Vector3f& velocity = al::getActorVelocity(other);
        if (velocity.x * velocity.x + velocity.z * velocity.z < 4.999696f)
            rs::sendMsgPushToPlayer(other, self);
    }
}

bool HackFork::receiveMsg(const al::SensorMsg* message, al::HitSensor* other, al::HitSensor* self) {
}

void HackFork::initBasicPoseInfo() {}

void HackFork::initAfterPlacement() {
    if (mMtxConnector != nullptr) {
        sead::Vector3f frontDir;
        al::calcFrontDir(&frontDir, this);
        sead::Vector3f a = al::getTrans(this) + frontDir * 100.0f;
        frontDir = frontDir *= -200.0f;
        al::attachMtxConnectorToCollision(mMtxConnector, this, a, frontDir);
    }
}

bool HackFork::tryTouch(float force, char const* reaction) {
    if (delay != 0) {
        delay = 30;
        return false;
    }

    if (isNerveHackable()) {
        touchForce = force;
        delay = 30;
        al::setNerve(this, &NrvHackFork.Damping);
        al::startHitReaction(this, reaction);
        return true;
    }
    return false;
}

void HackFork::resetCapMtx(al::HitSensor* sensor) {
    calcHackDir(sensor);
    sead::Matrix34f mtx = *al::getJointMtxPtr(this, mJointName);
    al::normalize(&mtx);

    sead::Matrix34f mat;
    mat.setMul(mtx, matrix3);

    sead::Vector3f beto;
    sead::Vector3f vecy;
    if (isSensor)
        vecy.set({0.0f, 1.0f, 0.0f});
    else
        vecy = -hack;

    al::verticalizeVec(&vecy, beto, vecy);

    if (!al::tryNormalizeOrZero(&vecy)) {
        matrix4 = matrix3;
    } else {
        sead::Matrix34f nike;
        nike.setInverse(mat);
        sead::Quatf saul;
        al::makeQuatRotationRate(&saul, sead::Vector3f::ez, vecy, 1.0f);
        sead::Matrix34f pina;
        pina.setInverse(mtx);
        matrix4.setMul(mat, pina);
    }
    updateCapMtx();
}

bool HackFork::isNerveHackable() const {
    return al::isNerve(this, &NrvHackFork.Wait) || al::isNerve(this, &NrvHackFork.Damping);
}

bool HackFork::isHack() const {
    return al::isNerve(this, &NrvHackFork.HackStartWait) ||
           al::isNerve(this, &NrvHackFork.HackStart) || al::isNerve(this, &NrvHackFork.HackWait) ||
           al::isNerve(this, &NrvHackFork.HackBend) || al::isNerve(this, &NrvHackFork.HackShoot);
}

void HackFork::controlSpring() {
    f32 fVar2 = damping * 0.92f;
    touchForce += fVar2 * -0.5f;
    damping = fVar2 + touchForce;
}

void HackFork::checkSwing() {
    if (rs::isTriggerHackSwing(mPlayerHack)) {
        isHackSwing = true;
        hackDelay = 0;
    } else {
        hackDelay++;
    }
}

bool HackFork::trySwingJump() {  
  if (rs::isTriggerHackSwing(mPlayerHack)) {
    isHackSwing = true;
    hackDelay = 0;
  }
  else {
    if (!isHackSwing||++hackDelay > 10 ) {
      return false;
    }
  }

  if (isSensor) {
    newJump.set({0.0f,-1.0f,0.0f});
  }
  else {
    sead::Vector3f tmpF=hack;
    tmpF.rotate(quat*quat2);
    al::tryNormalizeOrDirZ(&tmpF);
    newJump.set(tmpF);
  }
  isJump = newJump.dot(upDir) < 0.0f;
  
  damping = 22.5f;
  sead::Vector3f frontDir;
  al::calcFrontDir(&frontDir,this);
  bendAndTwist(newJump,frontDir);
  isShoot = true;
  al::setNerve(this, &NrvHackFork.HackShoot);
  return true;
}

bool HackFork::updateInput(sead::Vector3f*, sead::Vector3f) {
return true;
}

f32 HackFork::getJumpRange() const {
    return isLongJump ? 180.0f : 45.0f;
}

void HackFork::bendAndTwist(const sead::Vector3f& param_1, const sead::Vector3f& param_2) {
  damping = sead::Mathf::clampMax(damping + 1.0f,22.5f);
  touchForce = 0.0f;
  
  leJump.setCross(param_2,param_1);
  if (!al::tryNormalizeOrZero(&leJump)) {
    al::calcSideDir(&leJump,this);
  }
  for(s32 i=0;i<ptrArray.size();i++){
      ptrArray[i]->setVector28(leJump);
  }

}

void HackFork::shoot() {
    sead::Vector3f frontDir;
    al::calcFrontDir(&frontDir, this);
    sead::Quatf quack;
    al::makeQuatUpFront(&quack, frontDir, newJump);
    if (quack.length() > 0)
        upDir = frontDir;
    rs::resetRouteHeadGuidePosPtr(this);
    al::startHitReaction(this, "ジャンプ");
    al::tryStartAction(this, "HackEnd");
}

void HackFork::control() {}

void HackFork::calcAnim() {
    al::LiveActor::calcAnim();
    if (isHack())
        updateCapMtx();
}

void HackFork::updateCapMtx() {
    sead::Matrix34f mtx = *al::getJointMtxPtr(this, mJointName);
    al::normalize(&mtx);
    matrix1.setMul(mtx, matrix4);
}

void HackFork::calcHackDir(al::HitSensor* sensor) {
    if (isSensor)
        hack.set({0.0f, -1.0f, 0.0f});
    else
        hack = rs::getPlayerPos(this) - al::getSensorPos(sensor);

    sead::Vector3f frontDir;
    al::calcFrontDir(&frontDir, this);
    al::verticalizeVec(&hack, frontDir, hack);
    if (!al::tryNormalizeOrZero(&hack))
        al::calcUpDir(&hack, this);

    quat.inverse(&quat2);
}

void HackFork::exeWait() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "Wait");
        al::showModelIfHide(this);
    }
    if (mEventFlowExecutor != nullptr)
        rs::updateEventFlow(mEventFlowExecutor);
}

void HackFork::exeHackStartWait() {
    if (al::isFirstStep(this)) {
        if (mCameraTicket != nullptr) {
            al::startCamera(this, mCameraTicket, -1);
            al::requestStopCameraVerticalAbsorb(this);
        }
        al::setCameraTarget(this, mCameraTargetBase);
        isHackSwing = false;
        hackDelay = 0;
        isShoot = false;
        airVel = 0;
    }
    if (!rs::isTriggerHackSwing(mPlayerHack)) {
        hackDelay++;
    } else {
        isHackSwing = true;
        hackDelay = 0;
    }
    if (rs::isHackStartDemoEnterMario(mPlayerHack))
        al::setNerve(this, &NrvHackFork.HackStart);
}

void HackFork::exeDamping() {
    if (al::isFirstStep(this))
        al::showModelIfHide(this);
    if (!al::isNearZero(touchForce, 0.001f) || !al::isNearZero(damping, 0.001f)) {
        controlSpring();
    } else {
        if (al::isNearZero(damping2, 0.001f)) {
            damping2 = 0.0f;
            damping = 0.0f;
            touchForce = 0.0f;
            al::validateClipping(this);
            al::setNerve(this, &NrvHackFork.Wait);
            return;
        }
        damping2 *= 0.9f;
    }
}

void HackFork::exeHackStart() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "HackStart");
        mHackStartShaderCtrl->start();
        newJump = {0.0f, 0.0f, 0.0f};
    }
    checkSwing();
    if (al::isActionEnd(this)) {
        rs::endHackStartDemo(mPlayerHack, this);
        al::setNerve(this, &NrvHackFork.HackWait);
    }
}

void HackFork::exeHackWait() {  
  controlSpring();
  inputB = 0;
  inputC = 0;
  sead::Vector3f input;
  al::calcFrontDir(&input,this);
  sead::Vector3f input2;
  if (!trySwingJump() &&updateInput(&input,input2)) {
    f32 dot = sead::Mathf::abs(input.dot(upDir));
    f32 cos = sead::Mathf::cos(isLongJump?3.1415927f:0.78539819f);
    if (cos < dot) {
      newJump=input;
      isJump = newJump.dot(upDir) < 0.0f;
      al::setNerve(this, &NrvHackFork.HackBend);
    }
  }
}

void HackFork::exeHackBend() {
    if (al::isFirstStep(this)) {
    isSheep = false;
  }
  
  if (!al::isLessEqualStep(this,15) || !trySwingJump()) {
    sead::Vector3f frontDir;
    al::calcFrontDir(&frontDir,this);
    
    sead::Vector3f input;
    sead::Vector3f input2;
    bool isInput=updateInput(&input,input2);
    if (!isInput || rs::isTriggerHackSwing(mPlayerHack)) {
      if (isSheep == false) {
        al::setNerve(this, &NrvHackFork.HackWait);
      }
      else {
        if (isInput) {
          airVel = 60;
          isShoot = true;
        }
        al::setNerve(this, &NrvHackFork.HackShoot);
      }
    }
    f32 jumpDir = isJump?-1.0f:1.0f;
    sead::Vector3f oldJump=newJump;
    
    f32 angle = sead::Mathf::clamp((jumpDir*upDir).dot(input),-1.0f,1.0f);
    if (sead::Mathf::rad2deg(acosf(angle)) < (isLongJump?180.0f:45.0f)) {
      newJump = newJump * 0.5f + input * 0.5f;
    }
    al::normalize(&newJump);
    f32 oldDamping=damping;
    bendAndTwist(newJump,frontDir);
    sead::Vector3f nani = newJump - oldJump;
    f32 rumbleVolume=(damping - oldDamping) * 0.5f +nani.length() * 3.3f;
    if (!al::isNearZero(rumbleVolume,0.001f)) {
      al::holdSe(this,"PgBendLv");
      sead::Vector3f jointPos;
      al::calcJointPos(&jointPos,this,"Stick03");
      al::PadRumbleParam param=al::PadRumbleParam( 0.0f,  1300.0f,  rumbleVolume, rumbleVolume);
      alPadRumbleFunction::startPadRumbleWithParam(this,jointPos,"パルス（中）", param,-1);
    }
    if (4.5 <= damping) {
      isSheep = true;
    }
  }
}

void HackFork::exeHackShoot() {
    controlSpring();
    if (damping < 0.0f) {
        shoot();
        al::setNerve(this, &NrvHackFork.Damping);
    }
}

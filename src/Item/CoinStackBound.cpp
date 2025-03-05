#include "Item/CoinStackBound.h"

#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/Nature/NatureUtil.h"
#include "Library/Movement/FlashingTimer.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorAreaFunction.h"
#include "Library/Nature/WaterSurfaceFinder.h"
#include "Library/Effect/EffectSystemInfo.h"
#include "Library/Joint/JointControllerKeeper.h"
#include "Library/LiveActor/ActorCollisionFunction.h"
#include "Library/Math/MathUtil.h"
#include "Library/LiveActor/ActorSensorUtil.h"

#include "System/GameDataFunction.h"
#include "Util/SensorMsgFunction.h"

namespace {
NERVE_IMPL(CoinStackBound, Alive)
NERVE_IMPL(CoinStackBound, Appear)
NERVE_IMPL(CoinStackBound, Fall)
NERVE_IMPL(CoinStackBound, Bounce)
NERVE_IMPL(CoinStackBound, Vanish)
NERVE_IMPL(CoinStackBound, Collected)

NERVES_MAKE_NOSTRUCT(CoinStackBound,Alive, Appear, Collected)
NERVES_MAKE_STRUCT(CoinStackBound, Vanish, Fall,Bounce)
}  // namespace

CoinStackBound::CoinStackBound(char const* name):al::LiveActor(name){
matrix = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         1.0f, 0.0f};
                                         }

void CoinStackBound::init(const al::ActorInitInfo& initInfo) {  
  al::initActorWithArchiveName(this,initInfo,"CoinStackBound",nullptr);
  al::initJointControllerKeeper(this,1);
  quat.w=1.0f;
  quat.x=0.0f;
  quat.y=0.0f;
  quat.z=0.0f;
  al::initJointGlobalQuatController(this,&quat,"AllRoot");
  
  mFlashingTimer = new al::FlashingTimer(500,180,20,10);
  mWaterSurfaceFinder = new al::WaterSurfaceFinder(this);
  al::initNerve(this,&Alive,0);
  al::setEffectNamedMtxPtr(this,"跳ね返るエフェクトの原点",&matrix);
  makeActorDead();
}

bool CoinStackBound::receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                al::HitSensor* self) {
  if (rs::isMsgItemGetAll(message)&& counter != 0 &&
  mFlashingTimer->getLastTime() < 470) {
    al::invalidateClipping(this);
    al::setNerve(this,&Collected);
    return true;
  }
  return false;
  }
                
void CoinStackBound::appear() {}

void CoinStackBound::processTimer(){
  mFlashingTimer->update();
  if (mFlashingTimer->isHurryStart() && !timerStuf) {
    timerStuf = true;
    al::startHitReaction(this,"タイマー急ぎ期間開始");
  }
  if ( mFlashingTimer->getTime() <= 0) {
    al::setNerve(this,&NrvCoinStackBound.Vanish);
  }
}

void CoinStackBound::exeAlive(){ 
  al::startHitReaction(this,"タイマー起動");
  al::setNerve(this,&Appear);
  }

void CoinStackBound::exeAppear(){
  al::startHitReaction(this,"出力");
  al::setVelocitySeparateHV(this,vector1,10.0f,30.0f);
  al::setNerve(this,&NrvCoinStackBound.Fall);
  }

void CoinStackBound::exeFall(){
  
  if (!al::isInWater(this) && !al::isInDeathArea(this) &&
     !al::isCollidedGroundFloorCode(this,"DamageFire")&&
    !al::isCollidedGroundFloorCode(this,"Needle") &&
      !al::isCollidedGroundFloorCode(this,"Poison")) {
    mFlashingTimer->update();
    if (mFlashingTimer->isHurryStart() && timerStuf == false) {
      timerStuf = true;
      al::startHitReaction(this,"タイマー急ぎ期間開始");
    }
    if (mFlashingTimer->getTime() < 1) {
    al::setNerve(this,&NrvCoinStackBound.Vanish);
    }
    al::rotateQuatRadian(&quat,quat,vector2,sead::Mathf::deg2rad(rotateAngle));
    rotateAngle = rotateAngle * 0.995f;
    al::addVelocityToGravity(this,1.1f);
    al::scaleVelocityHV(this,1.0f,1.0f);
    if (!al::isOnGround(this,0)) {
      if (!nibool || !al::isCollidedWall(this)) {
        return;
      }
      vector3 = al::getCollidedWallNormal(this);
      someBools = false;
      nibool = false;
    al::setNerve(this,&NrvCoinStackBound.Bounce);
    }
    else {
      vector3 = al::getOnGroundNormal(this,0);
      someBools = true;
      nibool = true;
    al::setNerve(this,&NrvCoinStackBound.Bounce);
    }
  }
  else {
    al::setNerve(this,&NrvCoinStackBound.Vanish);
  }
}

void CoinStackBound::exeBounce(){
  if (al::isFirstStep(this) && someBools) {
    mWaterSurfaceFinder->update(al::getTrans(this),vector3,al::getSensorRadius(this,"Body"));
    if (!mWaterSurfaceFinder->isFoundSurface()) ||
        al::isInWaterPos(this,mWaterSurfaceFinder->getSurfacePosition())) {
        al::updateMaterialCodePuddle(this,false);
    al::makeMtxUpNoSupportPos(&matrix,vector3,al::getTrans(this));
    }
    else {
      al::updateMaterialCodePuddle(this,true);
    al::makeMtxUpNoSupportPos(&matrix,mWaterSurfaceFinder->getSurfaceNormal(),mWaterSurfaceFinder->getSurfacePosition());
    }
    al::startHitReaction(this,"跳ね返る");
    someBools = false;
  }
  counter++;
  mFlashingTimer->update();
  if (mFlashingTimer->isHurryStart() && !timerStuf) {
    timerStuf = true;
    al::startHitReaction(this,"タイマー急ぎ期間開始");
  }
  if (mFlashingTimer->getTime() < 1) {
    al::setNerve(this,&NrvCoinStackBound.Vanish);
  }
  /*fVar4 = (vector3).x;
  fVar7 = (vector3).y;
  fVar8 = (vector3).z;
  fVar6 = fVar7 * 0.0 - fVar8 * 1.0;
  fVar9 = fVar8 * 0.0 - fVar4 * 0.0;
  fVar5 = fVar4 * 1.0 - fVar7 * 0.0;
  fVar4 = atan2f(SQRT(fVar5 * fVar5 + fVar6 * fVar6 + fVar9 * fVar9),
                 fVar4 * 0.0 + fVar7 * 1.0 + fVar8 * 0.0);
  if ((25.0 < fVar4 * 57.29578) && (fVar4 * 57.29578 < 65.0)) {
    (vector3).x = 0.0;
    (vector3).y = 1.0;
    (vector3).z = 0.0;
  }
  pVVar3 = al::getVelocityPtr(this);
  al::calcReflectionVector(pVVar3,&vector3,0.95,0.0);
  al::separateVelocityHV(&local_60,&local_70,this);
  local_60.x = local_60.x * 0.9;
  local_60.y = local_60.y * 0.9;
  local_60.z = local_60.z * 0.9;
  fVar5 = SQRT(local_60.z * local_60.z + local_60.x * local_60.x + local_60.y * local_60.y);
  fVar4 = SQRT(local_70.x * local_70.x + local_70.y * local_70.y + local_70.z * local_70.z);
  if (28.0 < fVar4) {
    fVar4 = 28.0 / fVar4;
    local_70.x = fVar4 * local_70.x;
    local_70.y = fVar4 * local_70.y;
    local_70.z = fVar4 * local_70.z;
  }
  fVar4 = lefloat;
  if (anotherbool == false) {
    if (fVar5 <= fVar4) goto LAB_71001c64cc;
    anotherbool = true;
  }
  fVar5 = fVar4 / fVar5;
  local_60.x = local_60.x * fVar5;
  local_60.y = fVar5 * local_60.y;
  local_60.z = fVar5 * local_60.z;
  if (0.0 < fVar4) {
    lefloat = fVar4 + -1.0;
  }
LAB_71001c64cc:
  local_80.x = local_60.x + local_70.x;
  local_80.y = local_60.y + local_70.y;
  local_80.z = local_60.z + local_70.z;
  al::setVelocity(this,&local_80);
  pVVar3 = al::getVelocity(this);
  fVar5 = pVVar3->x;
  fVar6 = pVVar3->y;
  fVar7 = pVVar3->z;
  fVar4 = SQRT(fVar5 * fVar5 + fVar6 * fVar6 + fVar7 * fVar7);
  if (0.0 < fVar4) {
    fVar4 = 1.0 / fVar4;
    fVar5 = fVar5 * fVar4;
    fVar6 = fVar6 * fVar4;
    fVar7 = fVar7 * fVar4;
  }
  pVVar3 = al::getGravity(this);
  fVar8 = fVar7 * pVVar3->y - fVar6 * pVVar3->z;
  fVar7 = fVar5 * pVVar3->z - fVar7 * pVVar3->x;
  fVar4 = fVar6 * pVVar3->x - fVar5 * pVVar3->y;
  (vector2).z = fVar4;
  (vector2).y = fVar7;
  (vector2).x = fVar8;
  vector2.normalize();*/
  al::setNerve(this,&NrvCoinStackBound.Fall);
}

void CoinStackBound::exeVanish(){
  al::startHitReaction(this,"消える");
  al::tryAddRippleMiddle(this);
  makeActorDead();
}

void CoinStackBound::exeCollected(){  
  al::startHitReaction(this,"取得");
  GameDataFunction::addCoin(this,10);
  makeActorDead();
  }

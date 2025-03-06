#include "Item/CoinStackBound.h"

#include "Library/Effect/EffectSystemInfo.h"
#include "Library/Joint/JointControllerKeeper.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorAreaFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorCollisionFunction.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Matrix/MatrixUtil.h"
#include "Library/Movement/FlashingTimer.h"
#include "Library/Nature/NatureUtil.h"
#include "Library/Nature/WaterSurfaceFinder.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"

#include "System/GameDataFunction.h"
#include "Util/SensorMsgFunction.h"

namespace {
NERVE_IMPL(CoinStackBound, Alive)
NERVE_IMPL(CoinStackBound, Appear)
NERVE_IMPL(CoinStackBound, Fall)
NERVE_IMPL(CoinStackBound, Bounce)
NERVE_IMPL(CoinStackBound, Vanish)
NERVE_IMPL(CoinStackBound, Collected)

NERVES_MAKE_NOSTRUCT(CoinStackBound, Alive, Appear, Collected)
NERVES_MAKE_STRUCT(CoinStackBound, Vanish, Fall, Bounce)
}  // namespace

CoinStackBound::CoinStackBound(char const* name) : al::LiveActor(name) {
    matrix = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
}

void CoinStackBound::init(const al::ActorInitInfo& initInfo) {
    al::initActorWithArchiveName(this, initInfo, "CoinStackBound", nullptr);
    al::initJointControllerKeeper(this, 1);
    quat.w = 1.0f;
    quat.x = 0.0f;
    quat.y = 0.0f;
    quat.z = 0.0f;
    al::initJointGlobalQuatController(this, &quat, "AllRoot");

    mFlashingTimer = new al::FlashingTimer(500, 180, 20, 10);
    mWaterSurfaceFinder = new al::WaterSurfaceFinder(this);
    al::initNerve(this, &Alive, 0);
    al::setEffectNamedMtxPtr(this, "跳ね返るエフェクトの原点", &matrix);
    makeActorDead();
}

bool CoinStackBound::receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                                al::HitSensor* self) {
    if (rs::isMsgItemGetAll(message) && counter != 0 && mFlashingTimer->getLastTime() < 470) {
        al::invalidateClipping(this);
        al::setNerve(this, &Collected);
        return true;
    }
    return false;
}

void CoinStackBound::appear() { /*pVVar1 = al::getTrans(this);
 pfVar2 = (float *)al::getPlayerPos(this,0);
 fVar7 = pVVar1->y - pfVar2[1];
 fVar9 = pfVar2[2];
 fVar6 = pVVar1->x - *pfVar2;
 fVar8 = pVVar1->z;
 (this->vector1).y = fVar7;
 fVar8 = fVar8 - fVar9;
 (this->vector1).x = fVar6;
 fVar6 = SQRT(fVar6 * fVar6 + fVar7 * fVar7 + fVar8 * fVar8);
 (this->vector1).z = fVar8;
 pVVar1 = &this->vector1;
 if (0.0 < fVar6) {
   fVar6 = 1.0 / fVar6;
   (this->vector1).x = fVar6 * (this->vector1).x;
   (this->vector1).y = fVar6 * (this->vector1).y;
   (this->vector1).z = fVar6 * (this->vector1).z;
 }
 pVVar3 = al::getGravity(this);
 if (0.85 < (this->vector1).x * pVVar3->x + (this->vector1).y * pVVar3->y +
            (this->vector1).z * pVVar3->z) {
   (this->vector1).x = 0.0;
   (this->vector1).y = 0.0;
   (this->vector1).z = 1.0;
   pQVar4 = al::getQuat(this);
   al::rotateVectorQuat(pVVar1,pQVar4);
 }
 fVar6 = (float)al::getRandom(20.0,50.0);
 fVar7 = (float)al::getRandom();
 fVar8 = 1.0;
 if (fVar7 <= 0.5) {
   fVar8 = -1.0;
 }
 pVVar3 = al::getGravity(this);
 local_40.y = -pVVar3->y;
 local_40.x = -pVVar3->x;
 local_40.z = -pVVar3->z;
 al::rotateVectorDegree(pVVar1,pVVar1,&local_40,fVar6 * fVar8);
 pVVar1 = al::getGravity(this);
 fVar6 = (this->vector1).y;
 fVar7 = (this->vector1).z;
 fVar8 = fVar7 * pVVar1->y - fVar6 * pVVar1->z;
 fVar9 = (this->vector1).x;
 fVar7 = fVar9 * pVVar1->z - fVar7 * pVVar1->x;
 fVar6 = fVar6 * pVVar1->x - fVar9 * pVVar1->y;
 (this->vector2).y = fVar7;
 (this->vector2).z = fVar6;
 fVar6 = SQRT(fVar6 * fVar6 + fVar8 * fVar8 + fVar7 * fVar7);
 (this->vector2).x = fVar8;
 if (0.0 < fVar6) {
   fVar6 = 1.0 / fVar6;
   (this->vector2).x = fVar6 * (this->vector2).x;
   (this->vector2).y = fVar6 * (this->vector2).y;
   (this->vector2).z = fVar6 * (this->vector2).z;
 }
 this->counter = 0;
 this->rotateAngle = 30.0;
 this->timerStuf = false;
 local_40.x = 0.0;
 local_40.y = 0.0;
 local_40.z = 0.0;
 al::setVelocity(this,&local_40);
 mFlashingTimer->start(500,0xb4,0x14,10);
 al::invalidateClipping(this);
 if (al::isHideModel(this)) {
   al::showModel(this);
 }
 vector3.x = 0.0f;
 vector3.y = 0.0f;
 vector3.z = 0.0f;
 quat.w = 1.0f;
 quat.x = 0.0f;
 quat.y = 0.0f;
 quat.z = 0.0f;
 al::setNerve(this,&Alive);
 makeActorAlive();*/
}

void CoinStackBound::processTimer() {
    mFlashingTimer->update();
    if (mFlashingTimer->isHurryStart() && !timerStuf) {
        timerStuf = true;
        al::startHitReaction(this, "タイマー急ぎ期間開始");
    }
    if (mFlashingTimer->getTime() <= 0)
        al::setNerve(this, &NrvCoinStackBound.Vanish);
}

void CoinStackBound::exeAlive() {
    al::startHitReaction(this, "タイマー起動");
    al::setNerve(this, &Appear);
}

void CoinStackBound::exeAppear() {
    al::startHitReaction(this, "出力");
    al::setVelocitySeparateHV(this, vector1, 10.0f, 30.0f);
    al::setNerve(this, &NrvCoinStackBound.Fall);
}

void CoinStackBound::exeFall() {
    if (!al::isInWater(this) && !al::isInDeathArea(this) &&
        !al::isCollidedGroundFloorCode(this, "DamageFire") &&
        !al::isCollidedGroundFloorCode(this, "Needle") &&
        !al::isCollidedGroundFloorCode(this, "Poison")) {
        processTimer();
        al::rotateQuatRadian(&quat, quat, vector2, sead::Mathf::deg2rad(rotateAngle));
        rotateAngle = rotateAngle * 0.995f;
        al::addVelocityToGravity(this, 1.1f);
        al::scaleVelocityHV(this, 1.0f, 1.0f);
        if (al::isOnGround(this, 0)) {
            vector3 = al::getOnGroundNormal(this, 0);
            isOnGound = true;
            nibool = true;
            al::setNerve(this, &NrvCoinStackBound.Bounce);
            return;
        }
        if (nibool && al::isCollidedWall(this)) {
            vector3 = al::getCollidedWallNormal(this);
            isOnGound = false;
            nibool = false;
            al::setNerve(this, &NrvCoinStackBound.Bounce);
        }
    } else {
        al::setNerve(this, &NrvCoinStackBound.Vanish);
    }
}

float getAngle(const sead::Vector3f& v1, const sead::Vector3f& v2) {
    sead::Vector3f cross;
    float dot = v1.dot(v2);
    cross.setCross(v1, v2);
    return atan2f(cross.length(), dot);
}

void CoinStackBound::exeBounce() {
    if (al::isFirstStep(this) && isOnGound) {
        mWaterSurfaceFinder->update(al::getTrans(this), vector3, al::getSensorRadius(this, "Body"));
        if (!mWaterSurfaceFinder->isFoundSurface() ||
            !al::isInWaterPos(this, mWaterSurfaceFinder->getSurfacePosition())) {
            al::updateMaterialCodePuddle(this, false);
            al::makeMtxUpNoSupportPos(&matrix, vector3, al::getTrans(this));
        } else {
            al::updateMaterialCodePuddle(this, true);
            al::makeMtxUpNoSupportPos(&matrix, mWaterSurfaceFinder->getSurfaceNormal(),
                                      mWaterSurfaceFinder->getSurfacePosition());
        }
        al::startHitReaction(this, "跳ね返る");
        isOnGound = false;
    }

    counter++;
    processTimer();

    f32 angle = sead::Mathf::rad2deg(getAngle(vector3, sead::Vector3f::ey));
    if (25.0f < angle && angle < 65.0f)
        vector3 = sead::Vector3f::ey;

    al::calcReflectionVector(al::getVelocityPtr(this), vector3, 0.95f, 0.0f);

    sead::Vector3f verticalVelocity;
    sead::Vector3f horizontalVelocity;
    al::separateVelocityHV(&verticalVelocity, &horizontalVelocity, this);

    verticalVelocity *= 0.9f;
    f32 verticalLength = verticalVelocity.length();

    f32 horizontalLength = horizontalVelocity.length();
    if (28.0f < horizontalLength)
        horizontalVelocity *= 28.0f / horizontalLength;

    f32 fVar4 = lefloat;
    if (!anotherbool) {
        if (verticalLength > fVar4) {
            anotherbool = true;
            verticalVelocity *= fVar4 / verticalLength;
            if (fVar4 > 0.0f)
                lefloat = fVar4 - 1.0f;
        }
    } else {
        verticalVelocity *= fVar4 / verticalLength;
        if (fVar4 > 0.0f)
            lefloat = fVar4 - 1.0f;
    }

    al::setVelocity(this, verticalVelocity + horizontalVelocity);

    sead::Vector3f velocity = al::getVelocity(this);
    velocity.normalize();

    sead::Vector3f gravity = al::getGravity(this);
    vector2.setCross(velocity, gravity);
    vector2.normalize();

    al::setNerve(this, &NrvCoinStackBound.Fall);
}

void CoinStackBound::exeVanish() {
    al::startHitReaction(this, "消える");
    al::tryAddRippleMiddle(this);
    makeActorDead();
}

void CoinStackBound::exeCollected() {
    al::startHitReaction(this, "取得");
    GameDataFunction::addCoin(this, 10);
    makeActorDead();
}

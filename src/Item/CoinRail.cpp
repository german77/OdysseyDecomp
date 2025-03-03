#include "Item/CoinRail.h"

#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorInitFunction.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorSensorFunction.h"
#include "Library/LiveActor/ActorSensorMsgFunction.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Rail/RailUtil.h"

#include "Item/Coin.h"

namespace {
NERVE_IMPL(CoinRail, Move)
NERVE_IMPL(CoinRail, CloseMove)

NERVES_MAKE_STRUCT(CoinRail, CloseMove, Move)
}  // namespace

CoinRail::CoinRail(const char* name) : al::LiveActor(name) {}

void CoinRail::init(const al::ActorInitInfo& initInfo) {
    al::initActor(this, initInfo);

    if (!al::isExistRail(this)) {
        kill();
        return;
    }
    
    al::getArg(&mCoinNum, initInfo, "CoinNum");
    if (mCoinNum <= 1) {
        kill();
        return;
    }
    
    bool isLoopRail = al::isLoopRail(this);
    if (al::getRailPointNum(this) <= 1) {
        kill();
        return;
    }
    
    al::tryGetArg(&mMoveVelocity, initInfo, "MoveVelocity");
    if (mMoveVelocity < 0.0f) {
        kill();
        return;
    }
    
    al::tryGetDisplayOffset(&mDisplayOffset, initInfo);
    mCoinArray = new Coin*[mCoinNum];
    mCoinPosition = new float[mCoinNum];
    if (!al::isNearZero(mMoveVelocity, 0.001f)) {
        f32 railPosition = 0.0f;
        if(mCoinNum>0)
        for (s32 i = mCoinNum; i != 0; i--) {
            sead::Vector3f postion = sead::Vector3f::zero;
            al::calcRailPosAtCoord(&postion, this, railPosition);
            Coin* coin = new Coin("コイン", false);
            al::initCreateActorWithPlacementInfo(coin, initInfo);
            mCoinArray[i] = coin;
            coin->makeActorDead();
            mCoinPosition[i] = railPosition;
            al::setTrans(coin, postion);
            railPosition += 150.0;
            al::tryAddDisplayOffset(coin, initInfo);
        }
        al::getRailTotalLength(this);
    } else {
        f32 totalLength = al::getRailTotalLength(this);
        f32 totalSegments = mCoinNum - (isLoopRail ? 1 : 0);
        f32 railPosition = 0.0f;
        if(mCoinNum>0)
        for (s32 i = mCoinNum; i != 0; i--) {
            sead::Vector3f postion;
            al::calcRailPosAtCoord(&postion, this, railPosition);
            Coin* coin = new Coin("コイン", false);
            al::initCreateActorWithPlacementInfo(coin, initInfo);
            mCoinArray[i] = coin;
            coin->makeActorDead();
            al::setTrans(coin, postion);
            mCoinPosition[i] = railPosition;
            al::tryAddDisplayOffset(coin, initInfo);
            railPosition += totalLength / totalSegments;
        }
    }

    f32 shadowLength = 1500.0f;
    al::tryGetArg(&shadowLength, initInfo, "ShadowLength");
    for (s32 i = 0; i < mCoinNum; i++)
        mCoinArray[i]->setShadowDropLength(shadowLength);
    anotherKindOfIndex = mCoinNum + -1;
    someSortOfIndex = 0;
    f32 local_84 = 0.0;
    al::calcRailClippingInfo(&mRailClippingInfo, &local_84, this, 100.0f, 100.0f);
    al::setClippingInfo(this, local_84, &mRailClippingInfo);
    al::initSubActorKeeperNoFile(this, initInfo, mCoinNum);

    for (s32 i = 0; i < mCoinNum; i++) {
        al::invalidateClipping(mCoinArray[i]);
        al::registerSubActorSyncClipping(this, mCoinArray[i]);
    }
    
    if (isLoopRail)
        al::initNerve(this, &NrvCoinRail.CloseMove, 0);
    else
        al::initNerve(this, &NrvCoinRail.Move, 0);
    al::LiveActor::makeActorDead();

    if (!al::trySyncStageSwitchAppear(this)) {
        al::LiveActor::appear();
        for (s32 i = 0; i < mCoinNum; i++)
            mCoinArray[i]->appearCoinRail();
    }
    al::invalidateHitSensors(this);
}

void CoinRail::appear() {
    al::LiveActor::appear();
    al::startHitReaction(this, "出現");
    for (s32 i = 0; i < mCoinNum; i++)
        mCoinArray[i]->appearCoinRail();
}

void CoinRail::kill() {
    al::LiveActor::kill();
    for (s32 i = 0; i < mCoinNum; i++)
        mCoinArray[i]->kill();
}

void CoinRail::makeActorDead() {
    al::LiveActor::makeActorDead();
    for (s32 i = 0; i < mCoinNum; i++)
        mCoinArray[i]->makeActorDead();
}

bool CoinRail::receiveMsg(const al::SensorMsg* message, al::HitSensor* other, al::HitSensor* self) {
    if (al::isMsgShowModel(message)) {
        for (s32 i = 0; i < mCoinNum; i++)
            if (!mCoinArray[i]->isGot())
                al::showModelIfHide(mCoinArray[i]);
        return true;
    }

    if (al::isMsgHideModel(message)) {
        for (s32 i = 0; i < mCoinNum; i++)
            al::hideModelIfShow(mCoinArray[i]);
        return true;
    }

    return false;
}

bool CoinRail::isGot() const {
    for (s32 i = 0; i < mCoinNum; i++)
        if (!mCoinArray[i]->isGot())
            return false;
    return true;
}

void CoinRail::exeMove() {
  if (!al::isNearZero(mMoveVelocity,0.001f)) {
    if (mMoveVelocity <= 0.0) {
      mCoinPosition[someSortOfIndex] +=mMoveVelocity;
      uVar3._0_4_ = someSortOfIndex;
      uVar3._4_4_ = anotherKindOfindex;
      fVar9 = *(float *)((long)mCoinPosition +
                        (-(uVar3 >> 0x1f & 1) & 0xfffffffc00000000 | (uVar3 & 0xffffffff) << 2));
      if (fVar9 < 0.0) {
        fVar9 = 0.0;
        mMoveVelocity = -mMoveVelocity;
      }
      local_60.z = 0.0;
      local_60.x = 0.0;
      local_60.y = 0.0;
      if ((int)uVar3 <= (int)(uVar3 >> 0x20)) {
        lVar7 = (long)(int)uVar3;
        do {
          al::calcRailPosAtCoord(&local_60,(IUseRail *)&(this).railRider,fVar9);
          mCoinPosition[lVar7] = fVar9;
          local_70.x = local_60.x + (mDisplayOffset).x;
          local_70.y = local_60.y + (mDisplayOffset).y;
          local_70.z = local_60.z + (mDisplayOffset).z;
          fVar9 = fVar9 + 150.0;
          al::setTrans(&mCoinArray[lVar7]->actor,&local_70);
          bVar1 = lVar7 < anotherKindOfindex;
          lVar7 = lVar7 + 1;
        } while (bVar1);
        uVar3._0_4_ = someSortOfIndex;
        uVar3._4_4_ = anotherKindOfindex;
      }
    }
    else {
      ppRVar2 = &(this).railRider;
      fVar8 = (float)al::getRailTotalLength((IUseRail *)ppRVar2);
      mCoinPosition[anotherKindOfindex] =
           mMoveVelocity + mCoinPosition[anotherKindOfindex];
      lVar7 = (long)anotherKindOfindex;
      fVar9 = mCoinPosition[lVar7];
      if (fVar8 < mCoinPosition[lVar7]) {
        mMoveVelocity = -mMoveVelocity;
        fVar9 = fVar8;
      }
      local_60.z = 0.0;
      local_60.x = 0.0;
      local_60.y = 0.0;
      uVar3._0_4_ = someSortOfIndex;
      uVar3._4_4_ = anotherKindOfindex;
      if ((int)uVar3 <= anotherKindOfindex) {
        do {
          al::calcRailPosAtCoord(&local_60,(IUseRail *)ppRVar2,fVar9);
          mCoinPosition[lVar7] = fVar9;
          local_70.x = local_60.x + (mDisplayOffset).x;
          local_70.y = local_60.y + (mDisplayOffset).y;
          local_70.z = local_60.z + (mDisplayOffset).z;
          fVar9 = fVar9 + -150.0;
          al::setTrans(&mCoinArray[lVar7]->actor,&local_70);
          uVar3._0_4_ = someSortOfIndex;
          uVar3._4_4_ = anotherKindOfindex;
          bVar1 = (int)uVar3 < lVar7;
          lVar7 = lVar7 + -1;
        } while (bVar1);
      }
    }
    piVar6 = &someSortOfIndex;
    piVar5 = &anotherKindOfindex;
    iVar4 = (int)(uVar3 >> 0x20);
    if ((int)uVar3 <= iVar4) {
      lVar7 = (long)(int)uVar3;
      do {
        uVar3 = Coin::isGot(mCoinArray[lVar7]);
        if ((uVar3 & 1) == 0) {
          *piVar6 = (int)lVar7;
          iVar4 = *piVar5;
          break;
        }
        iVar4 = *piVar5;
        bVar1 = lVar7 < iVar4;
        lVar7 = lVar7 + 1;
      } while (bVar1);
    }
    if (*piVar6 <= iVar4) {
      lVar7 = (long)iVar4;
      do {
        uVar3 = Coin::isGot(mCoinArray[lVar7]);
        if ((uVar3 & 1) == 0) {
          *piVar5 = (int)lVar7;
          return;
        }
        bVar1 = *piVar6 < lVar7;
        lVar7 = lVar7 + -1;
      } while (bVar1);
    }
  }
}

void CoinRail::exeCloseMove() {
    sead::Vector3f postion = sead::Vector3f::zero;
    for (s32 i = 0; i < mCoinNum; i++) {
        if (!mCoinArray[i]->isGot()) {
            mCoinPosition[i] += mMoveVelocity;
            al::calcRailPosAtCoord(&postion, this, mCoinPosition[i]);
            al::setTrans(mCoinArray[i], postion);
        }
    }
}

#include "Npc/Rabbit.h"

#include "Library/Collision/CollisionPartsKeeperUtil.h"
#include "Library/Collision/CollisionPartsTriangle.h"
#include "Library/Effect/EffectKeeper.h"
#include "Library/Effect/EffectSystemInfo.h"
#include "Library/Joint/JointControllerKeeper.h"
#include "Library/Joint/JointSpringController.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorCollisionFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Math/ParabolicPath.h"
#include "Library/Nature/NatureUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Rail/VertexGraph.h"
#include "Library/Shadow/ActorShadowUtil.h"
#include "Library/Stage/StageSwitchKeeper.h"
#include "Library/Stage/StageSwitchUtil.h"
#include "Library/Thread/FunctorV0M.h"

#include "Enemy/EnemyStateReset.h"
#include "Npc/RabbitGraph.h"
#include "Util/ItemGenerator.h"
#include "Util/ItemUtil.h"
#include "Util/PlayerUtil.h"
#include "Util/SensorMsgFunction.h"

namespace {
NERVE_IMPL(Rabbit, Reset)
NERVE_IMPL_(Rabbit, StandbyWait, Standby)
NERVE_IMPL_(Rabbit, StandbyRest, Standby)
NERVE_IMPL(Rabbit, Find)
NERVE_IMPL(Rabbit, EndTired)
NERVE_ON_END_IMPL(Rabbit, Move)
NERVE_IMPL_(Rabbit, Jump, Move)
NERVE_IMPL(Rabbit, Wait)
NERVE_IMPL_(Rabbit, Rest, Wait)
NERVE_IMPL_(Rabbit, WaitTired, Wait)
NERVE_IMPL(Rabbit, Provoke)
NERVE_IMPL(Rabbit, Break)
NERVE_IMPL(Rabbit, Turn)
NERVE_IMPL(Rabbit, TurnReverse)
NERVE_IMPL(Rabbit, MoveStart)
NERVE_IMPL(Rabbit, JumpPath)
NERVE_IMPL(Rabbit, EndJump)
NERVE_IMPL_(Rabbit, CatchToGiveMoon, Catch)
NERVE_IMPL_(Rabbit, CatchToGiveItem, Catch)
NERVE_IMPL(Rabbit, GiveMoon)
NERVE_IMPL(Rabbit, GiveItem)
NERVE_IMPL(Rabbit, Disappear)

NERVES_MAKE_STRUCT(Rabbit, StandbyWait, Reset, Move, Jump, Turn, TurnReverse, Wait, Rest, Provoke,
                   EndTired, JumpPath, MoveStart, Break, Find, WaitTired, StandbyRest,
                   CatchToGiveMoon, CatchToGiveItem, EndJump, GiveMoon, GiveItem, Disappear)
}  // namespace

Rabbit::Rabbit(const char* name, const al::Graph* graph, bool isRabbitGraphMoon)
    : al::LiveActor(name), mGraph(graph), mIsRabbitGraphMoon{isRabbitGraphMoon} {}

void Rabbit::init(const al::ActorInitInfo& initInfo) {
    using RabbitFunctor = al::FunctorV0M<Rabbit*, void (Rabbit::*)()>;

    al::initActorWithArchiveName(this, initInfo, "Rabbit", mIsRabbitGraphMoon ? "Moon" : nullptr);
    vertexA = (RabbitGraphVertex*)al::findNearestPosVertex(mGraph, al::getTrans(this), -1.0f);
    al::tryGetArg(&mMoveType, initInfo, "MoveType");
    al::tryGetArg(&mAppearItemNum, initInfo, "AppearItemNum");
    al::tryGetArg(&mIsEnableAutoUpdateShadowMaskLength, initInfo,
                  "IsEnableAutoUpdateShadowMaskLength");
    al::tryGetArg(&mIsDisableCatchByBindPlayer, initInfo, "IsDisableCatchByBindPlayer");
    s32 linkChildNum = al::calcLinkChildNum(initInfo, "RabbitDestination");
    if (0 < linkChildNum) {
        mDestinations.allocBuffer(linkChildNum, nullptr);
        for (s32 i = 0; i < linkChildNum; i++) {
            sead::Vector3f linkpos;
            al::getChildLinkT(&linkpos, initInfo, "RabbitDestination", i);
            RabbitGraphVertex* selected = nullptr;
            f32 minLength = sead::Mathf::maxNumber();
            for (s32 e = 0; e < mGraph->getVertexCount(); e++) {
                RabbitGraphVertex* vertex = (RabbitGraphVertex*)mGraph->getVertex(e);
                if (!vertex->getBool2()) {
                    sead::Vector3f diff = linkpos;
                    diff -= vertex->getPos();
                    f32 size = diff.length();
                    if (size < minLength) {
                        selected = vertex;
                        minLength = size;
                    }
                }
            }
            if (selected != nullptr)
                mDestinations.pushBack(selected);
        }
    }
    s32 itemType = rs::getItemType(initInfo);
    s32 itemNum = mAppearItemNum;
    if (itemType == 0x11) {
        mItemGenerator = new ItemGenerator();
        mItemGenerator->initNoLinkShine(this, initInfo, true);
    } else {
        rs::tryInitItem(this, itemType, initInfo, false);
    }
    mAppearItemId = itemType;
    if (itemType != 0)
        itemNum = 1;
    mAppearItemNum = itemNum;
    al::startActionAtRandomFrame(this, "Wait");
    mParabolicPath = new al::ParabolicPath();
    al::initJointControllerKeeper(this, 5);
    mJointSpringArray.allocBuffer(4, nullptr);
    al::JointSpringController* spring = al::initJointSpringController(this, "EarL1");
    spring->setStability(0.3f);
    spring->setFriction(0.6f);
    spring->setLimitDegree(45.0f);
    mJointSpringArray.pushBack(spring);
    al::JointSpringController* spring1 = al::initJointSpringController(this, "EarL2");
    spring1->setStability(0.3f);
    spring1->setFriction(0.6f);
    spring1->setLimitDegree(45.0f);
    mJointSpringArray.pushBack(spring1);
    al::JointSpringController* spring2 = al::initJointSpringController(this, "EarR1");
    spring2->setStability(0.3f);
    spring2->setFriction(0.6f);
    spring2->setLimitDegree(45.0f);
    mJointSpringArray.pushBack(spring2);
    al::JointSpringController* spring3 = al::initJointSpringController(this, "EarR2");
    spring3->setStability(0.3f);
    spring3->setFriction(0.6f);
    spring3->setLimitDegree(45.0f);
    mJointSpringArray.pushBack(spring3);
    al::initJointLocalZRotator(this, &floatA, "Spine");
    mShadowDropLength = al::getShadowMaskDropLength(this, "Hip");

    auto asd = RabbitFunctor(this, &Rabbit::appearReset);
    if (!al::listenStageSwitchOnOff(this, "SwitchRabbitAppear",
                                    RabbitFunctor(this, &Rabbit::resetParam),
                                    RabbitFunctor(this, &Rabbit::kill))) {
        al::initNerve(this, &NrvRabbit.StandbyWait, 0);
    } else {
        mEnemyStateReset = new EnemyStateReset(this, initInfo, nullptr);
        al::initNerve(this, &NrvRabbit.StandbyWait, 1);
        al::initNerveState(this, mEnemyStateReset, &NrvRabbit.Reset, "リセット");
        position.set(al::getTrans(this));
    }
    makeActorDead();
}

void Rabbit::attackSensor(al::HitSensor* self, al::HitSensor* other) {
    if (al::isSensorEnemyAttack(self) && !rs::sendMsgPushToMotorcycle(other, self)) {
        if (rs::sendMsgRabbitKick(other, self)) {
            someC = 30;
            hitSensor2 = other;
        }
        if (al::isNerve(this, &NrvRabbit.CatchToGiveMoon) ||
            al::isNerve(this, &NrvRabbit.CatchToGiveItem) ||
            al::isNerve(this, &NrvRabbit.GiveMoon) || al::isNerve(this, &NrvRabbit.GiveItem)) {
            rs::sendMsgPushToPlayer(other, self);
        }
        if (mOtherHitSensor != nullptr && 0 < someD) {
            if (al::getSensorHost(mOtherHitSensor) == al::getSensorHost(other))
                return;
        }
        al::sendMsgPushAndKillVelocityToTarget(this, self, other);
    }
}

bool Rabbit::receiveMsg(const al::SensorMsg* message, al::HitSensor* other, al::HitSensor* self) {
    if (al::isNerve(this, &NrvRabbit.Reset))
        return false;
    if (al::isNerve(this, &NrvRabbit.Disappear))
        return false;
    if (!al::isNerve(this, &NrvRabbit.CatchToGiveMoon) &&
        !al::isNerve(this, &NrvRabbit.CatchToGiveItem) && !al::isNerve(this, &NrvRabbit.GiveMoon) &&
        !al::isNerve(this, &NrvRabbit.GiveItem) && al::isSensorName(self, "Attack") &&
        al::tryReceiveMsgPushAndAddVelocityH(this, message, other, self, 5.0f)) {
        return true;
    }
    if (al::isMsgExplosion(message)) {
        mOtherHitSensor = other;
        someD = 0x78;
        return false;
    }
    if (!rs::isMsgCapAttack(message) && !rs::isMsgHosuiAttack(message) &&
        !al::isMsgKickStoneAttack(message) && !rs::isMsgYoshiTongueAttack(message) &&
        !rs::isMsgSeedAttack(message) && !rs::isMsgRadishAttack(message) &&
        !rs::isMsgGamaneBullet(message) && !rs::isMsgMayorItemReflect(message) &&
        !rs::isMsgHammerBrosHammerEnemyAttack(message) &&
        !rs::isMsgHammerBrosHammerHackAttack(message) && !rs::isMsgFireDamageAll(message)) {
        if ((!al::isMsgPlayerItemGet(message) && !rs::isMsgPlayerRabbitGet(message) &&
             !rs::isMsgPlayerAndCapHipDropAll(message) && !al::isMsgPlayerTrampleReflect(message) &&
             !rs::isMsgHackAttack(message)) ||
            (!al::isNerve(this, &NrvRabbit.StandbyWait) &&
             !al::isNerve(this, &NrvRabbit.StandbyRest) && !al::isNerve(this, &NrvRabbit.Find) &&
             !al::isNerve(this, &NrvRabbit.Turn) && !al::isNerve(this, &NrvRabbit.MoveStart) &&
             !al::isNerve(this, &NrvRabbit.Move) && !al::isNerve(this, &NrvRabbit.Break) &&
             !al::isNerve(this, &NrvRabbit.EndJump) && !al::isNerve(this, &NrvRabbit.Provoke) &&
             !al::isNerve(this, &NrvRabbit.Jump) && !al::isNerve(this, &NrvRabbit.JumpPath) &&
             !al::isNerve(this, &NrvRabbit.Wait) && !isNerve(this, &NrvRabbit.WaitTired) &&
             !al::isNerve(this, &NrvRabbit.Rest))) {
            return false;
        }
        if (mIsDisableCatchByBindPlayer && rs::isPlayerBinding(this))
            return false;
        if (!someBools) {
            rs::requestHitReactionToAttacker(message, self, other);
            someBools = true;
            al::startHitReaction(this, "接触");
            if ((al::isNerve(this, &NrvRabbit.Jump) || al::isNerve(this, &NrvRabbit.JumpPath)) &&
                al::tryStartActionIfNotPlaying(this, "SwoonStart")) {
                al::setActionFrameRate(this, 1.0f);
            }
        }
        otherHitSensor = other;
        if (al::isNerve(this, &NrvRabbit.StandbyWait) ||
            al::isNerve(this, &NrvRabbit.StandbyRest) || al::isNerve(this, &NrvRabbit.Find) ||
            al::isNerve(this, &NrvRabbit.Turn) || al::isNerve(this, &NrvRabbit.MoveStart) ||
            al::isNerve(this, &NrvRabbit.Move) || al::isNerve(this, &NrvRabbit.Break) ||
            al::isNerve(this, &NrvRabbit.EndJump) || al::isNerve(this, &NrvRabbit.Provoke) ||
            al::isNerve(this, &NrvRabbit.Wait) || al::isNerve(this, &NrvRabbit.WaitTired) ||
            al::isNerve(this, &NrvRabbit.Rest)) {
            al::Nerve* nerve = &NrvRabbit.CatchToGiveItem;
            if (mAppearItemId == 0x11)
                nerve = &NrvRabbit.CatchToGiveMoon;

            al::setNerve(this, nerve);
        }
        return true;
    }
    if ((hitSensor2 != nullptr && hitSensor2 == other && someC >= 1) ||
        al::isActionPlaying(this, "SwoonStart") ||
        (al::isNerve(this, &NrvRabbit.Move) && 5 >= someA)) {
        return true;
    }
    if (!al::isNerve(this, &NrvRabbit.StandbyWait) && !al::isNerve(this, &NrvRabbit.StandbyRest) &&
        !al::isNerve(this, &NrvRabbit.Find) && !al::isNerve(this, &NrvRabbit.Turn) &&
        !al::isNerve(this, &NrvRabbit.MoveStart) && !al::isNerve(this, &NrvRabbit.Move) &&
        !al::isNerve(this, &NrvRabbit.Break) && !al::isNerve(this, &NrvRabbit.EndJump) &&
        !al::isNerve(this, &NrvRabbit.Provoke) && !al::isNerve(this, &NrvRabbit.Jump) &&
        !al::isNerve(this, &NrvRabbit.JumpPath) && !al::isNerve(this, &NrvRabbit.Wait) &&
        !al::isNerve(this, &NrvRabbit.WaitTired) && !al::isNerve(this, &NrvRabbit.Rest)) {
        if (!al::isNerve(this, &NrvRabbit.CatchToGiveMoon) &&
            !al::isNerve(this, &NrvRabbit.CatchToGiveItem) &&
            !al::isNerve(this, &NrvRabbit.GiveMoon) && !al::isNerve(this, &NrvRabbit.GiveItem)) {
            return al::isNerve(this, &NrvRabbit.EndTired);
        }
    } else {
        al::startHitReaction(this, "投げ物ヒット");
        rs::requestHitReactionToAttacker(message, self, other);
        someB = 300;
        clipA = true;
        clipB = true;
        mStamina = 1200.0f;
        if (!al::tryStartActionIfNotPlaying(this, "SwoonStart"))
            return true;
        al::setActionFrameRate(this, 1.0f);
    }
    return true;
}

void Rabbit::control() {
    if (!al::isNerve(this, &NrvRabbit.Move) && !al::isNerve(this, &NrvRabbit.Jump) &&
        !al::isNerve(this, &NrvRabbit.Turn) && !al::isNerve(this, &NrvRabbit.TurnReverse)) {
        someA = 0;
    } else {
        someA++;
    }

    if (clipA) {
        if (0 < someB)
            someB--;
        if ((al::isNerve(this, &NrvRabbit.Move) || al::isNerve(this, &NrvRabbit.Wait) ||
             al::isNerve(this, &NrvRabbit.Rest) || al::isNerve(this, &NrvRabbit.Provoke)) &&
            someB == 0) {
            clipA = false;
            clipB = false;
            al::setNerve(this, &NrvRabbit.EndTired);
            return;
        }
    }

    if (0 < someC) {
        someC--;
        if (someC == 0)
            hitSensor2 = nullptr;
    }

    if (0 < someD) {
        someD--;
        if (someD == 0)
            mOtherHitSensor = nullptr;
    }

    f32 frameRate;
    if (((someB > 0 || 400.0f > mStamina) && !al::isNerve(this, &NrvRabbit.Provoke) &&
         !al::isNerve(this, &NrvRabbit.Wait) &&
         (!al::isNerve(this, &NrvRabbit.WaitTired) || al::isActionPlaying(this, "WaitSwoon"))) &&
        (!al::isNerve(this, &NrvRabbit.Rest) && !al::isNerve(this, &NrvRabbit.CatchToGiveItem) &&
         !al::isNerve(this, &NrvRabbit.CatchToGiveMoon) &&
         !al::isNerve(this, &NrvRabbit.EndTired) && !al::isNerve(this, &NrvRabbit.GiveMoon) &&
         !al::isNerve(this, &NrvRabbit.GiveItem) && !al::isNerve(this, &NrvRabbit.Disappear))) {
        if (!al::isEffectEmitting(this, "Sweat"))
            al::emitEffect(this, "Sweat", nullptr);
        if (al::isActionPlaying(this, "RunFine")) {
            al::startAction(this, someB > 0 ? "RunTiredSlow" : "RunTired");
            frameRate = 0.8f;
            al::setActionFrameRate(this, frameRate);
        } else if (someB > 0 && al::isActionPlaying(this, "RunTired")) {
            al::startAction(this, "RunTiredSlow");
            frameRate = 0.8f;
            al::setActionFrameRate(this, frameRate);
        }

    } else {
        if (al::isEffectEmitting(this, "Sweat"))
            al::deleteEffect(this, "Sweat");
        if (al::isActionPlaying(this, "RunTired") || al::isActionPlaying(this, "RunTiredSlow")) {
            al::startAction(this, "RunFine");
            frameRate = 1.0f;
            al::setActionFrameRate(this, frameRate);
        }
    }

    al::tryAddRippleSmall(this);
    if (mItemGenerator != nullptr && mItemGenerator->isShine())
        mItemGenerator->tryUpdateHintTransIfExistShine();
    if (mIsEnableAutoUpdateShadowMaskLength) {
        if (al::isOnGround(this, 0)) {
            al::setShadowMaskDropLength(this, 100.0f, "Hip");
            return;
        }
        sead::Vector3f position;
        al::calcJointOffsetPos(&position, this, "Hip", al::getShadowMaskOffset(this, "Hip"));
        al::Triangle triangle;
        sead::Vector3f poly;
        if (alCollisionUtil::getFirstPolyOnArrow(this, &poly, &triangle, position,
                                                 -sead::Vector3f::ey * mShadowDropLength, nullptr,
                                                 nullptr)) {
            f32 newLength = sead::Mathf::clampMin((poly - position).length(), 10.0f);
            al::setShadowMaskDropLength(this, newLength, "Hip");

        } else {
            al::setShadowMaskDropLength(this, mShadowDropLength, "Hip");
        }
    }
}

void Rabbit::endClipped() {
    al::LiveActor::endClipped();
    resetParam();
}

void Rabbit::kill() {
    al::LiveActor::kill();
}

void Rabbit::initItem(s32 itemId, s32 itemCount, const al::ActorInitInfo& initInfo) {
    if (itemId == 0x11) {
        mItemGenerator = new ItemGenerator();
        mItemGenerator->initNoLinkShine(this, initInfo, true);
    } else {
        rs::tryInitItem(this, itemId, initInfo, false);
    }
    mAppearItemId = itemId;
    mAppearItemNum = itemId == 0 ? itemCount : 1;
}

void Rabbit::appearReset() {
    if (al::isAlive(this))
        return;
    al::LiveActor::appear();
    al::resetPosition(this, position);
    vertexA = (RabbitGraphVertex*)al::findNearestPosVertex(mGraph, al::getTrans(this), -1.0f);
    resetParam();
    al::setNerve(this, &NrvRabbit.Reset);
}

void Rabbit::resetParam() {
    someB = 0;
    clipA = false;
    clipB = false;
    mStamina = 1200.0f;
}

void Rabbit::setNerveJumpOrMove(al::LiveActor* actor, const RabbitGraphVertex* va,
                                const RabbitGraphVertex* vb, bool isMoveStart) {
    RabbitGraphEdge* edge = (RabbitGraphEdge*)al::tryFindEdgeStartVertex(va, vb);
    s32 val = edge->getValue();
    if ((val & 0xff) == 0) {
        al::validateClipping(this);
        al::Nerve* nerve;
        if (!isMoveStart)
            nerve = &NrvRabbit.Move;
        else
            nerve = &NrvRabbit.MoveStart;
        al::setNerve(actor, nerve);
    } else {
        al::invalidateClipping(this);
        if (val < 0x100)
            al::setNerve(actor, &NrvRabbit.JumpPath);
        else
            al::setNerve(actor, &NrvRabbit.Jump);
    }
}

void Rabbit::setNerveJumpOrMoveStart(al::LiveActor* actor, const RabbitGraphVertex* va,
                                     const RabbitGraphVertex* vb) {
    setNerveJumpOrMove(actor, va, vb, true);
}

void Rabbit::onMoveEndUpdateCurrentVertexAndNextNerve() { /*
   if (!vertexB->getBool()) {
     vertexA = vertexB;
     vertexB = nullptr;
     RabbitGraphVertex * vert = (RabbitGraphVertex *)tryFindNextVertex();
     sead::vector3f playerPos = rs::getPlayerBodyPos(this);
     sead::vector3f pos = al::getTrans(this);
     if (vert != nullptr) {
       playerPos-=pos;
       f32 distance = rs::isPlayerHackTRex(this)? 3000.0f:900.0f;
       if (playerPos.length() < distance) {
         vertexB = pRVar2;
         al::calcFrontDir(&VStack_60,this);
         pRVar2 = vertexB;
         local_70.z = (pRVar2->position).z;
         local_70.x = (pRVar2->position).x;
         local_70.y = (pRVar2->position).y;
         pVVar3 = al::getTrans(this);
         local_70._0_8_ = ZEXT48((uint)(local_70.x - pVVar3->x));
         local_70.z = local_70.z - pVVar3->z;
         uVar9 = al::tryNormalizeOrZero(&local_70);
         if (((uVar9 & 1) == 0) ||
            (fVar14 = (float)al::calcAngleDegree(&local_70,&VStack_60), fVar14 <= 80.0)) {
           lVar4 = al::tryFindEdgeStartVertex(&vertexA->vertex,&vertexB->vertex);
           uVar1 = *(ushort *)(lVar4 + 0x250);
           if ((uVar1 & 0xff) == 0) {
             al::validateClipping(this);
             ppuVar5 = &PTR_PTR_exeMove_7101d628f0;
           }
           else {
             al::invalidateClipping(this);
             if (uVar1 < 0x100) {
               ppuVar5 = &PTR_PTR_exeJumpPath_7101d62930;
             }
             else {
               ppuVar5 = &PTR_PTR_exeJump_7101d628f8;
             }
           }
         }
         else {
           ppuVar5 = &PTR_PTR_exeTurnReverse_7101d62908;
           if (vertexB != pRVar8) {
             ppuVar5 = &PTR_PTR_exeTurn_7101d62900;
           }
         }
         al::setNerve(this,(Nerve *)ppuVar5);
         return;
       }
     }
     al::validateClipping(this);
     if (!al::isNerve(this,&NrvRabbit.Move)||
        (!al::isActionPlaying(this,"RunFine") &&
          !al::isActionPlaying(this,"RunTired") &&
         !al::isActionPlaying(this,"RunTiredSlow"))) {
       ppuVar5 = &PTR_PTR_exeWaitTired_7101d62950;
       if (someB < 1) {
         ppuVar5 = &PTR_PTR_exeWait_7101d62910;
       }
     }
     else {
       ppuVar5 = &PTR_PTR_exeBreak_7101d62940;
     }
   }
   else {
     if ((pRVar2->vertex).mEdges.size < 1) {
       pRVar8 = (RabbitGraphVertex *)0x0;
     }
     else {

       fVar14 = 0.0;
       uVar9 = 0;
       pRVar7 = (RabbitGraphVertex *)0x0;
       do {
         if (uVar9 < (uint)(pRVar2->vertex).mEdges.size) {
           pRVar6 = (pRVar2->vertex).mEdges.array[uVar9];
         }
         else {
           pRVar6 = (RabbitGraphVertex *)0x0;
         }
         pRVar10 = *(RabbitGraphVertex **)&(pRVar6->vertex).index;
         pRVar8 = pRVar7;
         fVar11 = fVar14;
         if ((pRVar10 != vertexA) &&
            ((RabbitGraphVertex *)(pRVar6->vertex).mEdges.array == pRVar2)) {
           pVVar3 = rs::getPlayerBodyPos(this);
           fVar12 = pVVar3->y - (pRVar10->position).y;
           fVar11 = pVVar3->x - (pRVar10->position).x;
           fVar13 = pVVar3->z - (pRVar10->position).z;
           fVar11 = SQRT(fVar11 * fVar11 + fVar12 * fVar12 + fVar13 * fVar13);
           pRVar2 = vertexB;
           pRVar8 = pRVar10;
           if (fVar11 <= fVar14) {
             pRVar8 = pRVar7;
             fVar11 = fVar14;
           }
         }
         fVar14 = fVar11;
         uVar9 = uVar9 + 1;
         pRVar7 = pRVar8;
       } while ((long)uVar9 < (long)(pRVar2->vertex).mEdges.size);
     }
     setNerveJumpOrMove(this, vertexA, vertexB, false);
   }
   al::setNerve((IUseNerve *)this,(Nerve *)ppuVar5);*/
}

void Rabbit::tryFindNextVertex() {}

void Rabbit::fall(f32 velocity) {
    if (al::isOnGround(this, 0)) {
        al::addVelocityToDirection(this, -al::getCollidedGroundNormal(this), velocity);
        al::scaleVelocity(this, 0.5f);
        return;
    }
    al::addVelocityToGravity(this, velocity);
    al::scaleVelocity(this, 0.998f);
}

void Rabbit::reduceStamina() {
    if (someB == 0) {
        mStamina = sead::Mathf::max(mStamina + -1.0f, 0.0f);
        if (mStamina == 0.0) {
            clipA = true;
            someB = 300;
            mStamina = 1200.0f;
        }
    }
}

void Rabbit::trySetPoseGraphMoveDir(f32 delay) {
    if (vertexA == nullptr || vertexB == nullptr)
        return;

    sead::Vector3f position = vertexB->getPos();
    position -= al::getTrans(this);
    f32 length = sead::Mathf::clamp(position.length() / 500.0f, 0.0f, 1.0f);
    position.y = 0.0f;

    if (!al::tryNormalizeOrZero(&position))
        return;

    if (1.0f - length > 0.0f && vertexB->getEdgeCount() == 4) {
        RabbitGraphVertex* selected = nullptr;
        for (s32 i = 0; i < vertexB->getEdgeCount(); i++) {
            RabbitGraphVertex* v2 = (RabbitGraphVertex*)vertexB->getEdge(i)->getVertex2();
            if (v2 != vertexB && v2 != vertexA)
                selected = v2;
        }
        sead::Vector3f nipon = selected->getPos();
        nipon -= vertexB->getPos();
        nipon.y = 0;
        if (al::tryNormalizeOrZero(&nipon)) {
            f32 fVar11 = (1.0f - length) * 0.5f;
            position = fVar11 * nipon + (1.0f - fVar11) * position;
        }
    }

    if (!al::isParallelDirection(position, sead::Vector3f::ey)) {
        sead::Quatf quat;
        al::makeQuatFrontUp(&quat, position, sead::Vector3f::ey);
        al::slerpQuat(al::getQuatPtr(this), al::getQuat(this), quat, delay);
    }
}

inline f32 getJumpSpeed(bool mIsRabbitGraphMoon, s32 someB) {
    return !mIsRabbitGraphMoon ? 25.0f : someB > 0 ? 12.0f : 20.0f;
}

f32 Rabbit::getMoveSpeed() const {
    if (al::isNerve(this, &NrvRabbit.Jump) || al::isNerve(this, &NrvRabbit.JumpPath))
        return getJumpSpeed(mIsRabbitGraphMoon, someB);

    f32 speed = someB > 0 ? 12.0f : mIsRabbitGraphMoon ? 20.0f : 25.0f;
    f32 moddifier = sead::Mathf::clamp(someA / 30.0f, 0.0f, 1.0f);
    return speed * moddifier;
}

void Rabbit::exeReset() {
    if (al::updateNerveState(this))
        al::setNerve(this, &NrvRabbit.StandbyWait);
}

void Rabbit::exeStandby() {
    if (al::isFirstStep(this)) {
        if (!al::isNerve(this, &NrvRabbit.StandbyWait)) {
            al::startAction(this, "Rest");
        } else {
            randomWait = al::getRandom(0x1e0, 0xf0);
            al::tryStartActionIfNotPlaying(this, "Wait");
        }
    }
    fall(0.98f);
    if (vertexA->getBool())
        al::resetPosition(this, vertexA->getPos());
    sead::Vector3f position = rs::getPlayerBodyPos(this);
    position -= al::getTrans(this);
    f32 len = position.length();
    f32 uVar2 = rs::isPlayerHackTRex(this) ? 3000.0f : 1500.0f;
    if (len < uVar2) {
        al::setNerve(this, &NrvRabbit.Find);
    } else if (!clipA) {
        if (!al::isNerve(this, &NrvRabbit.StandbyWait)) {
            if (!al::isGreaterStep(this, 0xb4))
                return;
            al::setNerve(this, &NrvRabbit.StandbyWait);
        } else {
            if (!al::isGreaterStep(this, randomWait))
                return;
            al::setNerve(this, &NrvRabbit.StandbyRest);
        }
    } else {
        al::setNerve(this, &NrvRabbit.WaitTired);
    }
}

void Rabbit::exeFind() {
    if (al::isFirstStep(this))
        al::startAction(this, "Find");

    fall(0.98);
    if (vertexA->getBool())
        al::resetPosition(this, vertexA->getPos());
    al::turnToTarget(this, rs::getPlayerPos(this), 10.0f);

    if (al::isActionEnd(this)) {
        al::Nerve* nerve = &NrvRabbit.Wait;
        if (someB > 0)
            nerve = &NrvRabbit.WaitTired;
        al::setNerve(this, nerve);
    }
}

void Rabbit::exeEndTired() {
    if (al::isFirstStep(this))
        al::startAction(this, "EndTired");
    fall(0.98f);
    if (al::isActionEnd(this)) {
        if (vertexB == nullptr)
            al::setNerve(this, &NrvRabbit.Wait);
        else
            setNerveJumpOrMove(this, vertexA, vertexB, false);
    }
}

void Rabbit::exeMove() {}

void Rabbit::exeWait() {}

void Rabbit::exeProvoke() {}

void Rabbit::exeBreak() {
    if (al::isFirstStep(this))
        al::startAction(this, "Break");

    fall(0.98f);

    if (al::isActionEnd(this)) {
        al::Nerve* nerve = &NrvRabbit.Wait;
        if (someB > 0)
            nerve = &NrvRabbit.WaitTired;
        al::setNerve(this, nerve);
    }
}

void Rabbit::exeTurn() {
    if (al::isFirstStep(this))
        al::startAction(this, someB > 0 ? "TurnTired" : "Turn");

    fall(0.01f);
    trySetPoseGraphMoveDir(0.1f);

    if (al::isGreaterEqualStep(this, 15))
        setNerveJumpOrMove(this, vertexA, vertexB, false);
}

void Rabbit::exeTurnReverse() {
    if (al::isFirstStep(this)) {
        al::startAction(this, someB > 0 ? "TurnTired" : "Turn");
        al::calcQuat(&quat, this);

        sead::Vector3f diff = vertexB->getPos();
        sead::Vector3f frontDir;
        diff -= al::getTrans(this);
        al::tryNormalizeOrZero(&diff);
        al::calcFrontDir(&frontDir, this);
        bloat = al::calcAngleDegree(frontDir, diff);
        f32 cc = diff.z;
        if (frontDir.z * diff.x - cc * frontDir.x > 0.0f)  // mismatch here
            bloat = 360.0f - bloat;
    }
    fall(0.01f);

    const char* action = al::isActionPlaying(this, "Turn") ? "Turn" : "TurnTired";
    f32 fVar9 = al::calcNerveEaseInOutRate(this, al::getActionFrameMax(this, action));
    al::rotateQuatYDirDegree(al::getQuatPtr(this), quat, -bloat * fVar9);

    if (al::isActionEnd(this))
        setNerveJumpOrMove(this, vertexA, vertexB, false);
}

void Rabbit::exeMoveStart() {
    if (al::isFirstStep(this))
        al::startAction(this, "RunStart");
    fall(0.01f);
    trySetPoseGraphMoveDir(0.1f);
    if (al::isActionEnd(this))
        al::setNerve(this, &NrvRabbit.Move);
}

void Rabbit::exeJumpPath() {}

void Rabbit::exeEndJump() {
    if (al::isFirstStep(this)) {
        if (al::isActionPlaying(this, "JumpStart") || al::isActionPlaying(this, "JumpLoop"))
            al::startAction(this, "JumpEnd");
        else if (al::isActionPlaying(this, "SwoonStart") || al::isActionPlaying(this, "SwoonLoop"))
            al::startAction(this, "SwoonLand");
        al::startHitReaction(this, "着地");
        vertexA = vertexB;
        vertexB = nullptr;
    }
    mStamina = sead::Mathf::min(mStamina + 0.05f, 1200.0f);
    fall(0.98);
    if (vertexA->getBool())
        al::resetPosition(this, vertexA->getPos());
    if (al::isActionEnd(this)) {
        al::Nerve* nerve = &NrvRabbit.Wait;
        if (someB > 0)
            nerve = &NrvRabbit.WaitTired;
        al::setNerve(this, nerve);
    }
}

void Rabbit::exeCatch() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "Catch");
        al::setVelocityZero(this);
    }
    if (al::isActionEnd(this)) {
        al::Nerve* nerve = &NrvRabbit.GiveItem;
        if (al::isNerve(this, &NrvRabbit.CatchToGiveMoon))
            nerve = &NrvRabbit.GiveMoon;
        al::setNerve(this, nerve);
    }
}

void Rabbit::exeGiveMoon() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "Appear");
        sead::Vector3f napa;
        sead::Quatf nipon;
        al::calcQuatFront(&napa, this);
        al::makeQuatUpFront(&nipon, sead::Vector3f::ey, napa);
        al::slerpQuat(al::getQuatPtr(this), al::getQuat(this), nipon, 1.0f);
        al::setVelocityZero(this);
    }
    if (al::isStep(this, 30) && mItemGenerator != nullptr && mItemGenerator->isShine()) {
        mItemGenerator->generate(al::getTrans(this) + sead::Vector3f(0.0f, 250.0f, 0.0f),
                                 al::getQuat(this));
        al::startHitReaction(this, "シャイン出現");
    }
    if (al::isActionEnd(this))
        al::setNerve(this, &NrvRabbit.Disappear);
}

void Rabbit::exeGiveItem() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "Appear");
        sead::Vector3f napa;
        sead::Quatf nipon;
        al::calcQuatFront(&napa, this);
        al::makeQuatUpFront(&nipon, sead::Vector3f::ey, napa);
        al::slerpQuat(al::getQuatPtr(this), al::getQuat(this), nipon, 1.0f);
        al::setVelocityZero(this);
    }

    if (al::isLessStep(this, 0x1e))
        return;

    if (mItemCount != mAppearItemNum) {
        if (rs::tryAppearMultiCoinFromObj(this, otherHitSensor, al::getNerveStep(this), 200.0f)) {
            const char* reaction = "アイテム出現";
            if (mAppearItemId == 0xc)
                reaction = "ライフアップアイテム出現";
            if (mAppearItemId == 0)
                reaction = "コイン出現";
            al::startHitReaction(this, reaction);
            mItemCount++;
        }
        return;
    }

    if (al::isActionEnd(this))
        al::setNerve(this, &NrvRabbit.Disappear);
}

void Rabbit::exeDisappear() {
    if (al::isFirstStep(this))
        al::startAction(this, "Disappear");
    if (al::isActionEnd(this)) {
        al::onStageSwitch(this, "SwitchCatchOn");
        kill();
    }
}

void Rabbit::onMoveEnd() {
    if (!al::isActionPlaying(this, "RunTired") && !al::isActionPlaying(this, "RunTiredSlow"))
        al::setActionFrameRate(this, 1.0f);
}

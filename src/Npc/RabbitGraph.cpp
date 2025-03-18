#include "Npc/RabbitGraph.h"

#include "Library/Collision/PartsConnectorUtil.h"
#include "Library/Collision/PartsMtxConnector.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorInitFunction.h"
#include "Library/LiveActor/ActorInitInfo.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Placement/PlacementInfo.h"
#include "Library/Rail/RailRider.h"
#include "Library/Rail/RailUtil.h"

#include "Npc/Rabbit.h"
#include "Npc/RabbitGraphRailKeeper.h"
#include "Util/PlayerUtil.h"

namespace {
NERVE_IMPL(RabbitGraph, Standby)

NERVES_MAKE_STRUCT(RabbitGraph, Standby)
}  // namespace

s32 RabbitGraphVertex::calcValidEdgeNum() const {
    s32 validEdges = 0;
    for (s32 i = 0; i < getEdgeCount(); i++) {
        RabbitGraphEdge* edge = (RabbitGraphEdge*)getEdge(i);
        if (edge->isValid())
            validEdges++;
    }
    return validEdges;
}

RabbitGraphEdge::RabbitGraphEdge(RabbitGraphVertex* vertexA, RabbitGraphVertex* vertexB)
    : PosEdge(vertexA, vertexB) {}

bool RabbitGraphEdge::isValid() const {
    if (!_244)
        return false;

    for (s32 i = 0; i < mActors.size(); i++)
        if (al::isAlive(*mActors[i]))
            return true;

    return false;
}

f32 RabbitGraphEdge::getWeight() const {
    if (!_244)
        return sead::Mathf::maxNumber();

    for (s32 i = 0; i < mActors.size(); i++) {
        if (!al::isAlive(*mActors[i]))
            continue;

        sead::Vector3f bodyPos = rs::getPlayerBodyPos(mParent);
        f32 distance = al::calcDistanceAndNearestPos(nullptr, this, bodyPos);
        f32 weight = al::Graph::PosEdge::getWeight();

        weight += 20000.0f * sead::Mathf::clamp(500.0f - distance, 0.0f, 1.0f);

        if (getVertex2()->getPos().y - getVertex1()->getPos().y > 1000.0f)
            return weight + 200000.0f;

        return weight;
    }
    return sead::Mathf::maxNumber();
}

f32 RabbitGraphEdgeMoon::getWeight() const {
    if (!_244)
        return sead::Mathf::maxNumber();

    for (s32 i = 0; i < mActors.size(); i++) {
        if (!al::isAlive(*mActors[i]))
            continue;

        sead::Vector3f bodyPos = rs::getPlayerBodyPos(mParent);
        f32 distance = al::calcDistanceAndNearestPos(nullptr, this, bodyPos);
        f32 weight = al::Graph::PosEdge::getWeight();

        weight += 20000.0f * sead::Mathf::clamp(500.0f - distance, 0.0f, 1.0f);

        if (getVertex2()->getPos().y - getVertex1()->getPos().y > 1000.0f)
            return weight + 200000.0f;

        return weight;
    }
    return sead::Mathf::maxNumber();
}

RabbitGraph::RabbitGraph(const char* name) : al::LiveActor(name) {}

void RabbitGraph::init(const al::ActorInitInfo& info) {
    al::initActorSceneInfo(this, info);
    al::initActorPoseTRSV(this);
    al::initActorSRT(this, info);
    al::initActorClipping(this, info);
    al::initExecutorUpdate(this, info, "地形オブジェ[Movement]");
    al::invalidateClipping(this);
    al::initNerve(this, &NrvRabbitGraph.Standby, 2);
    al::tryGetArg(&mGraphBezierSplitUnitLength, info, "GraphBezierSplitUnitLength");
    s32 railKeeperNum = al::calcLinkChildNum(info, "RabbitGraphRailKeeper");
    s32 railNum = al::calcLinkChildNum(info, "RailRabbit");

    if (mRailActors.capacity() < railNum + railKeeperNum) {
        kill();
        return;
    }

    bool isRabbitGraphMoon = al::isObjectName(info, "RabbitGraphMoon");
    for (s32 i = 0; i < railKeeperNum; i++) {
        al::LiveActor* actor = nullptr;
        tryCreateRabbitGraphLinks(&actor, this, info, "RabbitGraphRailKeeper", i,
                                  isRabbitGraphMoon);
        mRailActors.pushBack(&actor);
    }
    for (s32 i = 0; i < railNum; i++) {
        al::LiveActor* actor = nullptr;
        tryCreateRabbitGraphLinks(&actor, this, info, "RailRabbit", i, isRabbitGraphMoon);
        mRailActors.pushBack(&actor);
    }

    s32 totalVertexCount = 0;
    s32 extraSplitVertexCount = 0;
    for (s32 i = 0; i < mRailActors.size(); i++) {
        al::LiveActor* actor = *mRailActors[i];
        railKeeperNum += al::getRailNum(actor);

        s32 railPointNum = al::getRailPointNum(actor);
        for (s32 j = 0; j < railPointNum; j++) {
            if (i < al::getRailNum(actor) && al::isRailBezierPart(actor, i + 1)) {
                f32 distanceA = al::calcRailCoordByPoint(actor, i + 1);
                f32 distanceB = al::calcRailCoordByPoint(actor, i);
                if (i == railPointNum)
                    distanceB = al::getRailTotalLength(actor);
                f32 distance = (distanceB - distanceA) / mGraphBezierSplitUnitLength;

                extraSplitVertexCount += distance != 0 ? distance - 1 : 0;
            }
        }
        totalVertexCount += railPointNum;
    }

    if (totalVertexCount > 0) {
        for (s32 i = 0; i < mRailActors.size(); i++) {
        }
    }

    s32 rabbitNum = al::calcLinkChildNum(info, "Rabbit");
    if (rabbitNum > mRabbitActors.capacity()) {
        kill();
        return;
    }

    for (s32 i = 0; i < rabbitNum; i++) {
        al::LiveActor* actor = nullptr;
        tryCreateRabbitGraphLinks(&actor, this, info, "Rabbit", i, isRabbitGraphMoon);
        mRabbitActors.pushBack(&actor);
    }
    for (s32 i = 0; i < mRailActors.size(); i++)
        for (s32 j = 0; j < mRailActors.size(); j++) {
            al::LiveActor* actor = *mRailActors[j];

            al::appendGraphFromRail<RabbitGraphVertex, RabbitGraphEdge>(
                mGraph, actor->getRailRider()->getRail(), true, true);
            al::appendGraphFromRail<RabbitGraphVertex, RabbitGraphEdgeMoon>(
                mGraph, actor->getRailRider()->getRail(), true, true);
            al::appendGraphFromRail<al::Graph::PosVertex, al::Graph::PosEdge>(
                mGraph, actor->getRailRider()->getRail(), false, true);
        }

    for (s32 i = 0; i < mGraph->getEdgeCount(); i++) {
        RabbitGraphEdge* edge = (RabbitGraphEdge*)mGraph->getEdge(i);
        edge->calcDistance();
        edge->setParent(this);
    }
    makeActorAlive();
}

void RabbitGraph::attackSensor(al::HitSensor* self, al::HitSensor* other) {}

bool RabbitGraph::receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                             al::HitSensor* self) {
    return false;
}

void RabbitGraph::initAfterPlacement() {
    for (s32 i = 0; i < mGraph->getVertexCount(); i++) {
        RabbitGraphVertex* vertex = (RabbitGraphVertex*)mGraph->getVertex(i);
        if (!vertex->getBool())
            continue;

        sead::Quatf* quat = new sead::Quatf();
        al::MtxConnector* mtxConnector = new al::MtxConnector(*quat, vertex->getPos());

        sead::Vector3f dir = -100.0f * sead::Vector3f::ey;
        sead::Vector3f pos = vertex->getPos() + 50.0f * sead::Vector3f::ey;
        al::attachMtxConnectorToCollision(mtxConnector, this, pos, dir);
        vertex->setMtxConnector(mtxConnector);
    }
}

void RabbitGraph::control() {
    for (s32 i = 0; i < mGraph->getVertexCount(); i++) {
        RabbitGraphVertex* vertex = (RabbitGraphVertex*)mGraph->getVertex(i);
        al::MtxConnector* mtxConnector = vertex->getMtxConnector();
        if (mtxConnector)
            al::calcConnectTrans(vertex->getPosPtr(), mtxConnector, mtxConnector->getBaseTrans());
    }
}

bool RabbitGraph::tryCreateRabbitGraphLinks(al::LiveActor** actor, al::LiveActor* parent,
                                            const al::ActorInitInfo& info, const char* name,
                                            int index, bool isRabbitGraphMoon) {
    al::PlacementInfo placementInfo;
    al::ActorInitInfo initInfo;
    initInfo.initViewIdHost(&placementInfo, info);

    if (al::isEqualString("RabbitGraphRailKeeper", name)) {
        if (al::calcLinkChildNum(info, "RabbitGraphRailKeeper") == 0)
            return false;

        al::getLinksInfoByIndex(&placementInfo, info, name, index);
        *actor = new RabbitGraphRailKeeper("ウサギグラフ用レール保持", parent);
        (*actor)->init(initInfo);
        return true;
    }

    if (al::isEqualString("RailRabbit", name)) {
        if (al::calcLinkChildNum(info, "RailRabbit") == 0)
            return false;
        al::getLinksInfoByIndex(&placementInfo, info, name, index);
        *actor = new RabbitGraphRailKeeper("ウサギグラフ用レール保持(直接)", parent);
        (*actor)->init(initInfo);
        return true;
    }

    if (al::isEqualString("Rabbit", name)) {
        al::calcLinkChildNum(info, "Rabbit");
        al::getLinksInfoByIndex(&placementInfo, info, name, index);
        *actor = new Rabbit(isRabbitGraphMoon ? "月ウサギ" : "ウサギ", mGraph, isRabbitGraphMoon);
        (*actor)->init(initInfo);
        (*actor)->makeActorAlive();
        return true;
    }
    return false;
}

void RabbitGraph::exeStandby() {}

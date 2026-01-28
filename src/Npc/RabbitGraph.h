#pragma once

#include <container/seadPtrArray.h>
#include <math/seadVector.h>

#include "Library/LiveActor/LiveActor.h"
#include "Library/Rail/VertexGraph.h"

namespace al {
struct ActorInitInfo;
class HitSensor;
class SensorMsg;
class MtxConnector;
}  // namespace al

class RabbitGraphVertex : public al::Graph::PosVertex {
public:
    RabbitGraphVertex(s32 index, const sead::Vector3f& pos)
        : al::Graph::PosVertex(index, pos, 16) {}

    s32 calcValidEdgeNum() const;

    bool getBool2() const { return _230; }

    bool getBool1() const { return _231; }

    bool getBool() const { return _232; }

    al::MtxConnector* getMtxConnector() const { return mMtxConnector; }

    void setMtxConnector(al::MtxConnector* mtxConnector) { mMtxConnector = mtxConnector; }

private:
    sead::FixedPtrArray<al::LiveActor*, 0x40> mActors;
    bool _230 = false;
    bool _231 = false;
    bool _232 = false;
    al::MtxConnector* mMtxConnector = nullptr;
};

static_assert(sizeof(RabbitGraphVertex) == 0x240);

class RabbitGraphEdge : public al::Graph::PosEdge {
public:
    RabbitGraphEdge(RabbitGraphVertex* vertexA, RabbitGraphVertex* vertexB);

    f32 getWeight() const override;
    void calcDistance(){
        mDistance = (getVertex2()->getPos() - getVertex1()->getPos()).length();
    }

    bool isValid() const;

    RabbitGraphVertex* getVertex1() const { return static_cast<RabbitGraphVertex*>(Edge::getVertex1()); }

    RabbitGraphVertex* getVertex2() const { return static_cast<RabbitGraphVertex*>(Edge::getVertex2()); }

    u16 getValue() const { return _250; }

    void setParent(al::LiveActor* parent){ mParent=parent;}

protected:
    sead::FixedPtrArray<al::LiveActor*, 0x40> mActors;
    f32 mDistance = 0.0f;
    bool _244 = true;
    al::LiveActor* mParent = nullptr;
    u16 _250 = 0;
};

static_assert(sizeof(RabbitGraphEdge) == 0x258);

class RabbitGraphEdgeMoon : public RabbitGraphEdge {
public:
    RabbitGraphEdgeMoon(RabbitGraphVertex* vertexA, RabbitGraphVertex* vertexB)
        : RabbitGraphEdge(vertexA, vertexB) {}

    f32 getWeight() const override;
};

static_assert(sizeof(RabbitGraphEdgeMoon) == 0x258);

class RabbitGraph : public al::LiveActor {
public:
    RabbitGraph(const char*);

    void init(const al::ActorInitInfo& info) override;
    void attackSensor(al::HitSensor* self, al::HitSensor* other) override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;
    void initAfterPlacement() override;
    void control() override;

    bool tryCreateRabbitGraphLinks(al::LiveActor**, al::LiveActor*, const al::ActorInitInfo&,
                                   const char*, int, bool);
    void exeStandby();

private:
    sead::FixedPtrArray<al::LiveActor*, 32> mRailActors;
    al::Graph* mGraph = nullptr;
    sead::FixedPtrArray<al::LiveActor*, 10> mRabbitActors;
    float mGraphBezierSplitUnitLength = 100.0f;
};

static_assert(sizeof(RabbitGraph) == 0x288);

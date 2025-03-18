#pragma once

#include <container/seadPtrArray.h>
#include <math/seadQuat.h>
#include <math/seadVector.h>

#include "Library/LiveActor/LiveActor.h"

namespace al {
struct ActorInitInfo;
class HitSensor;
class JointSpringController;
class SensorMsg;
class ParabolicPath;
class Graph;
}  // namespace al

class EnemyStateReset;
class ItemGenerator;
class RabbitGraphVertex;

class Rabbit : public al::LiveActor {
public:
    Rabbit(const char*, const al::Graph*, bool isRabbitGraphMoon);
    void init(const al::ActorInitInfo& info) override;
    void attackSensor(al::HitSensor* self, al::HitSensor* other) override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;
    void control() override;
    void endClipped() override;
    void kill() override;

    void initItem(s32, s32, const al::ActorInitInfo&);
    void appearReset();
    void resetParam();
    void setNerveJumpOrMove(al::LiveActor*, const RabbitGraphVertex*, const RabbitGraphVertex*,
                            bool);
    void setNerveJumpOrMoveStart(al::LiveActor*, const RabbitGraphVertex*,
                                 const RabbitGraphVertex*);
    void onMoveEndUpdateCurrentVertexAndNextNerve();
    void tryFindNextVertex();
    void fall(f32 velocity);
    void reduceStamina();
    void trySetPoseGraphMoveDir(f32);
    f32 getMoveSpeed() const;

    void exeReset();
    void exeStandby();
    void exeFind();
    void exeEndTired();
    void exeMove();
    void exeWait();
    void exeProvoke();
    void exeBreak();
    void exeTurn();
    void exeTurnReverse();
    void exeMoveStart();
    void exeJumpPath();
    void exeEndJump();
    void exeCatch();
    void exeGiveMoon();
    void exeGiveItem();
    void exeDisappear();

    void onMoveEnd();

private:
    const al::Graph* mGraph = nullptr;
    RabbitGraphVertex* vertexA = nullptr;
    RabbitGraphVertex* vertexB = nullptr;
    RabbitGraphVertex* vertexC = nullptr;
    s32 mMoveType = 2;

    ItemGenerator* mItemGenerator = nullptr;
    s32 mItemCount = 0;
    s32 mAppearItemNum = 1;
    s32 mAppearItemId = -1;

    al::HitSensor* otherHitSensor = nullptr;
    sead::PtrArray<RabbitGraphVertex> mDestinations;
    al::ParabolicPath* mParabolicPath = nullptr;
    f32 bloatA = 0.0f;
    bool mIsRabbitGraphMoon = false;
    bool clipA = false;

    s32 someB = 0;
    float mStamina = 1200.0f;
    bool someBools = false;

    int randomWait = 0xf0;
    sead::PtrArray<al::JointSpringController> mJointSpringArray;
    sead::Quatf quat = sead::Quatf::unit;
    float bloat = 0.0f;
    s32 someA = 0;
    bool clipB = false;

    //

    al::HitSensor* hitSensor2 = nullptr;
    s32 someC = 0;
    f32 mShadowDropLength = 100.0f;
    bool mIsEnableAutoUpdateShadowMaskLength = false;
    f32 floatA = 0.0f;
    EnemyStateReset* mEnemyStateReset = nullptr;
    al::HitSensor* mOtherHitSensor = nullptr;
    s32 someD = 0;
    sead::Vector3f position = sead::Vector3f::zero;
    bool mIsDisableCatchByBindPlayer = false;
};

// static_assert(sizeof(Rabbit) == 0x1b8);

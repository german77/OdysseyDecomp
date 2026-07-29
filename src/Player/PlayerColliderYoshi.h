#pragma once

#include <math/seadVector.h>

#include "Library/HostIO/HioNode.h"
#include "Player/IUsePlayerCeilingCheck.h"
#include "Player/IUsePlayerCollision.h"
#include "Player/IUsePlayerHeightCheck.h"

class CollisionShapeKeeper;
class PlayerCeilingCheck;
class PlayerCollider;

class PlayerColliderYoshi : public al::HioNode,
                            public IUsePlayerCollision,
                            public IUsePlayerCeilingCheck,
                            public IUsePlayerHeightCheck {
public:
    PlayerColliderYoshi(al::LiveActor*);

    void init();
    void tryChangeCollisionNormal();
    void updateCollider(const sead::Vector3f&);
    void updateHeightCheck(const sead::Vector3f&, const sead::Vector3f&);
    void updateCeilingCheck(const sead::Vector3f&, const sead::Vector3f&, float);
    void tryChangeCollisionTongueJump();
    bool isEnableStandUp() const override;
    bool isEnableHoldUp() const override;
    bool isPressedCeil() const override;
    f32 getSafetyCeilSpace() const override;
    f32 getCeilCheckHeight() const override;
    PlayerCollider* getPlayerCollider() const override;
    bool isAboveGround() const override;
    f32 getGroundHeight() const override;
    f32 getShadowDropHeight() const override;

private:
    al::LiveActor* mActor;
    PlayerCollider* mPlayerCollider;
    PlayerCeilingCheck* mPlayerCeilingCheck;
    CollisionShapeKeeper* _30;
    CollisionShapeKeeper* _38;
    CollisionShapeKeeper* _40;
    bool _48;
    f32 _4c;
    f32 _50;
};

static_assert(sizeof(PlayerColliderYoshi) == 0x58);

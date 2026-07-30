#pragma once

#include "Library/HostIO/HioNode.h"

#include "Player/IPlayerModelChanger.h"

namespace al {
class LiveActor;
}  // namespace al

class PlayerModelHolder;

class PlayerModelChangerYoshi : public al::HioNode, public IPlayerModelChanger {
public:
    PlayerModelChangerYoshi(const al::LiveActor*, PlayerModelHolder*);

    void syncHost();
    void syncModelFlag(al::LiveActor*);
    void appearModel();
    void killModel();
    void resetPosition() override;
    void hideModel() override;
    void hideSilhouette() override;
    void showModel() override;
    void showSilhouette() override;
    bool isHiddenModel() const override;
    void changeModel(al::LiveActor*);
    bool isFireFlower() const override;
    bool isMini() const override;
    bool isChange() const override;
    bool is2DModel() const override;
    bool isHiddenShadowMask() const override;
    void hideShadowMask() override;
    void showShadowMask() override;

private:
    al::LiveActor* mActor;
    al::LiveActor* mModelActor;
    PlayerModelHolder* mModelHolder;
    bool _20;
    bool _21;
    bool _22;
};

static_assert(sizeof(PlayerModelChangerYoshi) == 0x28);

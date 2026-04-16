#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al {
struct ActorInitInfo;
class LayoutInitInfo;
class LiveActor;
class Scene;
}  // namespace al

class Closet;

class StageSceneStateCloset : public al::HostStateBase<al::Scene> {
public:
    static StageSceneStateCloset* tryCreate(al::Scene*, const al::ActorInitInfo&,
                                            const al::LayoutInitInfo&);
    StageSceneStateCloset(const char*, al::Scene*, const al::ActorInitInfo&,
                          const al::LayoutInitInfo&, Closet*);
    void initLayout(const al::LayoutInitInfo&);
    void initModel(const al::ActorInitInfo&);
    void appear() override;
    void kill() override;
    void control() override;
    void updateAfterMovement();
    bool isDrawViewRenderer() const;
    bool isOpenEnd() const;
    bool isChangedStage() const { return mIsChangedStage; }
    void tryCloseDoor();
    void exeAppear();
    void updateExistItem();
    void getClothesListIdx(const char*);
    void appearMarioModel();
    void exeSelect();
    void hideMarioModel();
    void exeWear();
    void exeEnd();
    void killAllMarioModel();
    void startActionMario(al::LiveActor*, const char*);
    ~StageSceneStateCloset() override;

private:
    u8 _20[0x19C - 0x20];
    bool mIsChangedStage;
};

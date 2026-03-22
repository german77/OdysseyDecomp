#pragma once

#include <basis/seadTypes.h>

#include "Library/Camera/IUseCamera.h"
#include "Library/Execute/IUseExecutor.h"
#include "Library/HostIO/HioNode.h"
#include "Library/Scene/IUseSceneObjHolder.h"

namespace al {
struct ActorInitInfo;
class ClippingJudge;
class AreaObjGroup;
}  // namespace al

class BirdGatheringSpotDirector : public al::HioNode,
                                  public al::IUseCamera,
                                  public al::IUseExecutor,
                                  public al::IUseSceneObjHolder {
public:
    BirdGatheringSpotDirector();
    void init(const al::ActorInitInfo&, const al::AreaObjGroup*, al::CameraDirector*,
              al::SceneObjHolder*, const al::ClippingJudge*, const char*);
    void execute() override;

    al::CameraDirector* getCameraDirector() const override;
    al::SceneObjHolder* getSceneObjHolder() const override;

private:
    s8 filler[0x58 - sizeof(al::HioNode) - sizeof(al::IUseCamera) - sizeof(al::IUseExecutor) -
              sizeof(al::IUseSceneObjHolder)];
};

static_assert(sizeof(BirdGatheringSpotDirector) == 0x58);

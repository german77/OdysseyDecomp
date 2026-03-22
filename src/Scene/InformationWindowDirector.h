#pragma once

#include "Library/Execute/IUseExecutor.h"
#include "Library/HostIO/HioNode.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace al {
class AreaObjDirector;
class SceneObjHolder;
struct ActorInitInfo;
class PlayerHolder;
class LayoutInitInfo;
}  // namespace al

class InformationWindowDirector : public al::HioNode,
                                  public al::ISceneObj,
                                  public al::IUseExecutor {
public:
    static constexpr s32 sSceneObjId = SceneObjID_InformationWindowDirector;

    InformationWindowDirector(al::AreaObjDirector*, al::SceneObjHolder*, const al::PlayerHolder*);
    ~InformationWindowDirector() override;

    void init(const al::ActorInitInfo&, const al::LayoutInitInfo&);
    void execute() override;
    void killAll();
    void killNotRetry();
    void changeSeparatePlay();
    void changeSinglePlay();
    void updateGyroText();

    const char* getSceneObjName() const override {
        return "インフォメーションウィンドウディレクタ";
    }

    virtual al::SceneObjHolder* getSceneObjHolder() const;

private:
    s8 filler[0x88 - sizeof(al::HioNode) - sizeof(al::ISceneObj) - sizeof(al::IUseExecutor)];
};

static_assert(sizeof(InformationWindowDirector) == 0x88);

#pragma once

#include "Library/HostIO/HioNode.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace al {
class IUseDataStore;

class DataStoreDirector : public al::ISceneObj, public al::HioNode {
public:
    static constexpr s32 sSceneObjId = SceneObjID_alDataStoreDirector;

    DataStoreDirector();
    ~DataStoreDirector() override;

    const char* getSceneObjName() const override { return "データストアディレクタ"; }

    s8 filler[0x38 - sizeof(al::ISceneObj) - sizeof(al::HioNode)];
};
}  // namespace al

static_assert(sizeof(al::DataStoreDirector) == 0x38);

#pragma once

#include "Library/HostIO/HioNode.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace al {
class IUseDataStore;

class DataStoreDirector : public ISceneObj, public HioNode {
public:
    static constexpr s32 sSceneObjId = SceneObjID_alDataStoreDirector;

    DataStoreDirector();
    ~DataStoreDirector() override;

    const char* getSceneObjName() const override { return "データストアディレクタ"; }

    s8 filler[0x38 - sizeof(ISceneObj) - sizeof(HioNode)];
};
}  // namespace al

static_assert(sizeof(al::DataStoreDirector) == 0x38);

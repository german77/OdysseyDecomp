#pragma once

#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace al {
class IUseSceneObjHolder;
}

class WipeHolderRequester : public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_WipeHolderRequester;

    WipeHolderRequester();

    void disableOpenWipeForSkipDemo();
    bool isDisableOpenWipeForSkipDemo() const;
    void requestWipeClose(const char*);
    const char* getRequestWipeCloseInfoName() const;
    void requestWipeOpenImmidiate();
    bool tryReceiveRequestWipeOpenImmidiate();

    const char* getSceneObjName() const override { return "ワイプホルダーへのリクエスト"; }

private:
    bool mIsDisableOpenWipeForSkipDemo = false;
    const char* mWipeCloseInfoName = nullptr;
    bool _20 = false;
};

namespace rs {
void disableOpenWipeForSkipDemo(const al::IUseSceneObjHolder*);
bool isDisableOpenWipeForSkipDemo(const al::IUseSceneObjHolder*);
void requestWipeClose(const al::IUseSceneObjHolder*, const char*);
const char* getRequestWipeCloseInfoName(const al::IUseSceneObjHolder*);
void requestWipeOpenImmidiate(const al::IUseSceneObjHolder*);
bool tryReceiveRequestWipeOpenImmidiate(const al::IUseSceneObjHolder*);
}  // namespace rs

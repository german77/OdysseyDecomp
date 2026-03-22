#pragma once

#include <basis/seadTypes.h>

#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace al {
class IUseSceneObjHolder;
}

class HtmlViewerRequester : public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_HtmlViewerRequester;

    HtmlViewerRequester();

    bool isExistRequest() const;
    void clearRequest();
    void request();

private:
    bool mIsExistRequest = false;
};

static_assert(sizeof(HtmlViewerRequester) == 0x10);

namespace rs {
bool isExistRequestShowHtmlViewer(const al::IUseSceneObjHolder*);
void clearRequestHtmlViewerRequester(const al::IUseSceneObjHolder*);
void requestShowHtmlViewer(const al::IUseSceneObjHolder*);
}  // namespace rs

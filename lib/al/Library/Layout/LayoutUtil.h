#pragma once

namespace agl {
class RenderBuffer;

}  // namespace agl

namespace al {
class AudioDirector;
class GamePadSystem;
class LayoutInitInfo;
class LayoutKit;
class LayoutSystem;
class MessageSystem;

void initLayoutInitInfo(al::LayoutInitInfo*, const al::LayoutKit*, al::SceneObjHolder*,
                        const al::AudioDirector*, const al::LayoutSystem*, const al::MessageSystem*,
                        const al::GamePadSystem*);
void setRenderBuffer(LayoutKit*, const agl::RenderBuffer*);
void executeUpdate(al::LayoutKit*);
void executeUpdateList(al::LayoutKit*, const char*, const char*);
void executeUpdateEffect(al::LayoutKit*);
void executeDraw(const LayoutKit*, const char*);
void executeDrawEffect(const al::LayoutKit*);
}  // namespace al

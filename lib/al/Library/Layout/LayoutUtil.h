#pragma once

namespace agl {
class RenderBuffer;

}  // namespace agl

namespace al {
class LayoutKit;
class LayoutInitInfo;
class SceneObjHolder;
class AudioDirector;
class LayoutSystem;
class MessageSystem;
class GamePadSystem;

void initLayoutInitInfo(LayoutInitInfo*, const LayoutKit*, SceneObjHolder*, const AudioDirector*,
                        const LayoutSystem*, const MessageSystem*, const GamePadSystem*);
void setRenderBuffer(LayoutKit*, const agl::RenderBuffer*);
void executeUpdate(LayoutKit*);
void executeUpdateList(LayoutKit*, const char*, const char*);
void executeUpdateEffect(LayoutKit*);
void executeDraw(const LayoutKit*, const char*);

}  // namespace al

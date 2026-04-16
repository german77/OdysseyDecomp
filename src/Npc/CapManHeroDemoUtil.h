#pragma once

namespace al {
class Scene;
struct ActorInitInfo;
}  // namespace al

namespace CapManHeroDemoUtil {
void initCapManHeroDemoDirector(const al::Scene* scene, const al::ActorInitInfo& initInfo);
bool isExistTalkDemoStageStart(const al::Scene*);
bool isExistTalkDemoAfterMoonRockBreakDemo(const al::Scene*);
bool isExistTalkDemoMoonRockFind(const al::Scene*);
bool isEndDemo(const al::Scene*);
void startTalkDemoStageStart(const al::Scene*);
void startTalkDemoAfterMoonRockBreakDemo(const al::Scene*);
void startTalkDemoMoonRockFind(const al::Scene*);
}  // namespace CapManHeroDemoUtil

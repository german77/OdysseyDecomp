#pragma once

namespace al {
class LiveActor;
}

namespace KoopaHackFunction {

void startStopKoopaHack(al::LiveActor* actor);
void endStopKoopaHack(al::LiveActor* actor);
bool isStopKoopaHack(const al::LiveActor* actor);
bool isStatusDemoForSceneKoopaHack(const al::LiveActor* actor);

}  // namespace KoopaHackFunction

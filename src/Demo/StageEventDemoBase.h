#pragma once

#include "Library/LiveActor/LiveActor.h"

namespace al {
struct ActorInitInfo;
}

class StageEventDemoBase : public al::LiveActor {
public:
    StageEventDemoBase(const char*);

    void init(const al::ActorInitInfo&) override;

    bool isEnableStart() const;
};

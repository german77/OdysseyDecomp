#pragma once

#include "Library/LiveActor/LiveActor.h"

class CollectionList : public al::LiveActor {
public:
    CollectionList(const char* name);
    void init(const al::ActorInitInfo& info) override;
    bool isOpen() const;
    void close();
};

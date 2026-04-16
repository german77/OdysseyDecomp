#pragma once

#include "Library/Area/SwitchKeepOnAreaGroup.h"

namespace al {
class AreaObjDirector;
class SwitchAreaGroup;
}  // namespace al

class SwitchKeepOnIgnoreOffAreaTarget : public al::SwitchKeepOnAreaGroup {
public:
    static SwitchKeepOnIgnoreOffAreaTarget* tryCreate(const char*, al::AreaObjDirector*);
};

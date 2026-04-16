#pragma once

#include "Library/Area/SwitchOnAreaGroup.h"
namespace al {
class AreaObjDirector;
class PlayerHolder;
class SwitchAreaGroup;
}  // namespace al

class SwitchOn2DAreaGroup : public al::SwitchOnAreaGroup{
public:
    static SwitchOn2DAreaGroup* tryCreate(const char*, al::AreaObjDirector*,
                                          const al::PlayerHolder*);
};

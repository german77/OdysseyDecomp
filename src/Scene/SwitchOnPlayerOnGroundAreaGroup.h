#pragma once

#include "Library/Area/SwitchOnAreaGroup.h"
namespace al {
class AreaObjDirector;
class PlayerHolder;
class SwitchAreaGroup;
}  // namespace al

class SwitchOnPlayerOnGroundAreaGroup : public al::SwitchOnAreaGroup{
public:
    static SwitchOnPlayerOnGroundAreaGroup* tryCreate(const char*, al::AreaObjDirector*,
                                          const al::PlayerHolder*);
};

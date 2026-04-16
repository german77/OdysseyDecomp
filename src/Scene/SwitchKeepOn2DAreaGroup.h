#pragma once

#include "Library/Area/SwitchKeepOnAreaGroup.h"
namespace al {
class AreaObjDirector;
class PlayerHolder;
class SwitchAreaGroup;
}  // namespace al

class SwitchKeepOn2DAreaGroup : public al::SwitchKeepOnAreaGroup{
public:
    static SwitchKeepOn2DAreaGroup* tryCreate(const char*, al::AreaObjDirector*,
                                          const al::PlayerHolder*);
};

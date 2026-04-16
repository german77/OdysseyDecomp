#pragma once

#include "Library/Area/SwitchKeepOnAreaGroup.h"
namespace al {
class AreaObjDirector;
class PlayerHolder;
class SwitchAreaGroup;
}  // namespace al

class SwitchKeepOnPlayerInWaterAreaGroup : public al::SwitchKeepOnAreaGroup{
public:
    static SwitchKeepOnPlayerInWaterAreaGroup* tryCreate(const char*, al::AreaObjDirector*,
                                          const al::PlayerHolder*);
};

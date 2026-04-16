#pragma once

#include "Library/Area/SwitchKeepOnAreaGroup.h"
namespace al {
class AreaObjDirector;
class PlayerHolder;
class SwitchAreaGroup;
}  // namespace al

class SwitchKeepOn2DExceptDokanInAreaGroup  : public al::SwitchKeepOnAreaGroup{
public:
    static SwitchKeepOn2DExceptDokanInAreaGroup* tryCreate(const char*, al::AreaObjDirector*,
                                          const al::PlayerHolder*);
};

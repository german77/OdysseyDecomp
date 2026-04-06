#pragma once

#include <basis/seadTypes.h>

class ProjectSeNamedList {
public:
    ProjectSeNamedList();

    const char** getMuteSeInPVList();
    s32 getMuteSeInPVListSize();
};

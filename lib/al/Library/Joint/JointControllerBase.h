#pragma once

#include <basis/seadTypes.h>

namespace al {

class JointControllerBase {
public:
    JointControllerBase(s32);
    void appendJointId(s32);
    void findNextId(s32*, s32) const;
    bool isExistId(s32) const;

private:
    char filler[0x28];
};

static_assert(sizeof(JointControllerBase) == 0x28);

}  // namespace al

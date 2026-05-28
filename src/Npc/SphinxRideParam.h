#pragma once

#include <basis/seadTypes.h>

struct SphinxRideParam {
    f32 gravBase;
    f32 dumpRatio;
    sead::Vector3f a;
    sead::Vector3f b;
    sead::Vector3f c;
};

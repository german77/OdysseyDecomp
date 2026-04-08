#pragma once

#include <basis/seadTypes.h>
#include "Library/Yaml/ParameterBase.h"

namespace al {
class ParameterObj;

class IUseRequestParam {
public:
    void init() { mParameterObj = new ParameterObj(); }

    virtual const char* getParamName() const = 0;
    virtual ParameterObj* getParamObj() = 0;
    virtual const ParameterObj* getParamObj() const = 0;
    virtual bool isEqual(const al::IUseRequestParam&) const;
    virtual void copy(const al::IUseRequestParam&) const;
    virtual void copy(const al::IUseRequestParam&, f32) const;

protected:
    ParameterObj* mParameterObj;
};

static_assert(sizeof(IUseRequestParam) == 0x10);

}  // namespace al

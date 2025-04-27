#pragma once

#include <basis/seadTypes.h>
#include <prim/seadSafeString.h>

namespace al {
class ByamlIter;
class ParameterObj;

class ParameterArray {
public:
    ParameterArray();
    void tryGetParam(const ByamlIter&);
    bool isEqual(const ParameterArray&) const;
    void copy(const ParameterArray&);
    void copyLerp(const ParameterArray&, const ParameterArray&, f32);
    void addObj(ParameterObj*);
    void clearObj();
    void removeObj(ParameterObj*);
    bool isExistObj(ParameterObj*);
    
ParameterObj* getFirstObj()const{return mFirstObj;}

    s32 getSize() const{ return mSize; }

private:
    ParameterObj* mFirstObj = nullptr;
    ParameterArray* mNext = nullptr;
    sead::FixedSafeString<0x40> mParamObjKey;
    s32 mSize = 0;
};

}  // namespace al

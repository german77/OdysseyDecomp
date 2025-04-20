#pragma once

#include <basis/seadTypes.h>
#include <prim/seadSafeString.h>

namespace al {
class ByamlIter;
class ParameterObj;

class ParameterArray {
public:
    ParameterArray();
    bool tryGetParam(const ByamlIter&);
    bool isEqual(const ParameterArray&) const;
    void copy(const ParameterArray&);
    void copyLerp(const ParameterArray&, const ParameterArray&, f32);
    void addObj(ParameterObj*);
    void clearObj();
    void removeObj(ParameterObj*);
    bool isExistObj(ParameterObj*);

    s32 getKeyHash() { return mKeyHash; }

private:
    ParameterObj* mFirstParam;
    ParameterArray* mNext;
    sead::FixedSafeString<0x40> mParamObjKey;
    s32 mKeyHash;
};

}  // namespace al

#pragma once

#include <prim/seadSafeString.h>

namespace al {
class ParameterBase;
class ByamlIter;
class ParameterArray;
class ParameterObj;

class ParameterList {
public:
    ParameterList();

    void addArray(ParameterArray*, const sead::SafeString&);
    void addList(ParameterList*, const sead::SafeString&);
    void addObj(ParameterObj*, const sead::SafeString&);
    void addParam(ParameterBase*);
    void clearList(void);
    void clearObj(void);
    void isExistObj(ParameterObj*);
    void removeList(ParameterList*);
    void removeObj(ParameterObj*);
    void tryGetParam(const ByamlIter&);

private:
    ParameterBase* mRootParamNode;
    ParameterObj* mRootObjNode;
    ParameterList* mRootListNode;
    ParameterArray* mRootArrayNode;
    ParameterList* mNext;
    sead::FixedSafeString<0x40> mNodeKey;
};

class ParameterIo : public ParameterList {};

}  // namespace al

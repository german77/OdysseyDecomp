#pragma once

#include <prim/seadSafeString.h>

namespace al {
class ParameterBase;
class ByamlIter;
class ParameterArray;

class ParameterList {
public:
    ParameterList(void);

    void addArray(al::ParameterArray*, const sead::SafeString&);
    void addList(al::ParameterList*, const sead::SafeString&);
    void addObj(al::ParameterObj*, const sead::SafeString&);
    void addParam(al::ParameterBase*);
    void clearList(void);
    void clearObj(void);
    void isExistObj(al::ParameterObj*);
    void removeList(al::ParameterList*);
    void removeObj(al::ParameterObj*);
    void tryGetParam(const al::ByamlIter&);

private:
    al::ParameterBase* mRootParamNode;
    al::ParameterObj* mRootObjNode;
    al::ParameterList* mRootListNode;
    al::ParameterArray* mRootArrayNode;
    al::ParameterList* mNext;
    sead::FixedSafeString<0x40> mNodeKey;
};

class ParameterIo : public ParameterList {};

}  // namespace al

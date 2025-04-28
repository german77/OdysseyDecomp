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
    bool isExistObj(ParameterObj*);
    void removeList(ParameterList*);
    void removeObj(ParameterObj*);
    void tryGetParam(const ByamlIter&);

    ParameterList* getNext() const { return mNext; }

    void setNext(ParameterList* list) { mNext = list; }

    void setKey(const sead::SafeString& key) { mKey = key; }

private:
    ParameterBase* mRootParamNode = nullptr;
    ParameterObj* mRootObjNode = nullptr;
    ParameterList* mRootListNode = nullptr;
    ParameterArray* mRootArrayNode = nullptr;
    ParameterList* mNext = nullptr;
    sead::FixedSafeString<0x40> mKey;
};

class ParameterIo : public ParameterList {};

}  // namespace al

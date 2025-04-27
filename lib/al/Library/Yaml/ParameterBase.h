#pragma once

#include <prim/seadSafeString.h>

namespace al {
class ByamlIter;
class ParameterObj;
class ParameterList;

class ParameterBase {
public:
    ParameterBase(const sead::SafeString& a, const sead::SafeString& b, const sead::SafeString& c,
                  ParameterObj* d, bool e) {
        initialize("default", "parameter", "", e);
        mValue = 0;
        initializeListNode(a, b, c, d, e);
        mValue = 0;
    }

    ParameterBase(const sead::SafeString& a, const sead::SafeString& b, const sead::SafeString& c,
                  ParameterList* d, bool e) {
        initializeListNode(a, b, c, d, e);
    }

    virtual void afterGetParam() {}

    virtual bool isEqual(const ParameterBase* other);
    virtual void copy(const ParameterBase* other);
    virtual void copyLerp(const ParameterBase*, const ParameterBase*, f32);

    void initializeListNode(sead::SafeString const&, sead::SafeString const&,
                            sead::SafeString const&, ParameterObj*, bool);
    void initializeListNode(sead::SafeString const&, sead::SafeString const&,
                            sead::SafeString const&, ParameterList*, bool);
    void initialize(sead::SafeString const&, sead::SafeString const&, sead::SafeString const&,
                    bool);
    u32 calcHash(sead::SafeString const&);
    void tryGetParam(ByamlIter const&);

    s32 getValue() const { return mValue; }

    void setValue(s32 value) { mValue = value; }

private:
    ParameterBase* mNext;
    sead::FixedSafeString<0x40> mParamName;
    s32 mParamHash;
    s32 mValue;
};

// TODO: Define a macro or similar for all parameter types
class ParameterS32 : public ParameterBase {
public:
    ParameterS32(const sead::SafeString& a, const sead::SafeString& b, const sead::SafeString& c,
                 ParameterObj* obj, bool isBool)
        : ParameterBase(a, b, c, obj, isBool) {
        initialize("default", "parameter", "", true);
        mValue = 0;
    };

    ParameterS32(const sead::SafeString& a, const sead::SafeString& b, const sead::SafeString& c,
                 ParameterList* list, bool isBool)
        : ParameterBase(a, b, c, list, isBool) {
        initialize("default", "parameter", "", true);
        mValue = 0;
    };

    const char* getParamTypeStr() { return "S32"; };

    s32 getParamType() { return 3; };

    const void* ptr() const { return &mValue; };

    void* ptr() { return &mValue; };

    s32 size() { return 4; }

    s32 getValue() const { return mValue; }

    void setValue(s32 value) { mValue = value; }

private:
    s32 mValue = 0;
};

}  // namespace al

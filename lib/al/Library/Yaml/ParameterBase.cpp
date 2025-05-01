#include "Library/Yaml/ParameterBase.h"


#include "Library/Yaml/ByamlIter.h"
#include "Library/Yaml/ByamlUtil.h"

namespace al {

ParameterBase::ParameterBase(const sead::SafeString& keyA, const sead::SafeString& keyB,
                             const sead::SafeString& keyC, ParameterObj* obj, bool isValue) {
    initializeListNode(keyA, keyB, keyC, obj, isValue);
}

ParameterBase::ParameterBase(const sead::SafeString& keyA, const sead::SafeString& keyB,
                             const sead::SafeString& keyC, ParameterList* list, bool isValue) {
    initializeListNode(keyA, keyB, keyC, list, isValue);
}

void ParameterBase::afterGetParam() {}

bool ParameterBase::isEqual(const ParameterBase*) {}

bool ParameterBase::copy(const ParameterBase* param) {
    YamlParamType dstType = getParamType();
    YamlParamType srcType = param->getParamType();

    if (dstType != srcType && mHash != param->getHash())
        return false;
    return true;
}

bool ParameterBase::copyLerp(const ParameterBase*, const ParameterBase*, f32) {}

void ParameterBase::initializeListNode(const sead::SafeString&, const sead::SafeString&,
                                       const sead::SafeString&, ParameterObj*, bool) {}

void ParameterBase::initializeListNode(const sead::SafeString&, const sead::SafeString&,
                                       const sead::SafeString&, ParameterList*, bool) {}

void ParameterBase::initialize(const sead::SafeString&, const sead::SafeString&,
                               const sead::SafeString&, bool) {}

u32 ParameterBase::calcHash(const sead::SafeString&) {}

void ParameterBase::tryGetParam(const ByamlIter& iter) {
    getParamTypeStr();

    switch (getParamType()) {
    case YamlParamType::Bool: {
        bool value;
        if (tryGetByamlBool(&value, iter, mName.cstr()))
            *getValuePtr<bool>() = value;
        break;
    }
    case YamlParamType::F32: {
        f32 value;
        if (tryGetByamlF32(&value, iter, mName.cstr()))
            *getValuePtr<f32>() = value;
        break;
    }
    case YamlParamType::S32: {
        s32 value;
        if (tryGetByamlS32(&value, iter, mName.cstr()))
            *getValuePtr<s32>() = value;
        break;
    }
    case YamlParamType::U32: {
        u32 value;
        if (tryGetByamlU32(&value, iter, mName.cstr()))
            *getValuePtr<u32>() = value;
        break;
    }
    case YamlParamType::V2f: {
        sead::Vector2f value;
        if (tryGetByamlV2f(&value, iter, mName.cstr()))
            *getValuePtr<sead::Vector2f>() = value;
        break;
    }
    case YamlParamType::V2s32: {
        sead::Vector2i value;
        if (tryGetByamlV2s32(&value, iter, mName.cstr()))
            *getValuePtr<sead::Vector2i>() = value;
        break;
    }
    case YamlParamType::V3f: {
        sead::Vector3f value;
        if (tryGetByamlV3f(&value, iter, mName.cstr()))
            *getValuePtr<sead::Vector3f>() = value;
        break;
    }
    case YamlParamType::V4f:
    case YamlParamType::Q4f: {
        sead::Vector4f value;
        if (tryGetByamlV4f(&value, iter, mName.cstr()))
            *getValuePtr<sead::Vector4f>() = value;
        break;
    }
    case YamlParamType::C4f: {
        sead::Color4f value{sead::Color4f::cElementMin,sead::Color4f::cElementMin,
        sead::Color4f::cElementMin,sead::Color4f::cElementMax};
        if (tryGetByamlColor(&value, iter, mName.cstr()))
            *getValuePtr<sead::Color4f>() = value;

        break;
    }
    case YamlParamType::StringRef: {
        const char* value = tryGetByamlKeyStringOrNULL(iter, mName.cstr());
        if (value){
            *getValuePtr<const char*>() = value;
            }
        afterGetParam();
        return;
    }
    case YamlParamType::String32:
    case YamlParamType::String128:
    case YamlParamType::String256:
    case YamlParamType::String512:
    case YamlParamType::String1024:
    case YamlParamType::String2048:
    case YamlParamType::String4096: {
        const char* value = tryGetByamlKeyStringOrNULL(iter, mName.cstr());
        if (value)
            getValuePtr<sead::BufferedSafeStringBase<char>>()->format("%s", value);
        afterGetParam();
        return;
    }
    default:
        return;
    }

    afterGetParam();
}

}  // namespace al

#include "Library/Yaml/ParameterBase.h"

#include <math/seadQuat.h>

#include "Library/Base/StringUtil.h"

namespace al {

ParameterBase::ParameterBase(const sead::SafeString& a, const sead::SafeString& b,
                             const sead::SafeString& c, ParameterObj* d, bool e) {
    initializeListNode(a, b, c, d, e);
}

ParameterBase::ParameterBase(const sead::SafeString& a, const sead::SafeString& b,
                             const sead::SafeString& c, ParameterList* d, bool e) {
    initializeListNode(a, b, c, d, e);
}

void ParameterBase::afterGetParam() {}

bool ParameterBase::isEqual(const ParameterBase& parameter) const{
    if ((s32)getParamType() != (s32)parameter.getParamType())
        return false;

    if (mHash != parameter.getHash())
        return false;

    switch (getParamType()) {
    case YamlParamType::Bool:
        return *(bool*)ptr() == *(bool*)parameter.ptr();

    case YamlParamType::F32:
        return *(f32*)ptr() == *(f32*)parameter.ptr();

    case YamlParamType::S32:
        return *(s32*)ptr() == *(s32*)parameter.ptr();

    case YamlParamType::U32:
        return *(u32*)ptr() == *(u32*)parameter.ptr();

    case YamlParamType::V2f:
        return *(sead::Vector2f*)ptr() == *(sead::Vector2f*)parameter.ptr();

    case YamlParamType::V2s32:
        return *(sead::Vector2i*)ptr() == *(sead::Vector2i*)parameter.ptr();

    case YamlParamType::V3f:
        return *(sead::Vector3f*)ptr() == *(sead::Vector3f*)parameter.ptr();

    case YamlParamType::V4f:
        return *(sead::Vector4f*)ptr() == *(sead::Vector4f*)parameter.ptr();

    case YamlParamType::Q4f:
        return *(sead::Quatf*)ptr() == *(sead::Quatf*)parameter.ptr();

    case YamlParamType::C4f:
        return *(sead::Color4f*)ptr() == *(sead::Color4f*)parameter.ptr();

    case YamlParamType::StringRef:
        return isEqualString((const char*)ptr(), (const char*)parameter.ptr());

    case YamlParamType::String32:
    case YamlParamType::String64:
    case YamlParamType::String128:
    case YamlParamType::String256:
    case YamlParamType::String512:
    case YamlParamType::String1024:
    case YamlParamType::String2048:
    case YamlParamType::String4096:
        return isEqualString(((sead::SafeString*)ptr())->cstr(),
                             ((sead::SafeString*)parameter.ptr())->cstr());
    default:
        return false;
    }
}

bool ParameterBase::copy(const ParameterBase& parameter) {
    if ((s32)getParamType() != (s32)parameter.getParamType())
        return false;

    if (mHash != parameter.getHash())
        return false;

    char* dest=(char*)ptr();
    if (parameter.getParamType() == YamlParamType::StringRef) {
        strcpy((char*)dest, (const char*)parameter.ptr());
      return true;
    }

    const char* src=(char*)parameter.ptr();
    s32 length = size();
    if(length >=4){
        for(s32 i=0;i<length/4;i++){
            ((s32*)dest)[i]=((const s32*)src)[i];
        }
    }
     for(s32 i=0;i<length;i++){
        dest[i]=src[i];
    }
    return true;

}

bool copyLerp(const ParameterBase&, const ParameterBase&, f32) {}

void initializeListNode(const sead::SafeString&, const sead::SafeString&, const sead::SafeString&,
                        ParameterObj*, bool) {}

void initializeListNode(const sead::SafeString&, const sead::SafeString&, const sead::SafeString&,
                        ParameterList*, bool) {}


u32 calcHash(const sead::SafeString&) {}

void tryGetParam(const ByamlIter&) {}

}  // namespace al

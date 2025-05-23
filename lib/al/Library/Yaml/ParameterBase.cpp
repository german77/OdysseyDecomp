#include "Library/Yaml/ParameterBase.h"

namespace al {

ParameterBase::ParameterBase(const sead::SafeString& a, const sead::SafeString& b,
                             const sead::SafeString& c, ParameterObj* d, bool e) {
    initializeListNode(a, b, c, d, e);
}

ParameterBase::ParameterBase(const sead::SafeString& a, const sead::SafeString& b,
                             const sead::SafeString& c, ParameterList* d, bool e) {
    initializeListNode(a, b, c, d, e);
}

}  // namespace al

#include "Library/Yaml/ParameterList.h"

#include "Library/Yaml/ParameterObj.h"
#include "Library/Yaml/ByamlIter.h"
#include "Library/Yaml/ParameterArray.h"

namespace al {

ParameterList::ParameterList(){}

void ParameterList::addArray(al::ParameterArray*, const sead::SafeString&){}
void ParameterList::addList(al::ParameterList*, const sead::SafeString&){}
void ParameterList::addObj(al::ParameterObj*, const sead::SafeString&){}
void ParameterList::addParam(al::ParameterBase*){}
void ParameterList::clearList(void){}
void ParameterList::clearObj(void){}
void ParameterList::isExistObj(al::ParameterObj*){}
void ParameterList::removeList(al::ParameterList*){}
void ParameterList::removeObj(al::ParameterObj*){}
void ParameterList::tryGetParam(const al::ByamlIter&){}
    
}  // namespace al

#pragma once

#include <basis/seadTypes.h>
#include <container/seadRingBuffer.h>
#include <prim/seadSafeString.h>

namespace al {
class ByamlIter;
class Resource;

class ResourceSystem {
public:
    enum class ResourceCategory : s32;

    ResourceSystem(const char*);

    Resource* addCategory(const sead::SafeStringBase<char>&, s32, sead::Heap*);
    Resource* findOrCreateResourceCategory(const sead::SafeStringBase<char>&,
                                           const sead::SafeStringBase<char>&, const char*);
    void findResourceCategoryIter(const sead::SafeStringBase<char>&);
    bool isEmptyCategoryResource(const sead::SafeStringBase<char>&);
    void createCategoryResourceAll(const sead::SafeStringBase<char>&);
    void createResource(const sead::SafeStringBase<char>&, ResourceCategory*, const char*);
    void removeCategory(const sead::SafeStringBase<char>&);
    Resource* findResource(const sead::SafeStringBase<char>&);
    Resource* findResourceCore(const sead::SafeStringBase<char>&,
                               sead::RingBuffer<ResourceCategory*>::iterator*);
    Resource* findOrCreateResource(const sead::SafeStringBase<char>&, const char*);
    Resource* findResourceCategory(const sead::SafeStringBase<char>&);
    void loadCategoryArchiveAll(const sead::SafeStringBase<char>&);
    void setCurrentCategory(const char*);
    void findCategoryNameFromTable(const sead::SafeStringBase<char>&) const;
    void tryGetTableCategoryIter(al::ByamlIter*, const sead::SafeStringBase<char>&) const;
    void tryGetGraphicsInfoIter(al::ByamlIter*, const sead::SafeStringBase<char>&) const;

private:
};

}  // namespace al

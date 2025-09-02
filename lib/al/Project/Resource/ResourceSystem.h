#pragma once

#include <container/seadRingBuffer.h>
#include <prim/seadSafeString.h>

namespace sead {
class Heap;
template <s32, typename>
class StrTreeMap;
}  // namespace sead

namespace al {
class Resource;
class ByamlIter;
class SeadAudioPlayer;

class ResourceSystem {
public:
    struct ResourceCategory {
        sead::FixedSafeString<0x80> name;
        sead::Heap* heap = nullptr;
        sead::StrTreeMap<152, Resource*>* treeMap = nullptr;
        void* _a8 = nullptr;
        void* _b0 = nullptr;
        s32 size = 0;
    };

    static_assert(sizeof(ResourceCategory) == 0xc0);

    ResourceSystem(const char*);

    const sead::SafeString& addCategory(const sead::SafeString&, s32, sead::Heap*);
    Resource* findOrCreateResourceCategory(const sead::SafeString&, const sead::SafeString&,
                                           const char*);
    sead::RingBuffer<ResourceSystem::ResourceCategory*>::iterator
    findResourceCategoryIter(const sead::SafeString&);
    bool isEmptyCategoryResource(const sead::SafeString&);
    void createCategoryResourceAll(const sead::SafeString&);
    Resource* createResource(const sead::SafeString&, ResourceCategory*, const char*);
    void removeCategory(const sead::SafeString&);
    Resource* findResource(const sead::SafeString&);
    Resource* findResourceCore(const sead::SafeString&,
                               sead::RingBuffer<ResourceCategory*>::iterator*);
    Resource* findOrCreateResource(const sead::SafeString&, const char*);
    ResourceCategory* findResourceCategory(const sead::SafeString&);
    void loadCategoryArchiveAll(const sead::SafeString&);
    void setCurrentCategory(const char*);
    const char* findCategoryNameFromTable(const sead::SafeString&) const;
    bool tryGetTableCategoryIter(ByamlIter*, const sead::SafeString&) const;
    bool tryGetGraphicsInfoIter(ByamlIter*, const sead::SafeString&) const;

    void resetCurrentCategoryName() { mCurrentCategoryName = nullptr; }

    void setAudioPlayer(SeadAudioPlayer* audioPlayerA, SeadAudioPlayer* audioPlayerB) {
        mAudioPlayerA = audioPlayerA;
        mAudioPlayerB = audioPlayerB;
    }

private:
    sead::FixedRingBuffer<ResourceCategory*, 18> mCategories;
    ByamlIter* mResourceCategoryTable = nullptr;
    const char* mCurrentCategoryName = nullptr;
    SeadAudioPlayer* mAudioPlayerA = nullptr;
    SeadAudioPlayer* mAudioPlayerB = nullptr;
};

static_assert(sizeof(ResourceSystem) == 0xc8);
}  // namespace al

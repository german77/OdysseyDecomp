#include "Library/Resource/ResourceFunction.h"

#include "Library/Base/StringUtil.h"
#include "Library/Resource/Resource.h"
#include "Library/System/SystemKit.h"
#include "Project/Resource/ResourceSystem.h"
#include "System/ProjectInterface.h"

namespace al {

Resource* addResourceCategory(const sead::SafeString& resourceName, s32 category,
                              sead::Heap* heap) {
    return alProjectInterface::getSystemKit()->getResourceSystem()->addCategory(resourceName,
                                                                                category, heap);
}

bool isEmptyCategoryResource(const sead::SafeString& resourceName) {
    return alProjectInterface::getSystemKit()->getResourceSystem()->isEmptyCategoryResource(
        resourceName);
}

void createCategoryResourceAll(const sead::SafeString& resourceName) {
    alProjectInterface::getSystemKit()->getResourceSystem()->createCategoryResourceAll(
        resourceName);
}

void removeResourceCategory(const sead::SafeString& resourceName) {
    alProjectInterface::getSystemKit()->getResourceSystem()->createCategoryResourceAll(
        resourceName);
}

const char* getResourceName(const Resource* resource) {
    return resource->getArchiveName();
}

const char* getResourcePath(const Resource* resource) {
    return resource->getPath();
}

bool isExistResGraphicsFile(const Resource* resource) {
    return resource->getResFile() != nullptr;
}

Resource* findResource(const sead::SafeString& resourceName) {
    return alProjectInterface::getSystemKit()->getResourceSystem()->findResource(resourceName);
}

Resource* findOrCreateResource(const sead::SafeString& resourceName, const char* ext) {
    return alProjectInterface::getSystemKit()->getResourceSystem()->findOrCreateResource(
        resourceName, ext);
}

Resource* findOrCreateResourceCategory(const sead::SafeString& resourceName,
                                       const sead::SafeString& category, const char* ext) {
    return alProjectInterface::getSystemKit()->getResourceSystem()->findOrCreateResourceCategory(
        resourceName, category, ext);
}

Resource* findOrCreateResourceEventData(const char* eventDataName, const char* resourceName) {
    StringTmp<128> eventName = {"EventData/%s", eventDataName};
    return alProjectInterface::getSystemKit()->getResourceSystem()->findOrCreateResource(
        eventName, resourceName);
}

Resource* findOrCreateResourceSystemData(const char* systemDataName, const char* resourceName) {
    StringTmp<128> systemName = {"SystemData/%s", systemDataName};
    return alProjectInterface::getSystemKit()->getResourceSystem()->findOrCreateResource(
        systemName, resourceName);
}

ActorResource* findOrCreateActorResource(ActorResourceHolder*, const char*, const char*);
void findOrCreateActorResourceWithAnimResource(ActorResourceHolder*, const char*, const char*,
                                               const char*, bool);
bool isExistResourceYaml(const Resource*, const char*, const char*);
bool isExistResourceYaml(const ActorResource*, const char*, const char*);
void findResourceYaml(const Resource*, const char*, const char*);
void findResourceYaml(const ActorResource*, const char*, const char*);
bool tryFindStageParameterFileDesign(const sead::SafeString&, const sead::SafeString&, s32);
void loadCategoryArchiveAll(const sead::SafeString&);
void setCurrentCategoryName(const char*);
void resetCurrentCategoryName();
void setCurrentCategoryNameDefault();
u8* getBymlFromObjectResource(const sead::SafeString&, const sead::SafeString&);
u8* tryGetBymlFromObjectResource(const sead::SafeString&, const sead::SafeString&);
u8* getBymlFromLayoutResource(const sead::SafeString&, const sead::SafeString&);
u8* tryGetBymlFromLayoutResource(const sead::SafeString&, const sead::SafeString&);
u8* tryGetBymlFromArcName(const sead::SafeString&, const sead::SafeString&);
u8* getBymlFromArcName(const sead::SafeString&, const sead::SafeString&);
u8* getByml(const Resource*, const sead::SafeString&);
u8* tryGetByml(const Resource*, const sead::SafeString&);
bool setAudioPlayerToResourceSystem(SeadAudioPlayer*, SeadAudioPlayer*);

}  // namespace al

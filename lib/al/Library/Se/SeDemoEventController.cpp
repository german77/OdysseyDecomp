#include "Library/Se/SeDemoEventController.h"

#include "Library/Audio/AudioInfo.h"

namespace al {
//SeDemoEventController::SeDemoEventController(al::AudioDirector*);

void SeDemoEventController::startEvent(al::AudioInfoListWithParts<al::SeDemoProcInfo>* audioInfoList){
    mDemoProcInfo = audioInfoList;
}

//void SeDemoEventController::endEvent(al::AudioInfoListWithParts<al::SeDemoProcInfo>*, bool);
//void SeDemoEventController::procDemoEvent(const al::SeDemoProcInfo*);
//void SeDemoEventController::update(s32);

}  // namespace al

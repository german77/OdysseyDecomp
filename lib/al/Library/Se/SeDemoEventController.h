#pragma once

#include <basis/seadTypes.h>

namespace al {
template <typename T>
class AudioInfoListWithParts;
class AudioDirector;
class SeDemoProcInfo;

class SeDemoEventController {
public:
    SeDemoEventController(al::AudioDirector*);

    void startEvent(al::AudioInfoListWithParts<al::SeDemoProcInfo>*);
    void endEvent(al::AudioInfoListWithParts<al::SeDemoProcInfo>*, bool);
    void procDemoEvent(const al::SeDemoProcInfo*);
    void update(s32);

private:
    AudioDirector* director;
    AudioInfoListWithParts<SeDemoProcInfo>* mDemoProcInfo;
    AudioInfoListWithParts<SeDemoProcInfo>* mDemoProcInfo2;
    AudioInfoListWithParts<SeDemoProcInfo>* mDemoProcInfo3;
};

//static_assert(sizeof(SeDemoEventController) == 0x50);
}  // namespace al

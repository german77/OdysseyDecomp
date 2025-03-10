#pragma once

#include <math/seadVector.h>

#include "Library/Layout/LayoutActor.h"

namespace al {
class LayoutInitInfo;
class EventFlowChoiceInfo;
class MessageTagDataHolder;
}  // namespace al

class TalkMessage : public al::LayoutActor {
public:
    TalkMessage(const char*);
    void initLayoutTalk(const al::LayoutInitInfo&, const char*);
    void initLayoutWithArchiveName(const al::LayoutInitInfo&, const char*, const char*);
    void initLayoutImportant(const al::LayoutInitInfo&, const char*);
    void initLayoutOver(const al::LayoutInitInfo&, const char*);
    void initLayoutForEventTalk(const al::LayoutInitInfo&);
    void initLayoutForEventImportant(const al::LayoutInitInfo&);
    void startForNpc(const al::LiveActor*, const char16*, const char16*,
                     const al::MessageTagDataHolder*, bool);
    void reset();
    void startForSystem(const char16*, const al::MessageTagDataHolder*, bool);
    void end();
    bool isIconWait() const;
    void kill();
    bool isWait() const;
    void startSelectWithChoiceTable(const char16**, s32, s32);
    void startSelectWithChoiceInfo(const al::EventFlowChoiceInfo*);
    bool isSelectDecide() const;
    s32 getSelectedChoiceIndex() const;
    void exeAppear();
    void exeAppearWithText();
    void exeTextAnim();
    void exeIconAppearDelay();
    void exeIconAppear();
    void exeIconWait();
    void exeIconWaitTriggered();
    void exeIconPageNext();
    void exeIconPageNextAndPlayNextPage();
    void exeIconPageNextAndLoadNextMessage();
    void exeIconPageEnd();
    void exeWait();
    void exeEnd();
    void appear();
    void control();
    void startIconPageNext();

private:
    char filler[0x44];
};

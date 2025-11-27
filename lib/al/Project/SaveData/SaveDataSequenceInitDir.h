#pragma once

#include <basis/seadTypes.h>

#include "Project/SaveDataSequenceBase.h"

namespace al {

class SaveDataSequenceInitDir : public SaveDataSequenceBase {
public:
    SaveDataSequenceInitDir(u8);

    s32 threadFunc(const char* arg) override;

    void start(u8*, u32, u32);

private:
    u8* _8 = nullptr;
    u32 _10 = 0;
    u32 _14 = 0;
    u8 _18 = 0;
};

static_assert(sizeof(SaveDataSequenceInitDir) == 0x20);

}  // namespace al

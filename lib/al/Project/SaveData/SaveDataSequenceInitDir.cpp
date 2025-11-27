#include "Project/SaveData/SaveDataSequenceInitDir.h"

#include <nn/account.h>
#include <nn/fs/fs_save.h>

namespace al {

SaveDataSequenceInitDir::SaveDataSequenceInitDir(u8 cp) : _18(cp) {}

s32 SaveDataSequenceInitDir::threadFunc(const char* arg) {
    nn::account::Uid uid;
    nn::account::GetLastOpenedUser(&uid);

    nn::fs::EnsureSaveData(uid);
    nn::fs::MountSaveData("save", uid);
    return 0;
}

void SaveDataSequenceInitDir::start(u8* a, u32 b, u32 c) {
    _8 = a;
    _10 = b;
    _14 = c;
}

}  // namespace al

#pragma once

namespace al {
class SeDirector {
public:
    void setMuteSeInPVList(const char** list, s32 size) {
        mMuteSeInPVList = list;
        mMuteSeInPVListSize = size;
    }

private:
    unsigned char _padding[0xa8];
    const char** mMuteSeInPVList;
    s32 mMuteSeInPVListSize;
};
}  // namespace al

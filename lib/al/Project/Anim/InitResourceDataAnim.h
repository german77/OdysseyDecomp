#pragma once

namespace al {
class AnimInfoTable;
class Resource;

class InitResourceDataAnim {
public:
    static InitResourceDataAnim* tryCreate(Resource*, Resource*);

    InitResourceDataAnim(Resource*, AnimInfoTable*, AnimInfoTable*, AnimInfoTable*, AnimInfoTable*,
                         AnimInfoTable*);

    AnimInfoTable* getAnimInfoTable(s32 matType) const {
        switch (matType) {
        case 0:
            return mFclAnim;
        case 1:
            return mFtpAnim;
        case 2:
            return mFtsAnim;
        default:
            return nullptr;
        }
    }

private:
    AnimInfoTable* mInfoTable;
    AnimInfoTable* mFclAnim;
    AnimInfoTable* mFtsAnim;
    AnimInfoTable* mFtpAnim;
    AnimInfoTable* mInfoTable2;
};
}  // namespace al

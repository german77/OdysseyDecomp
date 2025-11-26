#pragma once

#include <basis/seadTypes.h>
<<<<<<< HEAD
#include <container/seadBuffer.h>
=======
>>>>>>> b7cb3671 (Library/SaveData: Implement SaveDataFunction)
#include <prim/seadSafeString.h>

namespace al {
class SaveDataSequenceBase;
class SaveDataSequenceInitDir;
class SaveDataSequenceFormat;
class SaveDataSequenceRead;
class SaveDataSequenceWrite;
class AsyncFunctorThread;

class SaveDataDirector {
public:
    SaveDataDirector(u32 workBufferSize, s32 threadPriority);

    void threadFunc();
    void initCheckSaveData();
    bool requestInitSaveDir(const char* fileName, u32 dirSize, u32 version);
    bool initSaveDirSync(const char* fileName, u32 dirSize, u32 version);
    bool requestFormat(s32, s32);
    bool formatSync(s32, s32);
    bool requestRead(const char* fileName, u32 readSize, u32 version);
    bool readSync(const char* fileName, u32 readSize, u32 version);
    bool requestWrite(const char* fileName, u32 writeSize, u32 version, bool isFlushNeeded);
    bool requestFlush();
    bool writeSync(const char* fileName, u32 writeSize, u32 version);
    bool updateSequence();
    bool isDoneSequence() const;
    u8* getWorkBuffer();
    s32 getResult();

    SaveDataSequenceRead* getReadSequence() const { return mReadSequence; }

    bool isInitialized() const { return mIsInitialized; }

    s32 getFSErrorCode() const { return mFileSystemErrorCode; }

private:
<<<<<<< HEAD
    SaveDataSequenceBase* mRunningSequence = nullptr;
    SaveDataSequenceInitDir* mInitDirSequence = nullptr;
    SaveDataSequenceFormat* mFormatSequence = nullptr;
    SaveDataSequenceRead* mReadSequence = nullptr;
    SaveDataSequenceWrite* mWriteSequence = nullptr;
    bool mIsInitialized = false;
    bool _29 = false;
    sead::Buffer<u8> mBuffer;
    sead::FixedSafeString<64> mCurrentFileName;
    AsyncFunctorThread* mSaveDataThread = nullptr;
    s32 mResult = 0;
    s32 mFileSystemErrorCode = 0;
=======
    SaveDataSequenceBase* _0;
    SaveDataSequenceInitDir* mInitDirSequence;
    SaveDataSequenceFormat* mFormatSequence;
    SaveDataSequenceRead* mReadSequence;
    SaveDataSequenceWrite* mWriteSequence;
    bool mIsInitialized;
    bool _29;
    void* filler;
    u8* mWorkBuffer;
    sead::FixedSafeString<64> _40;
    AsyncFunctorThread* _98;
    s32 mResult;
    s32 mFileSystemErrorCode;
>>>>>>> b7cb3671 (Library/SaveData: Implement SaveDataFunction)
};

static_assert(sizeof(SaveDataDirector) == 0xa8);

}  // namespace al

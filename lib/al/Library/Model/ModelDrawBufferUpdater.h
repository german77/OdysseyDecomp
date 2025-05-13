#pragma once

namespace al {
class ExecuteDirector;
class ModelDrawBufferCounter;

class ModelDrawBufferUpdater {
public:
    ModelDrawBufferUpdater(const ExecuteDirector*);
    ~ModelDrawBufferUpdater();

    ModelDrawBufferCounter* getModelDrawBufferCounter() const { return mModelDrawBufferCounter; }

    // incomplete
private:
    ModelDrawBufferCounter* mModelDrawBufferCounter;
    void* filler[2];
};

static_assert(sizeof(ModelDrawBufferUpdater) == 0x18);

}  // namespace al

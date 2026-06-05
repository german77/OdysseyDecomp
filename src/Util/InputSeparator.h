#pragma once

namespace al {
class IUseSceneObjHolder;
}  // namespace al

class InputSeparator {
public:
    InputSeparator(const al::IUseSceneObjHolder*, bool);

    void reset();
    void update();
    void updateForSnapShotMode();
    bool isTriggerUiLeft();
    void checkDominant(bool);
    bool isTriggerUiRight();
    bool isTriggerUiUp();
    bool isTriggerUiDown();
    bool isHoldUiLeft();
    bool isHoldUiRight();
    bool isHoldUiUp();
    bool isHoldUiDown();
    bool isRepeatUiLeft();
    bool isRepeatUiRight();
    bool isRepeatUiUp();
    bool isRepeatUiDown();
    bool isTriggerSnapShotMode();
    bool isTriggerIncrementPostProcessingFilterPreset();
    bool isTriggerDecrementPostProcessingFilterPreset();

private:
    char filler[0x18];
};

static_assert(sizeof(InputSeparator) == 0x18);

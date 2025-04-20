#pragma once

#include <container/seadPtrArray.h>
#include <math/seadVector.h>

namespace al {
class LiveActor;
class Resource;
struct ActorInitInfo;
class ScreenPointTarget;
class ParameterIo;
class ParameterArray;
class ParameterObj;
class ParameterBase;

class ScreenPointKeeper {
public:
    static bool isExistFile(const Resource* resource, const char* fileName);
    ScreenPointKeeper();
    void initByYaml(LiveActor*, const Resource*, const ActorInitInfo&, const char*);
    void initArray(s32);
    ScreenPointTarget* addTarget(LiveActor*, const ActorInitInfo&, const char*, f32,
                                 const sead::Vector3f*, const char*, const sead::Vector3f&);
    void update();
    void validate();
    const ScreenPointTarget* getTarget(s32 index) const;
    void invalidate();
    void validateBySystem();
    void invalidateBySystem();
    const ScreenPointTarget* getTarget(const char* targetName) const;
    bool isExistTarget(const char* targetName) const;

private:
    sead::PtrArray<ScreenPointTarget> mScreenPointTargets;
    ParameterIo* mParameterIo;
    ParameterArray* mParameterArray;
    ParameterObj* mParameterObj;
    ParameterBase* mTargetNum;
};
}  // namespace al

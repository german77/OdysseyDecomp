#pragma once

#include <basis/seadTypes.h>
#include <container/seadObjArray.h>
#include <math/seadVector.h>

namespace al {
struct ActorInitInfo;
class ScreenPointDirector;
class ScreenPointTarget;

struct ScreenPointTargetHitInfo {
    void* _0;
    ScreenPointTarget* target;
    float screenPoint;
    float directPoint;
    sead::Vector3f aa;
    sead::Vector3f bb;
};

static_assert(sizeof(ScreenPointTargetHitInfo) == 0x30);

class ScreenPointer {
public:
    ScreenPointer(const ActorInitInfo& info, const char* name);

    bool hitCheckSegment(const sead::Vector3f&, const sead::Vector3f&);
    bool hitCheckScreenCircle(const sead::Vector2f&, f32, f32);
    bool hitCheckLayoutCircle(const sead::Vector2f&, f32, f32,
                              s32 (*)(const ScreenPointTargetHitInfo*,
                                      const ScreenPointTargetHitInfo*));
    bool recheckAndSortSegment(const sead::Vector3f&, const sead::Vector3f&);
    ScreenPointTarget* getHitTarget(s32) const;
    bool isHitTarget(const ScreenPointTarget* target) const;

    s32 getHitTargetNum() const { return targetHitInfo.size(); }

private:
    ScreenPointDirector* mDirector = nullptr;
    sead::ObjArray<ScreenPointTargetHitInfo> targetHitInfo;
};

static_assert(sizeof(ScreenPointer) == 0x28);

}  // namespace al

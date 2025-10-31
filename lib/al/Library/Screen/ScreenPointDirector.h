#pragma once

#include <container/seadObjArray.h>
#include <math/seadVector.h>

namespace al {
class ScreenPointer;
class ScreenPointCheckGroup;
class ScreenPointTarget;
struct ScreenPointTargetHitInfo;

class ScreenPointDirector {
public:
    ScreenPointDirector();
    void registerTarget(ScreenPointTarget*);
    void setCheckGroup(ScreenPointTarget*);
    bool hitCheckSegment(ScreenPointer* screenPointer,
                         sead::ObjArray<ScreenPointTargetHitInfo>* targetHitInfo, s32 size,
                         const sead::Vector3f& a, const sead::Vector3f& b);
    bool hitCheckScreenCircle(ScreenPointer* screenPointer,
                              sead::ObjArray<ScreenPointTargetHitInfo>* targetHitInfo, s32 size,
                              const sead::Vector2f&, f32, f32);
    bool hitCheckLayoutCircle(ScreenPointer* screenPointer,
                              sead::ObjArray<ScreenPointTargetHitInfo>* targetHitInfo, s32 size,
                              const sead::Vector2f&, f32, f32,
                              s32 (*)(const ScreenPointTargetHitInfo*,
                                      const ScreenPointTargetHitInfo*));

private:
    ScreenPointCheckGroup* mCheckGroup;
};

static_assert(sizeof(ScreenPointDirector) == 0x8);

}  // namespace al

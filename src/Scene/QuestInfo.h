#pragma once

#include <basis/seadTypes.h>
#include <math/seadVector.h>
#include <prim/seadSafeString.h>

#include "Library/Scene/IUseSceneObjHolder.h"

namespace al {
struct ActorInitInfo;
class PlacementInfo;
class SceneObjHolder;
}  // namespace al

class QuestInfo : public al::IUseSceneObjHolder {
public:
    QuestInfo();

    void clear();
    void init(const al::ActorInitInfo& actor_info);
    void init(const al::PlacementInfo& placement_info, const al::ActorInitInfo& actor_info);
    void init(const al::PlacementInfo& placement_info, al::SceneObjHolder* scene_obj_holder);
    void setStageName(const char* stage_name);
    void setLabel(const char* obj_id);
    void copy(const QuestInfo* other);
    void end();
    bool isEqual(const QuestInfo* other) const;

    s32 getQuestNo() const { return mQuestNo; }

    const sead::Vector3f& getTrans() const { return mTrans; }

    bool isMainQuest() const { return mIsMainQuest; }

    bool isEnded() const { return mIsEnded; }

    void start() { mIsEnded = false; }

    al::SceneObjHolder* getSceneObjHolder() const override { return mSceneObjHolder; }

    void initSceneObjHolder(al::SceneObjHolder* holder) { mSceneObjHolder = holder; }

    bool isInitialized() const { return mSceneObjHolder != nullptr; }

    bool isValid() const { return !mShineLabel.isEmpty(); }

    const char* getLabel() const { return mShineLabel.cstr(); }

    const char* getStageName() const { return mShineStageName.cstr(); }

private:
    s32 mQuestNo = -1;
    sead::Vector3f mTrans = sead::Vector3f::zero;
    bool mIsMainQuest = false;
    bool mIsEnded = false;
    al::SceneObjHolder* mSceneObjHolder = nullptr;
    sead::FixedSafeString<128> mShineLabel;
    sead::FixedSafeString<128> mShineStageName;
    bool mIsSingle = false;
    sead::FixedSafeString<128> mObjId;
    sead::FixedSafeString<128> mStageName;
};

static_assert(sizeof(QuestInfo) == 0x290);

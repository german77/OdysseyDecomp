#pragma once

#include "Library/Base/StringUtil.h"
#include "Library/Message/IUseMessageSystem.h"
#include "Library/Nerve/NerveStateBase.h"

namespace nn::ui2d {
class TextureInfo;
}  // namespace nn::ui2d

namespace al {
class LayoutInitInfo;
class Scene;
class SimpleLayoutAppearWaitEnd;
class MessageSystem;
class WindowConfirm;
}  // namespace al

class CommonVerticalList;
class FooterParts;
class GameConfigData;
class GameDataHolder;
class InputSeparator;
class SimpleLayoutMenu;
class WindowConfirmData;

class StageSceneStateOption : public al::HostStateBase<al::Scene>, public al::IUseMessageSystem {
public:
    StageSceneStateOption(const char*, al::Scene*, const al::LayoutInitInfo&, FooterParts*,
                          GameDataHolder*, bool);

    void updateConfigDataInfo(const GameConfigData*);
    void killAllLayouts();
    void init() override;
    void appear() override;
    void kill() override;
    bool isModeSelectEnd() const;
    s32 getSelectedFileId() const;
    bool isChangeLanguage() const;
    void decide(const al::Nerve*, SimpleLayoutMenu*, CommonVerticalList*);
    void openConfirm(const al::Nerve*, SimpleLayoutMenu*, CommonVerticalList*);
    void cancel(const al::Nerve*, SimpleLayoutMenu*, CommonVerticalList*);
    void updateSaveDataInfo(bool);
    void changeNerve(const al::Nerve*, SimpleLayoutMenu*, CommonVerticalList*);

    void exeOptionTop();
    void exeModeSelectSelecting();
    void exeModeSelectSelectingByHelp();
    void exeModeSelectConfirmYesNo();
    void exeModeSelectConfirmEnd();
    void exeConfig();
    void endConfig();
    void exeDataManager();
    void exeSaveDataSelecting();
    void exeSaveDataConfirmYesNo();
    void exeSaveDataSaving();
    void exeSaveDataSaved();
    void exeLoadDataSelecting();
    void exeLoadDataConfirmNg();
    void exeLoadDataConfirmYesNo();
    void exeLoadDataSaving();
    void exeDeleteDataSelecting();
    void exeDeleteDataConfirmNg();
    void exeDeleteDataConfirmYesNo();
    void exeDeleteDataDeleting();
    void exeDeleteDataDeleted();
    void exeLanguageSetting();
    void exeLanguageSettingConfirmYesNo();
    void exeWaitEndDecideAnim();
    void exeWaitEndDecideAnimAndAutoSave();
    void exeWaitEndAutoSave();
    void exeClose();

    const al::MessageSystem* getMessageSystem() const override { return mMessageSystem; }

    const char* getLanguage() const { return mLanguage; };

    void set_51(bool isTrue) { mIsOptionMode = isTrue; }

    bool isLoadData() const { return mIsLoadData; }

private:
    const al::Nerve* mActiveNerve = nullptr;
    SimpleLayoutMenu* mActiveLayout = nullptr;
    CommonVerticalList* mActiveList = nullptr;
    const char* field_40 = nullptr;
    const char* field_48 = nullptr;
    bool field_50;
    bool mIsOptionMode = false;
    FooterParts* mFooterParts;
    al::WindowConfirm* mWindowConfirm = nullptr;
    SimpleLayoutMenu* mOptionSelect = nullptr;
    CommonVerticalList* field_70 = nullptr;
    SimpleLayoutMenu* field_78 = nullptr;
    CommonVerticalList* field_80 = nullptr;
    SimpleLayoutMenu* mOptionMode = nullptr;
    CommonVerticalList* field_90 = nullptr;
    al::SimpleLayoutAppearWaitEnd* mMenuGuide = nullptr;
    FooterParts* mParFooter = nullptr;
    SimpleLayoutMenu* mOptionConfig = nullptr;
    CommonVerticalList* mCtrlSettingsList = nullptr;
    SimpleLayoutMenu* field_b8 = nullptr;
    CommonVerticalList* field_c0 = nullptr;
    SimpleLayoutMenu* field_c8 = nullptr;
    bool field_d0 = false;
    WindowConfirmData* mWindowConfirmData = nullptr;
    SimpleLayoutMenu* field_e0 = nullptr;
    CommonVerticalList* field_e8 = nullptr;
    sead::WFixedSafeString<512>* field_f0 = nullptr;
    sead::WFixedSafeString<512>* field_f8 = nullptr;
    sead::WFixedSafeString<512>* field_100 = nullptr;
    sead::WFixedSafeString<512>* field_108 = nullptr;
    sead::WFixedSafeString<512>* field_110 = nullptr;
    nn::ui2d::TextureInfo* mTextureInfo = nullptr;
    void* field_120 = nullptr;
    void* field_128 = nullptr;
    void* field_130 = nullptr;
    void* field_138 = nullptr;
    s32 field_140 = 0;
    s32 field_144 = 0;
    SimpleLayoutMenu* field_148 = nullptr;
    FooterParts* field_150 = nullptr;
    CommonVerticalList* field_158 = nullptr;
    al::WindowConfirm* field_160 = nullptr;
    const char* mLanguage = nullptr;
    al::Scene* mScene;
    GameDataHolder* mGameDataHolder;
    bool mIsLoadData;
    al::MessageSystem* mMessageSystem;
    InputSeparator* mInputSeparator;
};

static_assert(sizeof(StageSceneStateOption) == 0x198);

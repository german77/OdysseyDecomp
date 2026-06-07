#include "Scene/StageSceneStateOption.h"

#include "Library/Layout/LayoutActionFunction.h"
#include "Library/Layout/LayoutActorUtil.h"
#include "Library/Layout/LayoutInitInfo.h"
#include "Library/Message/LanguageUtil.h"
#include "Library/Message/MessageHolder.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Play/Layout/RollParts.h"
#include "Library/Play/Layout/SimpleLayoutAppearWaitEnd.h"
#include "Library/Play/Layout/WindowConfirm.h"
#include "Library/Resource/Resource.h"
#include "Library/Resource/ResourceFunction.h"
#include "Library/Scene/Scene.h"

#include "Layout/CommonVerticalList.h"
#include "Layout/FooterParts.h"
#include "Layout/SimpleLayoutMenu.h"
#include "Layout/WindowConfirmData.h"
#include "System/GameConfigData.h"
#include "System/GameDataFile.h"
#include "System/GameDataFunction.h"
#include "System/GameDataHolder.h"
#include "System/GameDataUtil.h"
#include "System/SaveDataAccessFunction.h"
#include "Util/InputSeparator.h"
#include "Util/ScenePrepoFunction.h"
#include "Util/StageInputFunction.h"

namespace {
NERVE_IMPL(StageSceneStateOption, OptionTop)
NERVE_IMPL(StageSceneStateOption, ModeSelectSelecting)
NERVE_IMPL(StageSceneStateOption, ModeSelectSelectingByHelp)
NERVE_IMPL(StageSceneStateOption, ModeSelectConfirmYesNo)
NERVE_IMPL(StageSceneStateOption, ModeSelectConfirmEnd)
NERVE_IMPL(StageSceneStateOption, Config)
NERVE_IMPL(StageSceneStateOption, DataManager)
NERVE_IMPL(StageSceneStateOption, SaveDataSelecting)
NERVE_IMPL(StageSceneStateOption, SaveDataConfirmYesNo)
NERVE_IMPL(StageSceneStateOption, SaveDataSaving)
NERVE_IMPL(StageSceneStateOption, SaveDataSaved)
NERVE_IMPL(StageSceneStateOption, LoadDataSelecting)
NERVE_IMPL(StageSceneStateOption, LoadDataConfirmNg)
NERVE_IMPL(StageSceneStateOption, LoadDataConfirmYesNo)
NERVE_IMPL(StageSceneStateOption, LoadDataSaving)
NERVE_IMPL(StageSceneStateOption, DeleteDataSelecting)
NERVE_IMPL(StageSceneStateOption, DeleteDataConfirmNg)
NERVE_IMPL(StageSceneStateOption, DeleteDataConfirmYesNo)
NERVE_IMPL(StageSceneStateOption, DeleteDataDeleting)
NERVE_IMPL(StageSceneStateOption, DeleteDataDeleted)
NERVE_IMPL(StageSceneStateOption, LanguageSetting)
NERVE_IMPL(StageSceneStateOption, LanguageSettingConfirmYesNo)
NERVE_IMPL(StageSceneStateOption, WaitEndDecideAnim)
NERVE_IMPL(StageSceneStateOption, WaitEndDecideAnimAndAutoSave)
NERVE_IMPL(StageSceneStateOption, WaitEndAutoSave)
NERVE_IMPL(StageSceneStateOption, Close)

NERVES_MAKE_STRUCT(StageSceneStateOption, ModeSelectSelecting, DataManager, SaveDataSelecting,
                   LoadDataSelecting, DeleteDataSelecting, OptionTop, ModeSelectSelectingByHelp,
                   LanguageSettingConfirmYesNo, ModeSelectConfirmEnd, Close, ModeSelectConfirmYesNo,
                   SaveDataConfirmYesNo, SaveDataSaving, LoadDataConfirmNg, LoadDataConfirmYesNo,
                   LoadDataSaving, DeleteDataConfirmNg, DeleteDataConfirmYesNo, DeleteDataDeleting,
                   WaitEndDecideAnimAndAutoSave, WaitEndDecideAnim)
NERVES_MAKE_NOSTRUCT(StageSceneStateOption, SaveDataSaved, DeleteDataDeleted, WaitEndAutoSave,
                     LanguageSetting, Config);
}  // namespace

template <s32 capacity>
struct MysteryObj {
    s32 size = capacity;
    s32 nan = 0;
    sead::WFixedSafeString<512> txt[capacity];
};

struct Options {
    const char* name;
    const al::Nerve* nerve;
};

const Options options[7] = {
    {"PlayMode", &NrvStageSceneStateOption.ModeSelectSelecting},
    {"Data", &NrvStageSceneStateOption.DataManager},
    {"Config", &Config},
    {"Language", &LanguageSetting},
    {"Save", &NrvStageSceneStateOption.SaveDataSelecting},
    {"Load", &NrvStageSceneStateOption.LoadDataSelecting},
    {"Delete", &NrvStageSceneStateOption.DeleteDataSelecting},
};

StageSceneStateOption::StageSceneStateOption(const char* name, al::Scene* scene,
                                             const al::LayoutInitInfo& initInfo,
                                             FooterParts* footerParts,
                                             GameDataHolder* gameDataHolder, bool)
    : al::HostStateBase<al::Scene>(name, scene), mFooterParts(footerParts), mScene(scene),
      mGameDataHolder(gameDataHolder) {
    mMessageSystem = initInfo.getMessageSystem();
    mWindowConfirm = new al::WindowConfirm(initInfo, "WindowConfirm", "確認画面[メニュー]");
    mTopMenu = new SimpleLayoutMenu("設定画面[トップ]", "OptionSelect", initInfo, nullptr, false);
    mTopList = new CommonVerticalList(mTopMenu, initInfo, true);
    al::setPaneSystemMessage(mTopMenu, "TxtOption", "MenuOption", "OptionTop");

    MysteryObj<4>* mystery = new MysteryObj<4>();
    mTopList->initDataNoResetSelected(4);
    al::StringTmp<256> tmp("%s_%s", "OptionTop", "PlayMode");
    al::copyMessageWithTag(mystery->txt[0].getBuffer(), mystery->txt[0].getBufferSize(),
                           al::getSystemMessageString(this, "MenuOption", tmp.cstr()));
    al::StringTmp<256> tmp2("%s_%s", "OptionTop", "Data");
    al::copyMessageWithTag(mystery->txt[1].getBuffer(), mystery->txt[1].getBufferSize(),
                           al::getSystemMessageString(this, "MenuOption", tmp2.cstr()));
    al::StringTmp<256> tmp3("%s_%s", "OptionTop", "Config");
    al::copyMessageWithTag(mystery->txt[2].getBuffer(), mystery->txt[2].getBufferSize(),
                           al::getSystemMessageString(this, "MenuOption", tmp3.cstr()));
    al::StringTmp<256> tmp4("%s_%s", "OptionTop", "Language");
    al::copyMessageWithTag(mystery->txt[3].getBuffer(), mystery->txt[3].getBufferSize(),
                           al::getSystemMessageString(this, "MenuOption", tmp4.cstr()));
    mTopList->addStringData(mystery->txt, "TxtContent");

    mModeMenu =
        new SimpleLayoutMenu("設定画面[モード選択]", "OptionMode", initInfo, nullptr, false);
    mModeMenuHelp = new SimpleLayoutMenu("設定画面[モード選択(ヘルプから)]", "OptionMode", initInfo,
                                         "ByHelp", false);
    mModeList = new CommonVerticalList(mModeMenu, initInfo, true);
    mModeListHelp = new CommonVerticalList(mModeMenuHelp, initInfo, true);
    mModeList->initDataNoResetSelected(2);
    mModeListHelp->initDataNoResetSelected(2);
    mModeList->set_c8(0.8f);
    mModeListHelp->set_c8(0.8f);
    al::setPaneSystemMessage(mModeMenu, "TxtOption", "MenuOption", "PlayMode");
    al::setPaneSystemMessage(mModeMenuHelp, "TxtOption", "MenuOption", "PlayMode");

    MysteryObj<2>* mystery2 = new MysteryObj<2>();
    al::copyMessageWithTag(mystery2->txt[0].getBuffer(), mystery->txt[0].getBufferSize(),
                           al::getSystemMessageString(this, "MenuOption", "PlayMode_Normal"));
    al::copyMessageWithTag(mystery2->txt[1].getBuffer(), mystery->txt[1].getBufferSize(),
                           al::getSystemMessageString(this, "MenuOption", "PlayMode_Kids"));
    mModeList->addStringData(mystery2->txt, "TxtContent00");
    mModeListHelp->addStringData(mystery2->txt, "TxtContent00");

    MysteryObj<2>* mystery3 = new MysteryObj<2>();
    al::copyMessageWithTag(
        mystery3->txt[0].getBuffer(), mystery->txt[0].getBufferSize(),
        al::getSystemMessageString(this, "MenuOption", "PlayMode_Normal_Explain"));
    al::copyMessageWithTag(mystery3->txt[1].getBuffer(), mystery->txt[1].getBufferSize(),
                           al::getSystemMessageString(this, "MenuOption", "PlayMode_Kids_Explain"));
    mModeList->addStringData(mystery3->txt, "TxtContent01");
    mModeListHelp->addStringData(mystery3->txt, "TxtContent01");

    al::startAction(mModeList->getParts(0), "Normal", "Mode");
    al::startAction(mModeListHelp->getParts(0), "Normal", "Mode");
    al::startAction(mModeList->getParts(1), "Assist", "Mode");
    al::startAction(mModeListHelp->getParts(1), "Assist", "Mode");
    al::startAction(mModeList->getParts(-1), "Hide", nullptr);
    al::startAction(mModeListHelp->getParts(-1), "Hide", nullptr);
    al::startAction(mModeList->getParts(2), "Hide", nullptr);
    al::startAction(mModeListHelp->getParts(2), "Hide", nullptr);
    // mModeList->getParts(-1)->setMsg("Hide");
    // mModeListHelp->getParts(-1)->setMsg("Hide");
    // mModeList->getParts(2)->setMsg("Hide");
    // mModeListHelp->getParts(2)->setMsg("Hide");

    mModeHelpGuideLayout = new al::SimpleLayoutAppearWaitEnd(
        "選択画面[モード選択用フッター(ヘルプから)]", "MenuGuide", initInfo, "ByHelp", false);
    mModeHelpFooter = new FooterParts(
        mModeHelpGuideLayout, initInfo,
        al::getSystemMessageString(this, "Footer", "Choice_Back_Decide"), "TxtGuide", "ParFooter");
    mConfigMenu =
        new SimpleLayoutMenu("設定画面[コンフィグ]", "OptionConfig", initInfo, nullptr, false);
    mConfigList = new CommonVerticalList(mConfigMenu, initInfo, true);
    mConfigList->set_c8(0.03f);
    al::setPaneSystemMessage(mConfigMenu, "TxtOption", "MenuOption", "Config");

    MysteryObj<11>* mystery4 = new MysteryObj<11>();

    bool* enableData = new bool[mystery4->size];
    RollPartsData* rollPartsData = new RollPartsData[mystery4->size];
    for (s32 i = 0; i < mystery4->size; i++) {
    }

    mConfigList->initDataNoResetSelected(0xb);
    mConfigList->addStringData(mystery4->txt, "TxtContent");
    mConfigList->setEnableData(enableData);
    mConfigList->startLoopActionAll("Loop", "Loop");
    mConfigList->setRollPartsData(rollPartsData);
    updateConfigDataInfo(rs::getGameConfigData(mConfigMenu));

    mInputSeparator = new InputSeparator(getHost(), true);
    mAutoSaveWaitMenu = new SimpleLayoutMenu("設定画面[データ|オートセーブ待ち]", "OptionProcess",
                                             initInfo, nullptr, false);
    mDataTopMenu =
        new SimpleLayoutMenu("設定画面[データ|トップ]", "OptionSelect", initInfo, nullptr, false);
    mDataTopList = new CommonVerticalList(mDataTopMenu, initInfo, true);

    MysteryObj<3>* mystery5 = new MysteryObj<3>();
    mDataTopList->initDataNoResetSelected(3);
    al::StringTmp<256> tmp5("%s_%s", "Data", "Save");
    al::copyMessageWithTag(mystery5->txt[0].getBuffer(), mystery5->txt[0].getBufferSize(),
                           al::getSystemMessageString(this, "MenuOption", tmp5.cstr()));
    al::StringTmp<256> tmp6("%s_%s", "Data", "Load");
    al::copyMessageWithTag(mystery5->txt[1].getBuffer(), mystery5->txt[1].getBufferSize(),
                           al::getSystemMessageString(this, "MenuOption", tmp6.cstr()));
    al::StringTmp<256> tmp7("%s_%s", "Data", "Delete");
    al::copyMessageWithTag(mystery5->txt[2].getBuffer(), mystery5->txt[2].getBufferSize(),
                           al::getSystemMessageString(this, "MenuOption", tmp7.cstr()));
    mDataTopList->addStringData(mystery5->txt, "TxtContent");

    mFileSelectConfirmWindow =
        new WindowConfirmData(initInfo, "WindowConfirmData", "セーブデータ確認画面", true);
    mFileSelectMenu =
        new SimpleLayoutMenu("設定画面[セーブデータ]", "OptionData", initInfo, nullptr, false);
    mFileSelectList = new CommonVerticalList(mFileSelectMenu, initInfo, true);
    mFileSelectList->initDataNoResetSelected(5);

    field_f0 = (new MysteryObj<5>())->txt;
    field_f8 = (new MysteryObj<5>())->txt;
    field_100 = (new MysteryObj<5>())->txt;
    field_108 = (new MysteryObj<5>())->txt;
    field_110 = (new MysteryObj<5>())->txt;

    mFileSelectList->addStringData(field_f0, "TxtNumber");
    mFileSelectList->addStringData(field_f8, "TxtWorld");
    mFileSelectList->addStringData(field_100, "TxtShine");
    mFileSelectList->addStringData(field_108, "TxtDay");
    mFileSelectList->addStringData(field_110, "TxtPlay");

    // mTextureInfo = new nn::ui2d::TextureInfo();
    al::Resource* resource = al::findOrCreateResource("ObjectData/TextureSaveData", nullptr);

    for (s32 i = 0; i < 5; i++) {
    }

    MysteryObj<12>* mastery = new MysteryObj<12>();
    killAllLayouts();
}

inline s32 getStickSensitivityPartId(const GameConfigData* config) {
    s32 level = config->getCameraStickSensitivityLevel();
    if (level == -2)
        return 0;
    if (level == -1)
        return 1;
    if (level == 0)
        return -2;
    if (level == 1)
        return 3;
    if (level == 2)
        return 4;
    return -1;
}

inline s32 getCameraGyroSensitivityPartId(const GameConfigData* config) {
    s32 level = config->getCameraGyroSensitivityLevel();
    if (level == -2)
        return -1;
    if (level == -1)
        return 0;
    if (level == 0)
        return -1;
    if (level == 1)
        return 2;
    if (level == 2)
        return 3;
    return -2;
}

inline s32 getPadRumbleLevelPartId(const GameConfigData* config) {
    s32 level = config->getPadRumbleLevel();
    if (level == -2)
        return -1;
    if (level == -1)
        return 0;
    if (level == 0)
        return -1;
    if (level == 1)
        return 2;
    if (level == 2)
        return 3;
    return -2;
}

void StageSceneStateOption::updateConfigDataInfo(const GameConfigData* config) {
    mConfigList->setRollPartsSelected(getStickSensitivityPartId(config), 1);
    mConfigList->setRollPartsSelected(config->isCameraReverseInputH() ? 1 : 0, 2);
    mConfigList->setRollPartsSelected(config->isCameraReverseInputV() ? 1 : 0, 3);
    mConfigList->setRollPartsSelected(config->isValidCameraGyro() ? 0 : 1, 4);
    mConfigList->setRollPartsSelected(getCameraGyroSensitivityPartId(config), 5);
    mConfigList->setRollPartsSelected(config->isValidPadRumble() ? 0 : 1, 7);
    mConfigList->setRollPartsSelected(getPadRumbleLevelPartId(config), 8);
    mConfigList->setRollPartsSelected(config->isUseOpenListAdditionalButton() ? 0 : 1, 10);
}

void StageSceneStateOption::killAllLayouts() {
    mWindowConfirm->kill();
    mTopMenu->kill();
    mModeMenu->kill();
    mModeMenuHelp->kill();
    mConfigMenu->kill();
    mDataTopMenu->kill();
    mFileSelectConfirmWindow->kill();
    mFileSelectMenu->kill();
    mAutoSaveWaitMenu->kill();
    field_148->kill();
    field_150->kill();
    field_160->kill();
}

void StageSceneStateOption::init() {
    initNerve(&NrvStageSceneStateOption.OptionTop, 0);
}

void StageSceneStateOption::appear() {
    NerveStateBase::appear();
    if (mIsCalledByHelp) {
        mModeMenuHelp->startAppear("Appear");
        mModeHelpGuideLayout->appear();
        mModeHelpFooter->appear();
        al::setNerve(this, &NrvStageSceneStateOption.ModeSelectSelectingByHelp);
    } else {
        mTopMenu->startAppear("Appear");
        al::setNerve(this, &NrvStageSceneStateOption.OptionTop);
    }
}

void StageSceneStateOption::kill() {
    if (!al::isNerve(this, &NrvStageSceneStateOption.LanguageSettingConfirmYesNo)) {
        rs::trySavePrepoSettingsState(mGameDataHolder->getGameDataFile()->isKidsMode(),
                                      al::getLanguageString(),
                                      *mGameDataHolder->getGameConfigData(),
                                      GameDataFunction::getSaveDataIdForPrepo(getHost()),
                                      GameDataFunction::getPlayTimeAcrossFile(getHost()));
    }
    NerveStateBase::kill();
    mAppearAnimName = nullptr;
    mEndAnimName = nullptr;
    mNextNerve = nullptr;
    mActiveLayout = nullptr;
    mActiveList = nullptr;
    if (mIsCalledByHelp)
        mIsCalledByHelp = false;
}

bool StageSceneStateOption::isModeSelectEnd() const {
    return al::isNerve(this, &NrvStageSceneStateOption.ModeSelectConfirmEnd);
}

s32 StageSceneStateOption::getSelectedFileId() const {
    return mFileSelectList->getSelectedId();
}

bool StageSceneStateOption::isChangeLanguage() const {
    return !isDead() && al::isNerve(this, &NrvStageSceneStateOption.LanguageSettingConfirmYesNo);
}

void StageSceneStateOption::decideImpl_(const al::Nerve* nerve, SimpleLayoutMenu* layout,
                                        CommonVerticalList* list, bool autoSave) {
    mAppearAnimName = "RightIn";
    mEndAnimName = "RightOut";
    list->decide();
    if (autoSave) {
        changeNerve(nerve, layout, list);
    } else {
        mNextNerve = nerve;
        mActiveLayout = layout;
        mActiveList = list;
        al::setNerve(this, &NrvStageSceneStateOption.WaitEndDecideAnim);
    }
}

void StageSceneStateOption::decide(const al::Nerve* nerve, SimpleLayoutMenu* layout,
                                   CommonVerticalList* list) {
    decideImpl_(nerve, layout, list, true);
}

void StageSceneStateOption::openConfirm(const al::Nerve* nerve, SimpleLayoutMenu* layout,
                                        CommonVerticalList* list) {
    mAppearAnimName = nullptr;
    mEndAnimName = nullptr;
    list->decide();
    list->update();
    changeNerve(nerve, layout, list);
}

void StageSceneStateOption::cancel(const al::Nerve* nerve, SimpleLayoutMenu* layout,
                                   CommonVerticalList* list) {
    mAppearAnimName = "LeftIn";
    mEndAnimName = "LeftOut";
    list->deactivate();
    changeNerve(nerve, layout, list);
}

void StageSceneStateOption::updateSaveDataInfo(bool isDummy) {
    const char* objName = isDummy ? "Data_File_Empty" : "Data_File_Empty_Load";
    for (s32 i = 0; i < 5; i++) {
        al::replaceMessageTagScore(
            &field_f0[i], mFileSelectMenu,
            al::getSystemMessageString(this, "MenuOption", "Data_File_Number"), i + 1,
            "FileNumber");
        al::Resource* resource = al::findResource("ObjectData/TextureSaveData");
        nn::g3d::ResFile* resFile;
        sead::FixedSafeString<128> fileName;
        GameDataFunction::makeTextureSaveDataFileName(&fileName, resFile, mGameDataHolder, i);
    }
}

void StageSceneStateOption::changeNerve(const al::Nerve* nerve, SimpleLayoutMenu* layout,
                                        CommonVerticalList* list) {
    mNextNerve = nerve;
    mActiveLayout = layout;
    mActiveList = list;

    if ((nerve == &NrvStageSceneStateOption.LoadDataSelecting ||
         nerve == &NrvStageSceneStateOption.DeleteDataSelecting ||
         nerve == &NrvStageSceneStateOption.SaveDataSelecting) &&
        !SaveDataAccessFunction::isDoneSave(mGameDataHolder)) {
        al::setNerve(this, &NrvStageSceneStateOption.WaitEndDecideAnimAndAutoSave);
        return;
    }

    al::setNerve(this, &NrvStageSceneStateOption.WaitEndDecideAnim);
}

void updateListCursor(CommonVerticalList* list, al::IUseSceneObjHolder* holder) {
    if (rs::isTriggerUiUp(holder) && list->getSelectedId() == list->getFirstId()) {
        list->jumpBottom();
        return;
    }

    if (rs::isTriggerUiDown(holder) && list->getSelectedId() == list->getLastId() - 1) {
        list->jumpTop();
        return;
    }

    if (rs::isHoldUiUp(holder)) {
        if (rs::isRepeatUiUp(holder))
            list->up();
        return;
    }

    if (rs::isHoldUiDown(holder) && rs::isRepeatUiDown(holder))
        list->down();
}

inline void updateListCursor(CommonVerticalList* list, InputSeparator* mInputSeparator) {
    if (mInputSeparator->isTriggerUiUp() && list->getSelectedId() == list->getFirstId()) {
        list->jumpBottom();
        return;
    }

    if (mInputSeparator->isTriggerUiDown() && list->getSelectedId() == list->getLastId() - 1) {
        list->jumpTop();
        return;
    }

    if (mInputSeparator->isHoldUiUp()) {
        if (mInputSeparator->isRepeatUiUp())
            list->up();
        return;
    }

    if (mInputSeparator->isHoldUiDown() && mInputSeparator->isRepeatUiDown())
        list->down();
}

void StageSceneStateOption::exeOptionTop() {
    if (al::isFirstStep(this)) {
        if (mActiveLayout) {
            mActiveLayout->kill();
            mActiveLayout = nullptr;
        }
        if (!mTopMenu->isWait() && mAppearAnimName) {
            mTopMenu->startAppear(mAppearAnimName);
            mAppearAnimName = nullptr;
        }

        mTopList->activate();
        mFooterParts->tryChangeTextFade(
            al::getSystemMessageString(this, "Footer", "Choice_Back_Decide"));
    }

    mTopList->update();
    if (al::isStep(this, 6))
        mTopList->appearCursor();

    if (!al::isLessStep(this, 6)) {
        updateListCursor(mTopList, getHost());

        if (rs::isTriggerUiDecide(getHost())) {
            al::startHitReaction(mTopMenu, "決定", nullptr);
            mTopList->endCursor();
            decideImpl_(options[mTopList->getSelectedId()].nerve, mTopMenu, mTopList, false);
            return;
        }

        if (rs::isTriggerUiCancel(getHost())) {
            al::startHitReaction(mTopMenu, "キャンセル");
            mFooterParts->tryChangeTextFade(u"");
            al::setNerve(this, &NrvStageSceneStateOption.Close);
        }
    }
}

void StageSceneStateOption::exeModeSelectSelecting() {}

void StageSceneStateOption::exeModeSelectSelectingByHelp() {
    if (al::isFirstStep(this)) {
        mModeListHelp->activate();
        bool isKidsMode = rs::isKidsMode(mModeMenuHelp);
        al::startAction(mModeListHelp->getParts(0), isKidsMode ? "Off" : "On", "State");
        al::startAction(mModeListHelp->getParts(1), isKidsMode ? "On" : "On", "State");
        mModeListHelp->setSelectedIdx(isKidsMode ? 1 : 0, 0);
    } else if (al::isActionPlaying(mModeListHelp->getParts(mModeListHelp->getSelectedId()),
                                   "CheckOn", "Main")) {
        al::RollParts* selectedParts = mModeListHelp->getParts(mModeListHelp->getSelectedId());
        if (!al::isActionEnd(selectedParts, "Main"))
            return;
        al::startAction(selectedParts, "Select", nullptr);
        al::setActionFrame(selectedParts, al::getActionFrameMax(selectedParts, "Select", nullptr));
    }

    mModeListHelp->update();

    if (al::isStep(this, 6))
        mModeListHelp->appearCursor();

    if (!al::isLessStep(this, 6))
        return;

    updateListCursor(mModeListHelp, getHost());

    if (rs::isTriggerUiDecide(getHost())) {
        if (mModeListHelp->getSelectedId() != rs::isKidsMode(mModeMenuHelp)) {
            al::startAction(mModeListHelp->getParts(mModeListHelp->getSelectedId()), "CheckOn",
                            "Main");
            return;
        }

        al::startHitReaction(mModeMenuHelp, "モード切替", nullptr);
        mModeListHelp->endCursor();
        al::startAction(mModeListHelp->getParts(0),
                        mModeListHelp->getSelectedId() == 1 ? "On" : "Off", "State");
        al::startAction(mModeListHelp->getParts(1),
                        mModeListHelp->getSelectedId() == 1 ? "Off" : "On", "State");
        openConfirm(&NrvStageSceneStateOption.ModeSelectConfirmYesNo, mModeMenuHelp, mModeListHelp);
        return;
    }

    if (rs::isTriggerUiCancel(getHost())) {
        openConfirm(&NrvStageSceneStateOption.Close, mModeMenuHelp, mModeListHelp);
        return;
    }
}

void StageSceneStateOption::exeModeSelectConfirmYesNo() {
    CommonVerticalList* list = mIsCalledByHelp ? mModeListHelp : mModeList;
    if (al::isFirstStep(this)) {
        mWindowConfirm->setListNum(2);
        mWindowConfirm->setTxtMessage(al::getSystemMessageString(
            this, "ConfirMessage",
            list->getSelectedId() == 0 ? "PlayMode_Confirm_Normal" : "PlayMode_Confirm_Kids"));
        mWindowConfirm->setTxtList(
            0, al::getSystemMessageString(this, "ConfirMessage", "PlayMode_Confirm_Yes"));
        mWindowConfirm->setTxtList(
            1, al::getSystemMessageString(this, "ConfirMessage", "PlayMode_Confirm_No"));
        mWindowConfirm->appear();
    }

    if (mWindowConfirm->isNerveEnd()) {
        if (mWindowConfirm->GetSelectionIdx() != mWindowConfirm->GetCancelIdx()) {
            if (list->getSelectedId() == 0)
                GameDataFunction::setKidsModeOff(getHost());
            else
                GameDataFunction::setKidsModeOn(getHost());
            rs::trySavePrepoSettingsState(mGameDataHolder->getGameDataFile()->isKidsMode(),
                                          al::getLanguageString(),
                                          *mGameDataHolder->getGameConfigData(),
                                          GameDataFunction::getSaveDataIdForPrepo(getHost()),
                                          GameDataFunction::getPlayTimeAcrossFile(getHost()));
            mGameDataHolder->getGameDataFile()->changeWipeType("FadeBlack");
            al::setNerve(this, &NrvStageSceneStateOption.ModeSelectConfirmEnd);
            return;
        }
        if (mIsCalledByHelp)
            al::setNerve(this, &NrvStageSceneStateOption.ModeSelectSelectingByHelp);
        else
            al::setNerve(this, &NrvStageSceneStateOption.ModeSelectSelecting);
        return;
    }

    if (rs::isTriggerUiDecide(getHost())) {
        if (mWindowConfirm->GetCancelIdx() != mWindowConfirm->GetSelectionIdx()) {
            mWindowConfirm->tryDecideWithoutEnd();
            return;
        }
        mWindowConfirm->tryCancel();
        return;
    }

    if (rs::isTriggerUiCancel(getHost())) {
        mWindowConfirm->tryCancel();
        return;
    }

    if (rs::isRepeatUiDown(getHost())) {
        mWindowConfirm->tryDown();
        return;
    }
    if (rs::isRepeatUiUp(getHost())) {
        mWindowConfirm->tryUp();
        return;
    }
}

void StageSceneStateOption::exeModeSelectConfirmEnd() {}

void StageSceneStateOption::exeConfig() {
    if (al::isFirstStep(this)) {
        if (mActiveLayout) {
            mActiveLayout->kill();
            mActiveLayout = nullptr;
        }
        mInputSeparator->reset();
        mFooterParts->tryChangeTextFade(
            al::getSystemMessageString(this, "Footer", "Choice_Reset_Black"));
        mConfigMenu->startAppear(mAppearAnimName);
        mAppearAnimName = nullptr;
        mConfigList->activate();
    }

    mConfigList->update();
    if (al::isStep(this, 6))
        mConfigList->appearCursor();

    if (al::isLessStep(this, 6))
        return;

    mInputSeparator->update();
    if (mInputSeparator->isTriggerUiUp() &&
        mConfigList->getSelectedId() == mConfigList->getFirstId()) {
        mConfigList->jumpBottom();
    } else if (mInputSeparator->isTriggerUiDown() &&
               mConfigList->getSelectedId() == mConfigList->getLastId() - 1) {
        mConfigList->jumpTop();
    } else if (mInputSeparator->isHoldUiUp()) {
        if (mInputSeparator->isRepeatUiUp())
            mConfigList->up();
    } else if (mInputSeparator->isHoldUiDown()) {
        if (mInputSeparator->isRepeatUiDown())
            mConfigList->down();
    } else if (mInputSeparator->isTriggerUiLeft()) {
        mConfigList->rollLeft();
    } else if (mInputSeparator->isTriggerUiRight()) {
        mConfigList->rollRight();
    }

    if (rs::isTriggerUiY(getHost())) {
        al::startHitReaction(mConfigMenu, "リセット", nullptr);
        GameConfigData* gameConfig = rs::getGameConfigData(mConfigMenu);
        gameConfig->init();
        updateConfigDataInfo(gameConfig);
    }

    if (rs::isTriggerUiCancel(getHost())) {
        al::startHitReaction(mConfigMenu, "キャンセル", nullptr);
        mConfigList->hideCursor();
        decideImpl_(&NrvStageSceneStateOption.OptionTop, mConfigMenu, mConfigList, false);
    }
}

void StageSceneStateOption::endConfig() {
    GameConfigData* gameConfig = rs::getGameConfigData(mConfigMenu);

    const s32 sensitivityLevel[5] = {-2, -1, 0, 1, 2};
    gameConfig->setCameraStickSensitivityLevel(
        sensitivityLevel[mConfigList->getRollPartsSelected(1)]);

    if (mConfigList->getRollPartsSelected(2) == 1)
        gameConfig->onCameraReverseInputH();
    else
        gameConfig->offCameraReverseInputH();

    if (mConfigList->getRollPartsSelected(3) == 1)
        gameConfig->onCameraReverseInputV();
    else
        gameConfig->offCameraReverseInputV();

    if (mConfigList->getRollPartsSelected(4) == 0)
        gameConfig->validateCameraGyro();
    else
        gameConfig->invalidateCameraGyro();

    if (mConfigList->getRollPartsSelected(7) == 0)
        gameConfig->validatePadRumble();
    else
        gameConfig->invalidatePadRumble();

    if (mConfigList->getRollPartsSelected(10) == 0)
        gameConfig->onUseOpenListAdditionalButton();
    else
        gameConfig->offUseOpenListAdditionalButton();

    gameConfig->setCameraGyroSensitivityLevel(
        sensitivityLevel[mConfigList->getRollPartsSelected(5) + 1]);
    gameConfig->setPadRumbleLevel(sensitivityLevel[mConfigList->getRollPartsSelected(8) + 1]);
    rs::applyGameConfigData(mScene, gameConfig);
    rs::saveGameConfigData(mConfigMenu);
}

void StageSceneStateOption::exeDataManager() {
    if (al::isFirstStep(this)) {
        if (al::isActive(mAutoSaveWaitMenu))
            mAutoSaveWaitMenu->kill();
        if (mActiveLayout) {
            mActiveLayout->kill();
            mActiveLayout = nullptr;
        }
        if (mDataTopMenu->isWait()) {
            mDataTopMenu->startAppear(mEndAnimName);
            mEndAnimName = nullptr;
        }
        mDataTopList->activate();
        mFileSelectList->setSelectedIdx(mGameDataHolder->getPlayingFileId(), 0);
    }

    mDataTopList->update();
    if (al::isStep(this, 6))
        mDataTopList->appearCursor();
    if (al::isLessStep(this, 6))
        return;

    updateListCursor(mDataTopList, getHost());

    if (rs::isTriggerUiDecide(getHost())) {
        mDataTopList->endCursor();
        al::startHitReaction(mDataTopMenu, "決定", nullptr);
        decideImpl_(options[mDataTopList->getSelectedId() + 4].nerve, mDataTopMenu, mDataTopList,
                    false);
        return;
    }

    if (rs::isTriggerUiCancel(getHost())) {
        al::startHitReaction(mDataTopMenu, "キャンセル", nullptr);
        mDataTopList->hideCursor();
        cancel(&NrvStageSceneStateOption.OptionTop, mDataTopMenu, mDataTopList);
    }
}

void exeSaveDataSelecting() {}

void StageSceneStateOption::exeSaveDataConfirmYesNo() {
    if (al::isFirstStep(this))
        mFileSelectConfirmWindow->appearWithChoicingCancel();

    if (mFileSelectConfirmWindow->isDisable()) {
        al::setNerve(this, &NrvStageSceneStateOption.SaveDataSelecting);
        return;
    }

    mFileSelectList->update();
    mFileSelectConfirmWindow->updateNerve();
    mFileSelectConfirmWindow->updateConfirmDataDate();

    if (mFileSelectConfirmWindow->isEndSelect()) {
        if (mFileSelectConfirmWindow->isDecided()) {
            s32 selectedFileId = getSelectedFileId();
            s32 playingFileId = mGameDataHolder->getPlayingFileId();
            if (selectedFileId == playingFileId)
                SaveDataAccessFunction::startSaveDataWriteWithWindow(mGameDataHolder);
            else
                SaveDataAccessFunction::startSaveDataCopyWithWindow(mGameDataHolder, playingFileId,
                                                                    selectedFileId);
            al::setNerve(this, &NrvStageSceneStateOption.SaveDataSaving);
            return;
        }
        if (mFileSelectConfirmWindow->isCanceled())
            mFileSelectConfirmWindow->end();
    }
}

void StageSceneStateOption::exeSaveDataSaving() {
    if (SaveDataAccessFunction::updateSaveDataAccess(mGameDataHolder, false))
        al::setNerve(this, &SaveDataSaved);
}

void StageSceneStateOption::exeSaveDataSaved() {
    al::RollParts* selectedParts = mFileSelectList->getSelectedParts();
    if (al::isFirstStep(this)) {
        al::startAction(selectedParts, "FadeOut", nullptr);
        mFileSelectConfirmWindow->end();
    }

    if (al::isActionPlaying(selectedParts, "FadeOut", nullptr) &&
        al::isActionEnd(selectedParts, nullptr)) {
        s32 fileId = mGameDataHolder->getPlayingFileId();
        al::Resource* resource = al::findResource("ObjectData/TextureSaveData");
        sead::BufferedSafeString fileName(nullptr, 0x100);
        GameDataFunction::makeTextureSaveDataFileName(&fileName, resource->getResFile(),
                                                      mGameDataHolder, fileId);
        // mTextureInfo[mFileSelectList->getSelectedId()]
        updateSaveDataInfo(false);
        mFileSelectList->set_cc(true);
        mFileSelectList->activate();
        mFileSelectList->update();
        al::startAction(selectedParts, "FadeIn", nullptr);
    }

    if (!mFileSelectConfirmWindow->isDisable())
        mFileSelectConfirmWindow->updateNerve();

    if (al::isActionPlaying(selectedParts, "FadeIn", nullptr) &&
        al::isActionEnd(selectedParts, nullptr) && mFileSelectConfirmWindow->isDisable()) {
        al::startAction(selectedParts, "Select", nullptr);
        al::setActionFrame(selectedParts, al::getActionFrameMax(selectedParts, "Select", nullptr),
                           nullptr);
        al::setNerve(this, &NrvStageSceneStateOption.SaveDataSelecting);
    }
}

void StageSceneStateOption::exeLoadDataSelecting() {
    if (al::isFirstStep(this)) {
        if (al::isActive(mAutoSaveWaitMenu))
            mAutoSaveWaitMenu->kill();
        if (mActiveLayout) {
            mActiveLayout->kill();
            mActiveLayout = nullptr;
        }
        if (al::isDead(mFileSelectMenu) && mAppearAnimName) {
            mFileSelectMenu->startAppear(mAppearAnimName);
            mAppearAnimName = nullptr;
        }
        al::setPaneSystemMessage(mFileSelectMenu, "TxtOption", "MenuOption", "Data_Load");
        mFileSelectList->activate();
        updateSaveDataInfo(true);
    }

    mFileSelectList->update();
    if (al::isStep(this, 6))
        mFileSelectList->appearCursor();

    if (al::isLessStep(this, 6))
        return;

    updateListCursor(mFileSelectList, getHost());

    if (rs::isTriggerUiDecide(getHost())) {
        mFileSelectList->endCursor();
        al::startHitReaction(mFileSelectMenu, "決定", nullptr);
        if (mFileSelectList->getSelectedId() == mGameDataHolder->getPlayingFileId() &&
            !mIsNewGameFile) {
            mWindowConfirm->setListNum(1);
            mWindowConfirm->setTxtMessage(al::getSystemMessageString(
                this, "ConfirmMessage", "Data_Load_NG_PlayingData_Confirm"));
            openConfirm(&NrvStageSceneStateOption.LoadDataConfirmNg, mFileSelectMenu,
                        mFileSelectList);
            return;
        }

        mFileSelectConfirmWindow->setConfirmData(mFileSelectList->getSelectedParts(),
                                                 mTextureInfo[mFileSelectList->getSelectedId()]);
        mFileSelectConfirmWindow->setConfirmMessage(
            al::getSystemMessageString(this, "ConfirmMessage", "Data_Load_Confirm"),
            al::getSystemMessageString(this, "ConfirmMessage", "Data_Load_Confirm_Yes"),
            al::getSystemMessageString(this, "ConfirmMessage", "Data_Load_Confirm_No"));
        openConfirm(&NrvStageSceneStateOption.LoadDataConfirmYesNo, mFileSelectMenu,
                    mFileSelectList);
        return;
    }

    if (rs::isTriggerUiCancel(getHost())) {
        mFileSelectList->hideCursor();
        al::startHitReaction(mFileSelectMenu, "キャンセル", nullptr);
        cancel(&NrvStageSceneStateOption.DataManager, mFileSelectMenu, mFileSelectList);
        return;
    }
}

void StageSceneStateOption::exeLoadDataConfirmNg() {
    if (al::isFirstStep(this))
        mWindowConfirm->appear();

    mFileSelectList->update();
    if (al::isDead(mWindowConfirm)) {
        al::setNerve(this, &NrvStageSceneStateOption.LoadDataSelecting);
        return;
    }

    if (rs::isTriggerUiDecide(getHost()))
        mWindowConfirm->tryDecide();

    if (rs::isTriggerUiCancel(getHost()))
        mWindowConfirm->tryCancel();
}

void StageSceneStateOption::exeLoadDataConfirmYesNo() {
    if (al::isFirstStep(this))
        mFileSelectConfirmWindow->appear();

    if (mFileSelectConfirmWindow->isDisable()) {
        al::setNerve(this, &NrvStageSceneStateOption.LoadDataSelecting);
        return;
    }

    mFileSelectList->update();
    mFileSelectConfirmWindow->updateNerve();

    if (mFileSelectConfirmWindow->isEndSelect()) {
        if (mFileSelectConfirmWindow->isDecided()) {
            al::setNerve(this, &NrvStageSceneStateOption.LoadDataSaving);
            return;
        }
        if (mFileSelectConfirmWindow->isCanceled())
            mFileSelectConfirmWindow->end();
    }
}

void StageSceneStateOption::exeLoadDataSaving() {
    if (al::isFirstStep(this)) {
        mGameDataHolder->requestSetPlayingFileId(getSelectedFileId());
        mIsLoadData = true;
        mFileSelectConfirmWindow->end();
    }
}

void StageSceneStateOption::exeDeleteDataSelecting() {}

void StageSceneStateOption::exeDeleteDataConfirmNg() {
    if (al::isFirstStep(this))
        mWindowConfirm->appear();

    mFileSelectList->update();
    if (al::isDead(mWindowConfirm)) {
        al::setNerve(this, &NrvStageSceneStateOption.DeleteDataSelecting);
        return;
    }

    if (rs::isTriggerUiDecide(getHost()))
        mWindowConfirm->tryDecide();

    if (rs::isTriggerUiCancel(getHost()))
        mWindowConfirm->tryCancel();
}

void StageSceneStateOption::exeDeleteDataConfirmYesNo() {
    if (al::isFirstStep(this))
        mFileSelectConfirmWindow->appearWithChoicingCancel();

    if (mFileSelectConfirmWindow->isDisable()) {
        al::setNerve(this, &NrvStageSceneStateOption.DeleteDataSelecting);
        return;
    }

    mFileSelectList->update();
    mFileSelectConfirmWindow->updateNerve();

    if (mFileSelectConfirmWindow->isEndSelect()) {
        if (mFileSelectConfirmWindow->isDecided()) {
            al::setNerve(this, &NrvStageSceneStateOption.DeleteDataDeleting);
            return;
        }
        if (mFileSelectConfirmWindow->isCanceled())
            mFileSelectConfirmWindow->end();
    }
}

void StageSceneStateOption::exeDeleteDataDeleting() {
    if (al::isFirstStep(this))
        SaveDataAccessFunction::startSaveDataDeleteWithWindow(mGameDataHolder, getSelectedFileId());

    if (SaveDataAccessFunction::updateSaveDataAccess(mGameDataHolder, false))
        al::setNerve(this, &DeleteDataDeleted);
}

void StageSceneStateOption::exeDeleteDataDeleted() {
    al::RollParts* selectedParts = mFileSelectList->getSelectedParts();
    if (al::isFirstStep(this)) {
        mFileSelectConfirmWindow->end();
        al::startAction(selectedParts, "FadeOut", nullptr);
    }

    if (al::isActionPlaying(selectedParts, "FadeOut", nullptr) &&
        al::isActionEnd(selectedParts, nullptr)) {
        mTextureInfo[mFileSelectList->getSelectedId()] = field_120;
        updateSaveDataInfo(false);
        mFileSelectList->set_cc(true);
        mFileSelectList->activate();
        mFileSelectList->update();
        al::startAction(selectedParts, "FadeIn", nullptr);
    }

    if (!mFileSelectConfirmWindow->isDisable())
        mFileSelectConfirmWindow->updateNerve();

    if (al::isActionPlaying(selectedParts, "FadeIn", nullptr) &&
        al::isActionEnd(selectedParts, nullptr) && mFileSelectConfirmWindow->isDisable()) {
        al::startAction(selectedParts, "Select", nullptr);
        al::setActionFrame(selectedParts, al::getActionFrameMax(selectedParts, "Select", nullptr));
        al::setNerve(this, &NrvStageSceneStateOption.DeleteDataSelecting);
    }
}

void StageSceneStateOption::exeLanguageSetting() {}

void StageSceneStateOption::exeLanguageSettingConfirmYesNo() {
    if (al::isFirstStep(this))
        field_160->appearWithChoicingCancel();

    if (field_160->isNerveEnd()) {
        if (field_160->GetSelectionIdx() == field_160->GetCancelIdx()) {
            al::setNerve(this, &LanguageSetting);
            return;
        }
        kill();
        return;
    }

    if (!rs::isTriggerUiDecide(getHost())) {
        if (!rs::isTriggerUiCancel(getHost())) {
            if (rs::isRepeatUiDown(getHost())) {
                field_160->tryDown();
                return;
            }
            if (rs::isRepeatUiUp(getHost())) {
                field_160->tryUp();
                return;
            }
            return;
        }
        field_160->tryCancel();

    }

    else {
        if (field_160->GetCancelIdx() != field_160->GetSelectionIdx()) {
            field_160->tryDecideWithoutEnd();
            return;
        }
        field_160->tryCancel();
    }
}

void StageSceneStateOption::exeWaitEndDecideAnim() {
    mActiveList->update();
    if (mActiveList->isDecideEnd() || mActiveList->isDeactive()) {
        if (!mEndAnimName) {
            al::setNerve(this, mNextNerve);
            mActiveLayout = nullptr;
            mActiveList = nullptr;
            return;
        }
        mActiveLayout->startEnd(mEndAnimName);
    }

    if (mActiveLayout->isEndWait()) {
        al::setNerve(this, mNextNerve);
        mNextNerve = nullptr;
        mActiveList = nullptr;
        mEndAnimName = nullptr;
    }
}

void StageSceneStateOption::exeWaitEndDecideAnimAndAutoSave() {
    if (al ::isFirstStep(this)) {
    }

    mActiveList->update();
    if (mActiveList->isDecideEnd() || mActiveList->isDeactive())
        mActiveLayout->startEnd(mEndAnimName);

    if (mActiveLayout->isEndWait())
        al::setNerve(this, &WaitEndAutoSave);
}

void StageSceneStateOption::exeWaitEndAutoSave() {
    if (al ::isFirstStep(this)) {
        mIsAutoSaveWaitInterrupted = false;
        mActiveLayout->kill();
        mAutoSaveWaitMenu->startAppear("Appear");
        al::startAction(mAutoSaveWaitMenu, "Loop", "Loop");
    }
    if (rs::isHoldUiCancel(getHost()) || al::isGreaterEqualStep(mAutoSaveWaitMenu, 600)) {
        mIsAutoSaveWaitInterrupted = true;
        mAutoSaveWaitMenu->startEnd("End");
    }
    if (mAutoSaveWaitMenu->isWait() && al::isGreaterEqualStep(mAutoSaveWaitMenu, 45) &&
        SaveDataAccessFunction::isDoneSave(mGameDataHolder))
        mAutoSaveWaitMenu->startEnd("End");

    if (mAutoSaveWaitMenu->isEndWait()) {
        if (mIsAutoSaveWaitInterrupted || al::isGreaterEqualStep(mAutoSaveWaitMenu, 600)) {
            mActiveList = nullptr;
            mNextNerve = nullptr;
            mActiveLayout = nullptr;
            mEndAnimName = nullptr;
            mAppearAnimName = "LeftIn";
            al::setNerve(this, &NrvStageSceneStateOption.DataManager);
            return;
        }

        al::setNerve(this, mNextNerve);
        mEndAnimName = nullptr;
        mActiveLayout = nullptr;
        mActiveList = nullptr;
        mNextNerve = nullptr;
    }
}

void StageSceneStateOption::exeClose() {
    if (al ::isFirstStep(this)) {
        if (mIsCalledByHelp) {
            mModeListHelp->hideCursor();
            mModeMenuHelp->startEnd("End");
            mModeHelpGuideLayout->end();
        } else {
            mTopList->hideCursor();
            mTopMenu->startEnd("End");
        }
    }

    if (mIsCalledByHelp) {
        if (mModeMenuHelp->isEndWait()) {
            mModeMenuHelp->kill();
            mModeHelpGuideLayout->end();
            mModeHelpFooter->kill();
            kill();
        }
        return;
    }

    if (mTopMenu->isEndWait()) {
        mTopMenu->kill();
        kill();
    }
}

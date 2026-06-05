#include "Scene/StageSceneStateOption.h"

#include "Library/Layout/LayoutActionFunction.h"
#include "Library/Layout/LayoutActorUtil.h"
#include "Library/Layout/LayoutInitInfo.h"
#include "Library/Message/LanguageUtil.h"
#include "Library/Message/MessageHolder.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Play/Layout/SimpleLayoutAppearWaitEnd.h"
#include "Library/Play/Layout/WindowConfirm.h"
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
                     LanguageSetting);
}  // namespace

template <s32 capacity>
struct MysteryObj {
    s32 size = capacity;
    s32 nan = 0;
    sead::WFixedSafeString<512> txt[capacity];
};

StageSceneStateOption::StageSceneStateOption(const char* name, al::Scene* scene,
                                             const al::LayoutInitInfo& initInfo,
                                             FooterParts* footerParts,
                                             GameDataHolder* gameDataHolder, bool)
    : al::HostStateBase<al::Scene>(name, scene), mFooterParts(footerParts), mScene(scene),
      mGameDataHolder(gameDataHolder) {
    mMessageSystem = initInfo.getMessageSystem();
    mWindowConfirm = new al::WindowConfirm(initInfo, "WindowConfirm", "確認画面[メニュー]");
    mOptionSelect =
        new SimpleLayoutMenu("設定画面[トップ]", "OptionSelect", initInfo, nullptr, false);
    field_70 = new CommonVerticalList(mOptionSelect, initInfo, true);
    al::setPaneSystemMessage(mOptionSelect, "TxtOption", "MenuOption", "OptionTop");

    MysteryObj<4>* mystery = new MysteryObj<4>();
    field_70->initDataNoResetSelected(4);
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
    field_70->addStringData(mystery->txt, "TxtContent");

    field_78 = new SimpleLayoutMenu("設定画面[モード選択]", "OptionMode", initInfo, nullptr, false);
    mOptionMode = new SimpleLayoutMenu("設定画面[モード選択(ヘルプから)]", "OptionMode", initInfo,
                                       "ByHelp", false);
    field_80 = new CommonVerticalList(field_78, initInfo, true);
    field_90 = new CommonVerticalList(mOptionMode, initInfo, true);
    field_80->initDataNoResetSelected(2);
    field_90->initDataNoResetSelected(2);
    field_80->set_c8(0.8f);
    field_90->set_c8(0.8f);
    al::setPaneSystemMessage(field_78, "TxtOption", "MenuOption", "PlayMode");
    al::setPaneSystemMessage(mOptionMode, "TxtOption", "MenuOption", "PlayMode");

    MysteryObj<2>* mystery2 = new MysteryObj<2>();
    al::copyMessageWithTag(mystery2->txt[0].getBuffer(), mystery->txt[0].getBufferSize(),
                           al::getSystemMessageString(this, "MenuOption", "PlayMode_Normal"));
    al::copyMessageWithTag(mystery2->txt[1].getBuffer(), mystery->txt[1].getBufferSize(),
                           al::getSystemMessageString(this, "MenuOption", "PlayMode_Kids"));
    field_80->addStringData(mystery2->txt, "TxtContent00");
    field_90->addStringData(mystery2->txt, "TxtContent00");

    MysteryObj<2>* mystery3 = new MysteryObj<2>();
    al::copyMessageWithTag(
        mystery3->txt[0].getBuffer(), mystery->txt[0].getBufferSize(),
        al::getSystemMessageString(this, "MenuOption", "PlayMode_Normal_Explain"));
    al::copyMessageWithTag(mystery3->txt[1].getBuffer(), mystery->txt[1].getBufferSize(),
                           al::getSystemMessageString(this, "MenuOption", "PlayMode_Kids_Explain"));
    field_80->addStringData(mystery3->txt, "TxtContent01");
    field_90->addStringData(mystery3->txt, "TxtContent01");

    al::startAction(field_80->getParts(0), "Normal", "Mode");
    al::startAction(field_90->getParts(0), "Normal", "Mode");
    al::startAction(field_80->getParts(1), "Assist", "Mode");
    al::startAction(field_90->getParts(1), "Assist", "Mode");
    al::startAction(field_80->getParts(-1), "Hide", nullptr);
    al::startAction(field_90->getParts(-1), "Hide", nullptr);
    al::startAction(field_80->getParts(2), "Hide", nullptr);
    al::startAction(field_90->getParts(2), "Hide", nullptr);
    // field_80->getParts(-1)->setMsg("Hide");
    // field_90->getParts(-1)->setMsg("Hide");
    // field_80->getParts(2)->setMsg("Hide");
    // field_90->getParts(2)->setMsg("Hide");

    mMenuGuide = new al::SimpleLayoutAppearWaitEnd("選択画面[モード選択用フッター(ヘルプから)]",
                                                   "MenuGuide", initInfo, "ByHelp", false);
    mParFooter = new FooterParts(mMenuGuide, initInfo,
                                 al::getSystemMessageString(this, "Footer", "Choice_Back_Decide"),
                                 "TxtGuide", "ParFooter");
    mOptionConfig =
        new SimpleLayoutMenu("設定画面[コンフィグ]", "OptionConfig", initInfo, nullptr, false);
    mCtrlSettingsList = new CommonVerticalList(mOptionConfig, initInfo, true);
    mCtrlSettingsList->set_c8(0.03f);
    al::setPaneSystemMessage(mOptionConfig, "TxtOption", "MenuOption", "Config");

    MysteryObj<11>* mystery4 = new MysteryObj<11>();

    bool* enableData = new bool[mystery4->size];
    RollPartsData* rollPartsData = new RollPartsData[mystery4->size];
    for (s32 i = 0; i < mystery4->size; i++) {
    }

    mCtrlSettingsList->initDataNoResetSelected(0xb);
    mCtrlSettingsList->addStringData(mystery4->txt, "TxtContent");
    mCtrlSettingsList->setEnableData(enableData);
    mCtrlSettingsList->startLoopActionAll("Loop", "Loop");
    mCtrlSettingsList->setRollPartsData(rollPartsData);
    updateConfigDataInfo(rs::getGameConfigData(mOptionConfig));

    mInputSeparator = new InputSeparator(getHost(), true);
    field_c8 = new SimpleLayoutMenu("設定画面[データ|オートセーブ待ち]", "OptionProcess", initInfo,
                                    nullptr, false);
    field_b8 =
        new SimpleLayoutMenu("設定画面[データ|トップ]", "OptionSelect", initInfo, nullptr, false);
    field_c0 = new CommonVerticalList(field_b8, initInfo, true);

    MysteryObj<3>* mystery5 = new MysteryObj<3>();
    field_c0->initDataNoResetSelected(3);
    al::StringTmp<256> tmp5("%s_%s", "Data", "Save");
    al::copyMessageWithTag(mystery5->txt[0].getBuffer(), mystery5->txt[0].getBufferSize(),
                           al::getSystemMessageString(this, "MenuOption", tmp5.cstr()));
    al::StringTmp<256> tmp6("%s_%s", "Data", "Load");
    al::copyMessageWithTag(mystery5->txt[1].getBuffer(), mystery5->txt[1].getBufferSize(),
                           al::getSystemMessageString(this, "MenuOption", tmp6.cstr()));
    al::StringTmp<256> tmp7("%s_%s", "Data", "Delete");
    al::copyMessageWithTag(mystery5->txt[2].getBuffer(), mystery5->txt[2].getBufferSize(),
                           al::getSystemMessageString(this, "MenuOption", tmp7.cstr()));
    field_c0->addStringData(mystery5->txt, "TxtContent");

    mWindowConfirmData =
        new WindowConfirmData(initInfo, "WindowConfirmData", "セーブデータ確認画面", true);
    field_e0 =
        new SimpleLayoutMenu("設定画面[セーブデータ]", "OptionData", initInfo, nullptr, false);
    field_e8 = new CommonVerticalList(field_e0, initInfo, true);
    field_e8->initDataNoResetSelected(5);

    field_f0 = (new MysteryObj<5>())->txt;
    field_f8 = (new MysteryObj<5>())->txt;
    field_100 = (new MysteryObj<5>())->txt;
    field_108 = (new MysteryObj<5>())->txt;
    field_110 = (new MysteryObj<5>())->txt;

    field_e8->addStringData(field_f0, "TxtNumber");
    field_e8->addStringData(field_f8, "TxtWorld");
    field_e8->addStringData(field_100, "TxtShine");
    field_e8->addStringData(field_108, "TxtDay");
    field_e8->addStringData(field_110, "TxtPlay");

    //mTextureInfo = new nn::ui2d::TextureInfo();
    al::Resource* resource = al::findOrCreateResource("ObjectData/TextureSaveData", nullptr);

    for (s32 i = 0; i < 5; i++) {
    }

    MysteryObj<12>* mastery = new MysteryObj<12>();
    killAllLayouts();
}

void StageSceneStateOption::updateConfigDataInfo(const GameConfigData*) {}

void StageSceneStateOption::killAllLayouts() {
    mWindowConfirm->kill();
    mOptionSelect->kill();
    field_78->kill();
    mOptionMode->kill();
    mOptionConfig->kill();
    field_b8->kill();
    mWindowConfirmData->kill();
    field_e0->kill();
    field_c8->kill();
    field_148->kill();
    field_150->kill();
    field_160->kill();
}

void StageSceneStateOption::init() {
    initNerve(&NrvStageSceneStateOption.OptionTop, 0);
}

void StageSceneStateOption::appear() {
    NerveStateBase::appear();
    if (mIsOptionMode) {
        mOptionMode->startAppear("Appear");
        mMenuGuide->appear();
        mParFooter->appear();
        al::setNerve(this, &NrvStageSceneStateOption.ModeSelectSelectingByHelp);
    } else {
        mOptionSelect->startAppear("Appear");
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
    field_40 = nullptr;
    field_48 = nullptr;
    mActiveNerve = nullptr;
    mActiveLayout = nullptr;
    mActiveList = nullptr;
    if (mIsOptionMode)
        mIsOptionMode = false;
}

bool StageSceneStateOption::isModeSelectEnd() const {
    return al::isNerve(this, &NrvStageSceneStateOption.ModeSelectConfirmEnd);
}

s32 StageSceneStateOption::getSelectedFileId() const {
    return field_e8->getSelectedId();
}

bool StageSceneStateOption::isChangeLanguage() const {
    return !isDead() && al::isNerve(this, &NrvStageSceneStateOption.LanguageSettingConfirmYesNo);
}

void StageSceneStateOption::decide(const al::Nerve* nerve, SimpleLayoutMenu* layout,
                                   CommonVerticalList* list) {
    field_40 = "RightIn";
    field_48 = "RightOut";
    list->decide();
    changeNerve(nerve, layout, list);
}

void StageSceneStateOption::openConfirm(const al::Nerve* nerve, SimpleLayoutMenu* layout,
                                        CommonVerticalList* list) {
    field_40 = nullptr;
    field_48 = nullptr;
    list->decide();
    list->update();
    changeNerve(nerve, layout, list);
}

void StageSceneStateOption::cancel(const al::Nerve* nerve, SimpleLayoutMenu* layout,
                                   CommonVerticalList* list) {
    field_40 = "LeftIn";
    field_48 = "LeftOut";
    list->decide();
    changeNerve(nerve, layout, list);
}

void StageSceneStateOption::updateSaveDataInfo(bool) {}

void StageSceneStateOption::changeNerve(const al::Nerve* nerve, SimpleLayoutMenu* layout,
                                        CommonVerticalList* list) {
    mActiveNerve = nerve;
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

void StageSceneStateOption::exeOptionTop() {}

void StageSceneStateOption::exeModeSelectSelecting() {}

void StageSceneStateOption::exeModeSelectSelectingByHelp() {}

void StageSceneStateOption::exeModeSelectConfirmYesNo() {}

void StageSceneStateOption::exeModeSelectConfirmEnd() {}

void StageSceneStateOption::exeConfig() {}

void StageSceneStateOption::endConfig() {}

void StageSceneStateOption::exeDataManager() {}

void exeSaveDataSelecting() {}

void StageSceneStateOption::exeSaveDataConfirmYesNo() {
    if (al::isFirstStep(this))
        mWindowConfirmData->appearWithChoicingCancel();

    if (mWindowConfirmData->isDisable()) {
        al::setNerve(this, &NrvStageSceneStateOption.SaveDataSelecting);
        return;
    }

    field_e8->update();
    mWindowConfirmData->updateNerve();
    mWindowConfirmData->updateConfirmDataDate();

    if (mWindowConfirmData->isEndSelect()) {
        if (mWindowConfirmData->isDecided()) {
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
        if (mWindowConfirmData->isCanceled())
            mWindowConfirmData->end();
    }
}

void StageSceneStateOption::exeSaveDataSaving() {
    if (SaveDataAccessFunction::updateSaveDataAccess(mGameDataHolder, false))
        al::setNerve(this, &SaveDataSaved);
}

void StageSceneStateOption::exeSaveDataSaved() {}

void StageSceneStateOption::exeLoadDataSelecting() {}

void StageSceneStateOption::exeLoadDataConfirmNg() {
    if (al::isFirstStep(this))
        mWindowConfirm->appear();

    field_e8->update();
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
        mWindowConfirmData->appear();

    if (mWindowConfirmData->isDisable()) {
        al::setNerve(this, &NrvStageSceneStateOption.LoadDataSelecting);
        return;
    }

    field_e8->update();
    mWindowConfirmData->updateNerve();

    if (mWindowConfirmData->isEndSelect()) {
        if (mWindowConfirmData->isDecided()) {
            al::setNerve(this, &NrvStageSceneStateOption.LoadDataSaving);
            return;
        }
        if (mWindowConfirmData->isCanceled())
            mWindowConfirmData->end();
    }
}

void StageSceneStateOption::exeLoadDataSaving() {
    if (al::isFirstStep(this)) {
        mGameDataHolder->requestSetPlayingFileId(getSelectedFileId());
        mIsLoadData = true;
        mWindowConfirmData->end();
    }
}

void StageSceneStateOption::exeDeleteDataSelecting() {}

void StageSceneStateOption::exeDeleteDataConfirmNg() {
    if (al::isFirstStep(this))
        mWindowConfirm->appear();

    field_e8->update();
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
        mWindowConfirmData->appearWithChoicingCancel();

    if (mWindowConfirmData->isDisable()) {
        al::setNerve(this, &NrvStageSceneStateOption.DeleteDataSelecting);
        return;
    }

    field_e8->update();
    mWindowConfirmData->updateNerve();

    if (mWindowConfirmData->isEndSelect()) {
        if (mWindowConfirmData->isDecided()) {
            al::setNerve(this, &NrvStageSceneStateOption.DeleteDataDeleting);
            return;
        }
        if (mWindowConfirmData->isCanceled())
            mWindowConfirmData->end();
    }
}

void StageSceneStateOption::exeDeleteDataDeleting() {
    if (al::isFirstStep(this))
        SaveDataAccessFunction::startSaveDataDeleteWithWindow(mGameDataHolder, getSelectedFileId());

    if (SaveDataAccessFunction::updateSaveDataAccess(mGameDataHolder, false))
        al::setNerve(this, &DeleteDataDeleted);
}

void StageSceneStateOption::exeDeleteDataDeleted() {}

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
        if (!field_48) {
            al::setNerve(this, mActiveNerve);
            mActiveLayout = nullptr;
            mActiveList = nullptr;
            return;
        }
        mActiveLayout->startEnd(field_48);
    }

    if (mActiveLayout->isEndWait()) {
        al::setNerve(this, mActiveNerve);
        mActiveNerve = nullptr;
        mActiveList = nullptr;
        field_48 = nullptr;
    }
}

void StageSceneStateOption::exeWaitEndDecideAnimAndAutoSave() {
    if (al ::isFirstStep(this)) {
    }

    mActiveList->update();
    if (mActiveList->isDecideEnd() || mActiveList->isDeactive())
        mActiveLayout->startEnd(field_48);

    if (mActiveLayout->isEndWait())
        al::setNerve(this, &WaitEndAutoSave);
}

void StageSceneStateOption::exeWaitEndAutoSave() {}

void StageSceneStateOption::exeClose() {
    if (al ::isFirstStep(this)) {
        if (mIsOptionMode) {
            field_90->hideCursor();
            mOptionMode->startEnd("End");
            mMenuGuide->end();
        } else {
            field_70->hideCursor();
            mOptionSelect->startEnd("End");
        }
    }

    if (mIsOptionMode) {
        if (mOptionMode->isEndWait()) {
            mOptionMode->kill();
            mMenuGuide->end();
            mParFooter->kill();
            kill();
        }
        return;
    }

    if (mOptionSelect->isEndWait()) {
        mOptionSelect->kill();
        kill();
    }
}

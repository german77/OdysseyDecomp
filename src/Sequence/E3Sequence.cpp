#include "Sequence/E3Sequence.h"

#include <common/aglDrawContext.h>
#include <common/aglRenderBuffer.h>
#include <gfx/seadViewport.h>

#include "Library/Controller/InputFunction.h"
#include "Library/Layout/LayoutActorUtil.h"
#include "Library/Layout/LayoutUtil.h"
#include "Library/Memory/HeapUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Play/Layout/SimpleLayoutAppearWaitEnd.h"
#include "Library/Play/Layout/WipeHolder.h"
#include "Library/Scene/Scene.h"
#include "Library/Screen/ScreenFunction.h"
#include "Library/System/GameSystemInfo.h"

#include "Scene/StageScene.h"
#include "Sequence/GameSequenceInfo.h"
#include "System/GameDataFunction.h"
#include "System/GameDataHolder.h"
#include "System/SaveDataAccessFunction.h"
#include "Util/StageInputFunction.h"

namespace {
NERVE_IMPL(E3Sequence, LoadStationedResource)
NERVE_IMPL(E3Sequence, Title)
NERVE_IMPL(E3Sequence, StageSelect)
NERVE_IMPL(E3Sequence, EndCard)
NERVE_IMPL(E3Sequence, SoftReset)
NERVE_IMPL(E3Sequence, InitSystem)
NERVE_IMPL(E3Sequence, LoadWorldResource)
NERVE_IMPL(E3Sequence, LoadWorldResourceWithBoot)
NERVE_IMPL(E3Sequence, LoadStage)
NERVE_IMPL(E3Sequence, PlayStage)
NERVE_IMPL_(E3Sequence, DestroyToReboot, Destroy)
NERVE_IMPL_(E3Sequence, DestroyToReset, Destroy)
NERVE_IMPL_(E3Sequence, DestroyToMiss, Destroy)
NERVE_IMPL(E3Sequence, Destroy)
NERVE_IMPL_(E3Sequence, DestroyToReload, Destroy)
NERVE_IMPL(E3Sequence, DestroyToRebootEnd)
NERVE_IMPL(E3Sequence, Miss)
NERVE_IMPL(E3Sequence, MissCoinSub)
NERVE_IMPL(E3Sequence, MissEnd)

// NERVES_MAKE_NOSTRUCT(E3Sequence, LoadStationedResource, SoftReset, InitSystem, LoadWorldResource,
//                      LoadWorldResourceWithBoot)
NERVES_MAKE_STRUCT(E3Sequence, PlayStage, EndCard, StageSelect, Title, LoadStage, DestroyToReboot,
                   DestroyToReset, DestroyToMiss, Destroy, DestroyToReload, Miss,
                   DestroyToRebootEnd, MissCoinSub, MissEnd)
}  // namespace

E3Sequence::E3Sequence(const char* name) : al::Sequence(name) {}

bool E3Sequence::isDisposable() const {
    return true;
}

void E3Sequence::destroySceneHeap(bool removeCategory) {
    al::destroySceneHeap(removeCategory);
}

void E3Sequence::init(const al::SequenceInitInfo&) {}

void E3Sequence::initSystem() {}

void E3Sequence::deleteScene() {
    if (mCurrentScene) {
        mCurrentScene->~Scene();  // Calls second destructor?
        mCurrentScene = nullptr;
        if (GameDataFunction::isPlayDemoWorldWarp(mGameDataHolder) ||
            GameDataFunction::isPlayDemoWorldWarpHole(mGameDataHolder) ||
            al::isNerve(this, &NrvE3Sequence.DestroyToReboot)) {
            al::destroySceneHeap(true);
            return;
        }
        al::destroySceneHeap(false);
    }
}

void E3Sequence::update() {
    ControllerAppletFunction::tryReconnectGamePad(mGamePadSystem);
    al::Sequence::update();
    if (mGameDataHolder->isRequireSave() && !al::isNerve(this, &NrvE3Sequence.Destroy) &&
        !al::isNerve(this, &NrvE3Sequence.DestroyToMiss) &&
        !al::isNerve(this, &NrvE3Sequence.DestroyToReboot) && isEnableSave()) {
        SaveDataAccessFunction::startSaveDataWrite(mGameDataHolder);
    }
    SaveDataAccessFunction::updateSaveDataAccess(mGameDataHolder, false);
    al::executeUpdate(mLayoutKit);

    al::SceneCreator* sceneCreator = getSceneCreator();

    if (sceneCreator && mWipeHolder->isOpenEnd())
        rs::endWipeOpen(mGameDataHolder);
    else
        rs::startWipeOpen(mGameDataHolder);

    E3SequenceData* sequenceData = mSequenceData;
    s32 controllerPort = al::getMainControllerPort();

    if (al::isPadHoldZR(controllerPort) && al::isPadHoldPlus(controllerPort)) {
        if (!sequenceData->useSpecialControls)
            sequenceData->specialControlsDelay++;
        if (sequenceData->specialControlsDelay >= 181) {
            if (!sequenceData->useSpecialControls)
                sequenceData->useSpecialControls = true;
        }
    } else {
        sequenceData->specialControlsDelay = 0;
    }

    if (sequenceData->useSpecialControls) {
        if ((al::isPadTriggerZR(controllerPort) || al::isPadHoldZR(controllerPort)) &&
            al::isPadTriggerB(controllerPort)) {
            sequenceData->seconds = sequenceData->hours * 3600 + -300;
        }

        if ((al::isPadTriggerZR(controllerPort) || al::isPadHoldZR(controllerPort)) &&
            al::isPadTriggerA(controllerPort) && sequenceData->hours < 0xf) {
            sequenceData->hours++;
        }

        if ((al::isPadTriggerZR(controllerPort) || al::isPadHoldZR(controllerPort)) &&
            al::isPadTriggerY(controllerPort) && 0 < sequenceData->hours) {
            sequenceData->hours--;
        }

        if ((al::isPadTriggerZR(controllerPort) || al::isPadHoldZR(controllerPort)) &&
            al::isPadTriggerX(controllerPort)) {
            sequenceData->_1 = !sequenceData->_1;
        }

        if ((al::isPadTriggerR(controllerPort) || al::isPadHoldR(controllerPort)) &&
            al::isPadTriggerA(controllerPort)) {
            if (sequenceData->minutes == 5)
                sequenceData->minutes = 10;
            else if (299 >= sequenceData->minutes)
                sequenceData->minutes += 10;
        }

        if ((al::isPadTriggerR(controllerPort) || al::isPadHoldR(controllerPort)) &&
            al::isPadTriggerY(controllerPort)) {
            if (sequenceData->minutes >= 11)
                sequenceData->minutes -= 10;
            else if (sequenceData->minutes == 10)
                sequenceData->minutes = 5;
        }
        if ((al::isPadTriggerR(controllerPort) || al::isPadHoldR(controllerPort)) &&
            al::isPadTriggerX(controllerPort)) {
            sequenceData->_2 = !sequenceData->_2;
        }
        if ((al::isPadTriggerZR(controllerPort) || al::isPadHoldZR(controllerPort)) &&
            al::isPadTriggerPlus(controllerPort) && sequenceData->specialControlsDelay == 0) {
            sequenceData->useSpecialControls = false;
        }
    }

    if (al::isNerve(this, &NrvE3Sequence.PlayStage)) {
        sequenceData = this->mSequenceData;
        controllerPort = al::getMainControllerPort();
        if (sequenceData->_1 != false)
            sequenceData->seconds++;
        if (sequenceData->_2 != false) {
            if (!al::isPadHoldAny(controllerPort))
                sequenceData->playTime++;
            else
                sequenceData->playTime = 0;
        }

        sequenceData = this->mSequenceData;
        if (!al::isLessStep(this, 30)) {
            if (sequenceData->seconds != sequenceData->hours * 3600 &&
                sequenceData->seconds - sequenceData->hours * 3600 >= 0) {
                sequenceData = this->mSequenceData;
                sequenceData->seconds = 0;
                sequenceData->playTime = 0;
                al::setNerve(this, &NrvE3Sequence.EndCard);
                return;
            }
            sequenceData = this->mSequenceData;
            if (!al::isLessStep(this, 30) && sequenceData->playTime != sequenceData->minutes * 60 &&
                sequenceData->playTime - sequenceData->minutes * 60 >= 0) {
                sequenceData = this->mSequenceData;
                sequenceData->seconds = 0;
                sequenceData->playTime = 0;
                al::setNerve(this, &NrvE3Sequence.EndCard);
                return;
            }
        } else {
            sequenceData = this->mSequenceData;
            if (!al::isLessStep(this, 30) && sequenceData->playTime != sequenceData->minutes * 60 &&
                sequenceData->playTime - sequenceData->minutes * 60 >= 0) {
                sequenceData = this->mSequenceData;
                sequenceData->seconds = 0;
                sequenceData->playTime = 0;
                al::setNerve(this, &NrvE3Sequence.EndCard);
                return;
            }
        }
    }

    if (al::isNerve(this, &NrvE3Sequence.StageSelect)) {
        sequenceData = this->mSequenceData;
        s32 controllerPort = al::getMainControllerPort();
        if (sequenceData->_1 != false)
            sequenceData->seconds++;
        if (sequenceData->_2 != false) {
            if (!al::isPadHoldAny(controllerPort))
                sequenceData->playTime++;
            else
                sequenceData->playTime = 0;
        }
        sequenceData = this->mSequenceData;
        if (!al::isLessStep(this, 30) && sequenceData->playTime != sequenceData->minutes * 60 &&
            sequenceData->playTime - sequenceData->minutes * 60 >= 0) {
            mE3StageSelect->end();
            sequenceData = this->mSequenceData;
            sequenceData->seconds = 0;
            sequenceData->playTime = 0;
            al::setNerve(this, &NrvE3Sequence.Title);
        }
    }
}

bool E3Sequence::isEnableSave() const {
    if (!al::isNerve(this, &NrvE3Sequence.MissEnd)) {
        if (!al::isNerve(this, &NrvE3Sequence.Miss) &&
            !al::isNerve(this, &NrvE3Sequence.MissCoinSub)) {
            if (!al::isNerve(this, &NrvE3Sequence.PlayStage))
                return SaveDataAccessFunction::isEnableSave(mGameDataHolder);

            // Bad Object?
            if (((StageScene*)getSceneCreator())->isEnableSave())
                return SaveDataAccessFunction::isEnableSave(mGameDataHolder);
        }
        return false;
    }
    return !al::isActive(mCounterMiss);
}

void E3Sequence::drawMain() const {
    al::Sequence::drawMain();
    al::DrawSystemInfo* drawInfo = getDrawInfo();
    agl::DrawContext* drawContext = drawInfo->drawContext;
    agl::RenderBuffer* rendererBuffer =
        drawInfo->isDocked ? drawInfo->dockedRenderBuffer : drawInfo->handheldRenderBuffer;
    mScreenCaptureExecutor->tryCaptureAndDraw(drawContext, rendererBuffer, 0);

    sead::Viewport viewport(*rendererBuffer);
    viewport.apply(drawContext, *rendererBuffer);
    rendererBuffer->bind(drawContext);

    al::setRenderBuffer(mLayoutKit, rendererBuffer);
    al::executeDraw(mLayoutKit, "２Ｄベース（メイン画面）");
    al::executeDraw(mLayoutKit, "２Ｄオーバー（メイン画面）");
}

void E3Sequence::updateDestroy() {}

bool E3Sequence::isAbleReset() {
    if (al::isNerve(this, &NrvE3Sequence.PlayStage) ||
        al::isNerve(this, &NrvE3Sequence.StageSelect)) {
        return true;
    }
    return al::isNerve(this, &NrvE3Sequence.Title) && al::isGreaterStep(this, 60);
}

al::Scene* E3Sequence::getCurrentScene() const {
    return mCurrentScene;
}

void E3Sequence::exeLoadStationedResource() {}

void E3Sequence::exeTitle() {}

void E3Sequence::exeStageSelect() {}

void E3Sequence::exeEndCard() {}

void E3Sequence::exeSoftReset() {}

void E3Sequence::exeInitSystem() {}

void E3Sequence::exeLoadWorldResource() {}

void E3Sequence::exeLoadWorldResourceWithBoot() {}

void E3Sequence::exeLoadStage() {}

void E3Sequence::exePlayStage() {}

void E3Sequence::exeDestroy() {}

void E3Sequence::exeDestroyToRebootEnd() {}

void E3Sequence::exeMiss() {}

void E3Sequence::exeMissCoinSub() {}

void E3Sequence::exeMissEnd() {}

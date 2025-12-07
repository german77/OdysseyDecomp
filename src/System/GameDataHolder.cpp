#include "System/GameDataHolder.h"

#include <stream/seadRamStream.h>

#include "Library/Base/StringUtil.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/Resource/ResourceFunction.h"
#include "Library/SaveData/SaveDataFunction.h"
#include "Library/Yaml/ByamlIter.h"

#include "Item/Coin.h"
#include "Scene/QuestInfoHolder.h"
#include "Sequence/GameSequenceInfo.h"
#include "System/CapMessageBossData.h"
#include "System/GameConfigData.h"
#include "System/GameDataFile.h"
#include "System/GameDataFunction.h"
#include "System/GameProgressData.h"
#include "System/SaveDataAccessFunction.h"
#include "System/SaveDataAccessSequence.h"
#include "System/TempSaveData.h"
#include "System/UniqObjInfo.h"
#include "System/WorldList.h"
#include "Util/ScenePrepoFunction.h"
#include "Util/SpecialBuildUtil.h"

GameDataHolder::GameDataHolder(const al::MessageSystem* messageSystem) {}

GameDataHolder::GameDataHolder() {}

GameDataHolder::~GameDataHolder() = default;

const char* GameDataHolder::getSceneObjName() const {
    return "ゲームデータ保持";
}

al::MessageSystem* GameDataHolder::getMessageSystem() const {}

void GameDataHolder::setPlayingFileId(s32 fileId) {
    mPlayingFile = getGameDataFile(fileId);
    resetTempSaveData(false);
    if (fileId != -1)
        mPlayingFileId = fileId;
}

void GameDataHolder::initializeData() {
    for (s32 i = 0; i < 5; i++)
        getGameDataFile(i)->initializeData();

    initializeDataCommon();
    mPlayingFile = getGameDataFile(0);
    resetTempSaveData(false);
    mPlayingFileId = 0;
}

void GameDataHolder::initializeDataCommon() {
    _244 = false;
    mPlayTimeAcrossFiles = 0;
    mIsExistKoopaShip = false;
    resetTempSaveData(false);
    mIsValidCheckpointWarp = true;
    mStageMapPlayerPos = sead::Vector3f::zero;
    mIsPlayDemoLavaErupt = false;
    _220 = false;
    _224 = 0;
    mCapMessageBossData->init();

    for (s32 i = 0; i < mShowHackTutorialList.size(); i++)
        mShowHackTutorialList[i]->clear();

    for (s32 i = 0; i < 3; i++)
        mIsShowBindTutorial[i] = false;

    for (s32 i = 0; i < 10; i++)
        mCoinTransForDeadPlayer[i].set(sead::Vector3f::zero);

    mDeadPlayerCoinIdx = 0;
    mLocationName->clear();
    mQuestInfoHolder->clearAll();
    mSequenceInfo->init();
}

void GameDataHolder::resetTempSaveData(bool isSwapTempSaveData) {
    if (isSwapTempSaveData) {
        s32 worldIndex = mWorldList->tryFindWorldIndexByMainStageName(getNextStageName());
        TempSaveData* tempSaveData = mTempSaveData;
        s32 worldIndexBackup = mTempSaveDataBackup->getWorldIndex();

        mTempSaveData = mTempSaveDataBackup;
        mTempSaveDataBackup = tempSaveData;
        if (worldIndex != worldIndexBackup)
            mTempSaveData->init();
    } else {
        mTempSaveData->init();
        mTempSaveDataBackup->init();
    }

    for (s32 i = 0; i < 5; i++)
        getGameDataFile(i)->resetTempData();

    mCapMessageBossData->init();
    mLocationName->clear();
    resetScenarioStartCamera();

    mQuestInfoHolder->clearAll();
    mIsExistKoopaShip = false;
}

void GameDataHolder::initializeDataId(s32 fileId) {
    getGameDataFile(fileId)->initializeData();
}

void GameDataHolder::readByamlData(s32 fileId, const char* fileName) {
    resetTempSaveData(false);
    getGameDataFile(fileId)->initializeData();

    const u8* bymlData = al::getBymlFromArcName("DebugData/DebugSaveData", fileName);
    getGameDataFile(fileId)->tryReadByamlData(bymlData);

    SaveDataAccessFunction::startSaveDataWriteSync(this);
}

s32 GameDataHolder::tryFindEmptyFileId() const {
    for (s32 i = 0; i < 5; i++)
        if (getGameDataFile(i)->isEmpty())
            return i;
    return -1;
}

void GameDataHolder::createSaveDataAccessSequence(const al::LayoutInitInfo& layoutInitInfo) {
    mSaveDataAccessSequence = new SaveDataAccessSequence(this, layoutInitInfo);
}

void GameDataHolder::createSaveDataAccessSequenceDevelop(const al::LayoutInitInfo& layoutInitInfo) {
    createSaveDataAccessSequence(layoutInitInfo);
    mSaveDataAccessSequence->setDevelop();
}

bool GameDataHolder::isRequireSave() const {
    return mIsRequireSave && mRequireSaveFrame == 0;
}

void GameDataHolder::setRequireSave() {
    mIsRequireSave = true;
    mRequireSaveFrame = 0;
}

void GameDataHolder::setRequireSaveFalse() {
    mIsRequireSave = false;
    mRequireSaveFrame = 0;
}

void GameDataHolder::setRequireSaveFrame() {
    if (!isRequireSave()) {
        mRequireSaveFrame = 60;
        mIsRequireSave = true;
    }
}

void GameDataHolder::updateRequireSaveFrame() {
    mRequireSaveFrame = sead::Mathi::clampMin(mRequireSaveFrame - 1, 0);
}

bool GameDataHolder::isInvalidSaveForMoonGet() const {
    return mIsInvalidSaveForMoonGet;
}

void GameDataHolder::invalidateSaveForMoonGet() {
    mIsInvalidSaveForMoonGet = true;
}

void GameDataHolder::validateSaveForMoonGet() {
    mIsInvalidSaveForMoonGet = false;
}

void GameDataHolder::setLanguage(const char* language) {
    mLanguage.format("%s", language);
}

const char* GameDataHolder::getLanguage() const {
    return mLanguage.cstr();
}

void GameDataHolder::changeNextStage(const ChangeStageInfo* changeStageInfo, s32 raceType) {
    if (_49)
        return;

    mPlayingFile->changeNextStage(changeStageInfo, raceType);
    _49 = true;
    resetLocationName();
}

void GameDataHolder::resetLocationName() {
    mLocationName->clear();
}

void GameDataHolder::changeNextStageWithDemoWorldWarp(const char* stageName) {
    if (_49)
        return;

    resetTempSaveData(false);
    mPlayingFile->changeNextStageWithDemoWorldWarp(stageName);
    _49 = false;
}

bool GameDataHolder::tryChangeNextStageWithWorldWarpHole(const char* stageName) {
    if (_49)
        return false;

    mPlayingFile->changeNextStageWithWorldWarpHole(stageName);
    _49 = true;
    resetTempSaveData(true);
    return true;
}

void GameDataHolder::returnPrevStage() {
    if (_49)
        return;

    mPlayingFile->returnPrevStage();
    _49 = true;
    resetLocationName();
}

const char* GameDataHolder::getNextStageName() const {
    return mPlayingFile->getStageNameNext();
}

const char* GameDataHolder::getNextStageName(s32 fileId) const {
    return getGameDataFile(fileId)->getStageNameNext();
}

GameDataFile* GameDataHolder::getGameDataFile(s32 fileId) const {
    return mFiles[fileId];
}

const char* GameDataHolder::getNextPlayerStartId() const {
    return mPlayingFile->getPlayerStartId();
}

const char* GameDataHolder::getCurrentStageName() const {
    return mPlayingFile->getStageNameCurrent();
}

const char* GameDataHolder::tryGetCurrentStageName() const {
    return mPlayingFile->tryGetStageNameCurrent();
}

const char* GameDataHolder::getCurrentStageName(s32 fileId) const {
    return getGameDataFile(fileId)->getStageNameCurrent();
}

void GameDataHolder::setCheckpointId(const al::PlacementId* placementId) {
    mPlayingFile->setCheckpointId(placementId);
    setRequireSaveFrame();
}

const char* GameDataHolder::tryGetRestartPointIdString() const {
    mPlayingFile->tryGetRestartPointIdString();
}

void GameDataHolder::endStage() {
    if (_4a)
        return;
    mPlayingFile->endStage();
}

void GameDataHolder::startStage(const char* stageName, s32 scenarioNo) {
    mPlayingFile->startStage(stageName, scenarioNo);
    mTempSaveData->setInfo(mPlayingFile->getCurrentWorldId(), scenarioNo);
}

void GameDataHolder::onObjNoWriteSaveData(const al::PlacementId* placementId) {
    mTempSaveData->writeInWorld(placementId, getCurrentStageName());
}

void GameDataHolder::offObjNoWriteSaveData(const al::PlacementId* placementId) {
    mTempSaveData->deleteInWorld(placementId, getCurrentStageName());
}

bool GameDataHolder::isOnObjNoWriteSaveData(const al::PlacementId* placementId) const {
    return mTempSaveData->isOnInWorld(placementId, getCurrentStageName());
}

void GameDataHolder::onObjNoWriteSaveDataResetMiniGame(const al::PlacementId* placementId) {
    mTempSaveData->writeInWorldResetMiniGame(placementId, getCurrentStageName());
}

void GameDataHolder::offObjNoWriteSaveDataResetMiniGame(const al::PlacementId* placementId) {
    mTempSaveData->deleteInWorldResetMiniGame(placementId, getCurrentStageName());
}

bool GameDataHolder::isOnObjNoWriteSaveDataResetMiniGame(const al::PlacementId* placementId) const {
    return mTempSaveData->isOnInWorldResetMiniGame(placementId, getCurrentStageName());
}

void GameDataHolder::onObjNoWriteSaveDataInSameScenario(const al::PlacementId* placementId) {
    mTempSaveData->writeInScenario(placementId, getCurrentStageName());
}

bool GameDataHolder::isOnObjNoWriteSaveDataInSameScenario(
    const al::PlacementId* placementId) const {
    return mTempSaveData->isOnInScenario(placementId, getCurrentStageName());
}

void GameDataHolder::writeTempSaveDataToHash(const char* hashName, bool value) {
    mTempSaveData->writeHashInWorld(hashName, value);
}

bool GameDataHolder::findValueFromTempSaveDataHash(const char* hashName) {
    return mTempSaveData->findHashValueInWorld(hashName);
}

void GameDataHolder::resetMiniGameData() {
    mTempSaveData->resetMiniGame();
}

s32 GameDataHolder::getPlayingFileId() const {
    for (s32 i = 0; i < 5; i++)
        if (mPlayingFile == getGameDataFile(i))
            return i;
    return -1;
}

s32 GameDataHolder::getPlayingOrNextFileId() const {
    GameDataFile* gameDataFile = mNextFile ?: mPlayingFile;

    for (s32 i = 0; i < 5; i++)
        if (gameDataFile == getGameDataFile(i))
            return i;
    return -1;
}

void GameDataHolder::requestSetPlayingFileId(s32 fileId) {
    mNextFile = getGameDataFile(fileId);
}

void GameDataHolder::receiveSetPlayingFileIdMsg() {
    mPlayingFileId = getPlayingFileId();
    resetTempSaveData(false);
}

GameDataFile* GameDataHolder::findGameDataFile(const char* fileName) const {
    for (s32 i = 0; i < 5; i++) {
        al::StringTmp<32> gameDataFilename{"%s%d.bin", "File", i};
        if (al::isEqualString(fileName, gameDataFilename))
            return getGameDataFile(i);
    }
    return nullptr;
}

GameDataFile* GameDataHolder::findFileByName(const char* fileName) const {
    return findGameDataFile(fileName);
}

void GameDataHolder::resetScenarioStartCamera() {
    for (s32 i = 0; i < 16; i++)
        mIsPlayAlreadyScenarioStartCamera[i] = false;
}

void GameDataHolder::resetTempSaveDataInSameScenario() {
    mTempSaveData->initForScenario();
    mIsExistKoopaShip = false;
}

void GameDataHolder::readFromSaveDataBuffer(const char* fileName) {}

bool GameDataHolder::tryReadByamlDataCommon(const u8* byamlData) {
    if (alByamlLocalUtil::verifiByaml(byamlData)) {
        al::ByamlIter save{byamlData};
        mGameConfigData->read(save);
        return true;
    }
    return false;
}

void GameDataHolder::readFromSaveDataBufferCommonFileOnlyLanguage() {
    sead::RamReadStream readStream(al::getSaveDataWorkBuffer(), 0x400, sead::Stream::Modes::Binary);

    struct SaveDataBuffer {
        s32 a;
        s32 b;
        s32 c;
        char language[0x2c];
    };

    static_assert(sizeof(SaveDataBuffer) == 0x38);

    SaveDataBuffer buffer;
    memset(&buffer, 0, 0x38);
    readStream.readMemBlock((void*)&buffer, sizeof(SaveDataBuffer));

    if (buffer.a == 0)
        mLanguage.format("%s", buffer.language);
}

void GameDataHolder::writeToSaveBuffer(const char* fileName) {}

void GameDataHolder::updateSaveInfoForDisp(const char* fileName) {}

void GameDataHolder::updateSaveTimeForDisp(const char* fileName) {}

s32 GameDataHolder::findUnlockShineNum(bool* isCountTotal, s32 worldId) const {}

s32 GameDataHolder::calcBeforePhaseWorldNumMax(s32 worldId) const {
    s32 worldNumMax = -1;
    for (s32 i = 0; i < mStageLockList.size(); i++) {
        if (mStageLockList[i]->shineNumInfoNum + worldNumMax >= worldId)
            return worldNumMax;

        worldNumMax += mStageLockList[i]->shineNumInfoNum;
    }
    return -1;
}

bool GameDataHolder::isFindKoopaNext(s32 worldId) const {
    if ((mPlayingFile->getGameProgressData()->getUnlockWorldNum() ==
             GameDataFunction::getWorldIndexCloud() ||
         mPlayingFile->getGameProgressData()->getUnlockWorldNum() ==
             GameDataFunction::getWorldIndexCloud() + 1) &&
        GameDataFunction::getWorldIndexCity() == worldId) {
        return true;
    }
    return false;
}

bool GameDataHolder::isBossAttackedHomeNext(s32 worldId) const {
    if ((mPlayingFile->getGameProgressData()->getUnlockWorldNum() ==
             GameDataFunction::getWorldIndexBoss() ||
         mPlayingFile->getGameProgressData()->getUnlockWorldNum() ==
             GameDataFunction::getWorldIndexBoss() + 1) &&
        GameDataFunction::getWorldIndexSky() == worldId) {
        return true;
    }
    return false;
}

void GameDataHolder::playScenarioStartCamera(s32 questNo) {
    mIsPlayAlreadyScenarioStartCamera[questNo] = true;
}

bool GameDataHolder::isPlayAlreadyScenarioStartCamera(s32 questNo) const {
    return mIsPlayAlreadyScenarioStartCamera[questNo];
}

const sead::PtrArray<ShopItem::ItemInfo>& GameDataHolder::getShopItemInfoList() const {
    if (rs::isModeE3Rom() || rs::isModeE3LiveRom())
        return mShopItemListE3;
    return mShopItemList;
}

bool GameDataHolder::checkNeedTreasureMessageStage(const char* stageName) const {
    return mWorldList->checkNeedTreasureMessageStage(stageName);
}

bool GameDataHolder::tryFindLinkDestStageInfo(const char** destStageName, const char** destLabel,
                                              const char* srcStageName,
                                              const char* srcLabel) const {}

bool GameDataHolder::isShowHackTutorial(const char* hackName, const char* suffix) const {}

void GameDataHolder::setShowHackTutorial(const char* hackName, const char* suffix) {}

bool GameDataHolder::isShowBindTutorial(const char* bindName) const {
    s32 index;

    if (al::isEqualString("SphinxRide", bindName))
        index = 0;
    else if (al::isEqualString("Motorcycle", bindName))
        index = 1;
    else if (al::isEqualString("WorldWarpHole", bindName))
        index = 2;
    else
        return false;

    return mIsShowBindTutorial[index];
}

const char* GameDataHolder::getCoinCollectArchiveName(s32 worldId) const {
    if (worldId > -1)
        return mWorldItemTypeInfo[worldId]->coinCollect.cstr();
    return "CoinCollect";
}

const char* GameDataHolder::getCoinCollectEmptyArchiveName(s32 worldId) const {
    if (worldId > -1)
        return mWorldItemTypeInfo[worldId]->coinCollectEmpty.cstr();
    return "CoinCollectEmptyA";
}

const char* GameDataHolder::getCoinCollect2DArchiveName(s32 worldId) const {
    if (worldId > -1)
        return mWorldItemTypeInfo[worldId]->coinCollect2D.cstr();
    return "CoinCollect2D";
}

const char* GameDataHolder::getCoinCollect2DEmptyArchiveName(s32 worldId) const {
    if (worldId > -1)
        return mWorldItemTypeInfo[worldId]->coinCollectEmpty2D.cstr();
    return "CoinCollectEmpty2D_A";
}

s32 GameDataHolder::getShineAnimFrame(s32 worldId) const {
    if (worldId < 0)
        return 0;

    return mWorldItemTypeInfo[worldId]->shineAnimFrame;
}

s32 GameDataHolder::getCoinCollectNumMax(s32 worldId) const {
    return mCoinCollectNumMax[worldId];
}

bool GameDataHolder::isInvalidOpenMapStage(const char* stageName, s32 scenarioNo) const {}

void GameDataHolder::setShowBindTutorial(const char* bindName) {
    if (al::isEqualString("SphinxRide", bindName)) {
        mIsShowBindTutorial[0] = true;
        return;
    }
    if (al::isEqualString("Motorcycle", bindName)) {
        mIsShowBindTutorial[1] = true;
        return;
    }
    if (al::isEqualString("WorldWarpHole", bindName)) {
        mIsShowBindTutorial[2] = true;
        return;
    }
}

s32 GameDataHolder::tryCalcWorldWarpHoleSrcId(s32 destId) const {
    s32 holeSrcId = -1;
    for (s32 i = 0; i < mWorldList->getWorldNum(); i++) {
        if (mWorldWarpHoleDestIds[i] != destId)
            continue;
        if (holeSrcId != -1)
            return -1;
        holeSrcId = i;
    }

    return holeSrcId;
}

s32 GameDataHolder::calcWorldWarpHoleDestId(s32 srcId) const {
    return mWorldWarpHoleDestIds[srcId];
}

s32 GameDataHolder::calcWorldWarpHoleIdFromWorldId(s32 worldId) const {
    if (GameDataFunction::getWorldIndexPeach() == worldId ||
        GameDataFunction::getWorldIndexLava() == worldId ||
        GameDataFunction::getWorldIndexCity() == worldId) {
        return worldId;
    }

    for (s32 i = 0; i < mWorldList->getWorldNum(); i++)
        if (mPlayingFile->getGameProgressData()->getWorldIdForWorldWarpHole(i) == worldId)
            return i;

    return -1;
}

s32 GameDataHolder::calcWorldIdFromWorldWarpHoleId(s32 worldWarpHoleId) const {
    return mPlayingFile->getGameProgressData()->getWorldIdForWorldWarpHole(worldWarpHoleId);
}

void GameDataHolder::calcWorldWarpHoleLabelAndStageName(sead::BufferedSafeString* label,
                                                        sead::BufferedSafeString* stageName,
                                                        const char* srcLabel, s32 worldId) const {}

const GameDataHolder::WorldWarpHoleInfo*
GameDataHolder::findWorldWarpHoleInfo(s32 worldId, s32 scenarioNo, const char* name) const {
    for (s32 i = 0; i < mWorldWarpHoleInfoNum; i++) {
        if (mWorldWarpHoleInfos[i].worldId != worldId)
            continue;
        sead::FixedSafeString<128> nmp;
        nmp.format("%s%s%d", name, "From", scenarioNo);

        if (al::isEqualString(name, mWorldWarpHoleInfos[i].name) ||
            al::isEqualString(nmp, mWorldWarpHoleInfos[i].name)) {
            return &mWorldWarpHoleInfos[i];
        }
    }

    return nullptr;
}

bool GameDataHolder::checkIsOpenWorldWarpHoleInScenario(s32 worldId, s32 scenarioNo) const {
    for (s32 i = 0; i < mWorldWarpHoleInfoNum; i++) {
        if (mWorldWarpHoleInfos[i].worldId != worldId)
            continue;

        if (al::isEqualString(mWorldWarpHoleInfos[i].name, "Go"))
            return mWorldWarpHoleInfos[i].scenarioNo <= scenarioNo;
    }

    return false;
}

void GameDataHolder::setLocationName(const al::PlacementInfo* placementInfo) {
    mLocationName->set(getCurrentStageName(), placementInfo);
}

bool GameDataHolder::isPrevLocation(const al::PlacementInfo* placementInfo) const {
    UniqObjInfo objInfo;

    const char* stageName =
        GameDataFunction::getCurrentStageName(const_cast<GameDataHolder*>(this));
    objInfo.set(stageName, placementInfo);

    return mLocationName->isEqual(objInfo);
}

void GameDataHolder::setCoinTransForDeadPlayer(const sead::PtrArray<Coin>& coins, s32 coinNum) {
    mDeadPlayerCoinIdx = 0;
    for (s32 i = 0; i < coinNum; i++) {
        if (coins[i] == nullptr || al::isDead(coins[i]))
            continue;

        mCoinTransForDeadPlayer[mDeadPlayerCoinIdx].set(al::getTrans(coins[i]));
        mDeadPlayerCoinIdx++;
    }
}

const sead::Vector3f& GameDataHolder::getCoinTransForDeadPlayer(s32 coinIdx) {
    return mCoinTransForDeadPlayer[coinIdx];
}

void GameDataHolder::setSeparatePlay(bool isSeparatePlay) {
    mIsSeparatePlay = isSeparatePlay;
    rs::trySavePrepoSeparatePlayMode(isSeparatePlay, mPlayingFile->getPlayTimeTotal(),
                                     mPlayingFile->getSaveDataIdForPrepo(), mPlayTimeAcrossFiles);
}

CapMessageBossData* GameDataHolder::getCapMessageBossData() const {
    return mCapMessageBossData;
}

s32 GameDataHolder::findUseScenarioNo(const char* stageName) const {
    return mWorldList->findUseScenarioNo(stageName);
}

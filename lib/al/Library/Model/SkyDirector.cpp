#include "Library/Model/SkyDirector.h"

#include "Library/Base/StringUtil.h"
#include "Library/Yaml/ParameterBase.h"

namespace al {


SkyParam::SkyParam() {
    IUseRequestParam::init();
    mSkyName = new ParameterString64(al::StringTmp<64>("RSGraphicTestSkyBlue"),mParameterObj,
                                     "Name", "空のモデル名", "", true);

    mRotate = new ParameterV3f({0.0f,0.0f,0.0f}, mParameterObj, "Rotate", "回転", "Min=-360.0f, Max=360.0f", true);
    mStarIntensity = new ParameterF32(0.0f, mParameterObj, "StarIntensity", "星の明るさ", "Min=0.0f, Max=100.0f", true);
}

bool SkyParam::isEqual(const al::IUseRequestParam& other) const {}

const sead::Vector3f& SkyParam::getRotate() const {
    return mRotate->getValue();
}

const char* SkyParam::getSkyName() const {
    return mSkyName->getValue().cstr();
}

f32 SkyParam::getStarIntensity() const {
    return mStarIntensity->getValue();
}

}  // namespace al

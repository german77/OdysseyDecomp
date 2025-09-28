#pragma once

#include <basis/seadTypes.h>

namespace nn::atk {
    struct AuxBus;
}

namespace al {
    struct BgmRegionChangeParams;

class IBgmParamsChanger {
public:
    virtual void resetForReuse();
    virtual bool isActivate() const;
    virtual bool isPaused() const;
    virtual void changeVolume(f32, s32);
    virtual void changePitchShift(f32, s32);
    virtual void changePitchShiftModulation(f32, f32, s32);
    virtual void changeLpf(f32, s32);
    virtual void changeBiquadFilter(f32, s32);
    virtual void changeEffectSend(nn::atk::AuxBus, f32, s32);
    virtual void changeTrackVolume(s32, f32, s32);
    virtual void changeDefaultParams(s32);
    virtual void forceDeactivate();
    virtual bool isEnableRegionChange() const;
    virtual void setRegionChangeParams(const al::BgmRegionChangeParams&);
    virtual const char* getSituationName() const;
    virtual const char* getSubSituationName() const;
    virtual void setSituationName(const char*);
    virtual void setSubSituationName(const char*);
    virtual f32 getCurVolume() const;
    virtual f32 getCurPitchShift() const;
    virtual f32 getCurPitchShiftModulation() const;
    virtual f32 getCurLpfCutOff() const;
    virtual f32 getBiquadFilterValue() const;
    virtual nn::atk::AuxBus getEffectSend() const;
    virtual s32 getEffectBusId() const;
    virtual f32 getCurTrackVolume(s32) const;
    virtual const al::BgmRegionChangeParams& getRegionChangeParams() const;
};

}  // namespace al

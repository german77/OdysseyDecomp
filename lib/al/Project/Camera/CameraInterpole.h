#pragma once

#include "Library/Nerve/NerveExecutor.h"
#include "gfx/seadCamera.h"

namespace al {
class CameraPoser;
class CameraTicket;

class CameraInterpole : public NerveExecutor {
public:
    CameraInterpole();

    void start(const CameraTicket* ticket, f32 prevFovyDegree, s32 endInterpoleStep);
    void update(const sead::LookAtCamera& camera);
    bool isActive() const;
    void makeLookAtCamera(sead::LookAtCamera* camera) const;
    void lerpFovyDegree(f32);
    void exeDeactive();
    void exeActiveHermite();
    void exeActiveHermiteDistanceHV();
    void exeActiveRotateAxisY();
    void exeActiveBrigade();

private:
    void updatePoser(sead::LookAtCamera* cam);
    f32 getStep(CameraPoser* poser, s32 offset, s32 end);
    f32 getStepIn(s32 offset, s32 end);

    const al::CameraTicket* mNextTicket = nullptr;
    const al::CameraTicket* mPrevTicket = nullptr;
    al::CameraPoser* mPrevPoser = nullptr;
    s32 mEndInterpoleStep = -1;
    bool mIsRequestCancel = false;
    sead::Vector3f _30_pos = {0.0f, 0.0f, 0.0f};
    sead::Vector3f _3c_at = {0.0f, 0.0f, 0.0f};
    sead::Vector3f _49_up = sead::Vector3f::ey;
    sead::LookAtCamera _48CamA;
    sead::LookAtCamera _B8CamB;
    sead::LookAtCamera _118CamC;
    sead::LookAtCamera _178CamD;
    f32 _1d8 = 30.0f;
    f32 mPrevFovyDegree = 30.0f;
    f32 mAngleRotateY = 0.0f;
    sead::Vector3f _1e4 = {0.0f, 0.0f, 0.0f};
    f32 _1f0 = 0.0f;
    f32 _1f4 = 0.0f;
};

}  // namespace al

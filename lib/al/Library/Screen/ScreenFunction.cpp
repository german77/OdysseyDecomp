#include "Library/Screen/ScreenFunction.h"

#include <gfx/seadProjection.h>

#include "Library/Camera/CameraUtil.h"
#include "Library/Math/MathUtil.h"

namespace al {

u32 getDisplayWidth() {
    return 1280;
}

u32 getDisplayHeight() {
    return 720;
}

u32 getSubDisplayWidth() {
    return 0;
}

u32 getSubDisplayHeight() {
    return 0;
}

u32 getLayoutDisplayWidth() {
    return 1280;
}

u32 getLayoutDisplayHeight() {
    return 720;
}

u32 getVirtualDisplayWidth() {
    return 1280;
}

u32 getVirtualDisplayHeight() {
    return 720;
}

bool isInScreen(const sead::Vector2f& screenPos, f32 tolerance) {
    if (screenPos.x < -tolerance || screenPos.y < -tolerance)
        return false;

    if (screenPos.x < getDisplayWidth() + tolerance && screenPos.y < getDisplayHeight() + tolerance)
        return true;

    return false;
}

void calcScreenPosFromLayoutPos(sead::Vector2f* outScreenPos, const sead::Vector2f& layoutPos) {
    outScreenPos->x = layoutPos.x + getLayoutDisplayWidth() / 2.0f;
    outScreenPos->y = -layoutPos.y + getLayoutDisplayHeight() / 2.0f;
}

inline void calcScreenPosFromLayoutPosSub(sead::Vector2f* outScreenPos,
                                          const sead::Vector2f& layoutPosSub) {
    outScreenPos->x = layoutPosSub.x + getSubDisplayWidth() / 2.0f;
    outScreenPos->y = -layoutPosSub.y + getSubDisplayHeight() / 2.0f;
}

void calcLayoutPosFromScreenPos(sead::Vector2f* outLayoutPos, const sead::Vector2f& screenPos) {
    outLayoutPos->x = screenPos.x - getLayoutDisplayWidth() / 2.0f;
    outLayoutPos->y = -screenPos.y + getLayoutDisplayHeight() / 2.0f;
}

bool calcWorldPosFromScreen(sead::Vector3f* outWorldPos, const sead::Vector2f& screenPos,
                            const sead::Matrix34f& viewMtx, f32 scale) {
    sead::Vector2f pos = screenPos;
    f32 screenScale = 360.0f * sead::Mathf::tan(sead::Mathf::pi() / 8.0f);
    scale = scale >= 0.0f ? scale : screenScale;

    if (!outWorldPos)
        return true;

    f32 rate = scale / screenScale;
    f32 negScale = -scale;
    sead::Vector2f pos3{rate * (pos.x - getDisplayWidth() / 2.0f),
                        -(rate * (pos.y - getDisplayHeight() / 2.0f))};

    outWorldPos->x = viewMtx.m[0][0] * pos3.x + viewMtx.m[0][1] * pos3.y +
                     viewMtx.m[0][2] * negScale +
                     (-(viewMtx.m[0][0] * viewMtx.m[3][0]) - (viewMtx.m[0][1] * viewMtx.m[3][1]) -
                      (viewMtx.m[0][2] * viewMtx.m[3][2]));

    outWorldPos->y = viewMtx.m[1][0] * pos3.x + viewMtx.m[1][1] * pos3.y +
                     viewMtx.m[1][2] * negScale +
                     (-(viewMtx.m[1][0] * viewMtx.m[3][0]) - (viewMtx.m[1][1] * viewMtx.m[3][1]) -
                      (viewMtx.m[1][2] * viewMtx.m[3][2]));

    outWorldPos->z = viewMtx.m[2][0] * pos3.x + viewMtx.m[2][1] * pos3.y +
                     viewMtx.m[2][2] * negScale +
                     (-(viewMtx.m[2][0] * viewMtx.m[3][0]) - (viewMtx.m[2][1] * viewMtx.m[3][1]) -
                      (viewMtx.m[2][2] * viewMtx.m[3][2]));
    return true;
}

void calcWorldPosFromScreenPos(sead::Vector3f* outWorldPos, const IUseCamera* camera,
                               const sead::Vector2f& screenPos, f32 scale) {
    sead::Vector2f layoutPos = {screenPos.x - getDisplayWidth() / 2.0f,
                                -(screenPos.y - getDisplayHeight() / 2.0f)};
    calcWorldPosFromLayoutPos(outWorldPos, camera, layoutPos, scale);
}

void calcWorldPosFromLayoutPos(sead::Vector3f* outWorldPos, const IUseCamera* camera,
                               const sead::Vector2f& layoutPos, f32 scale) {
    sead::Vector3f cameraPos;
    sead::Viewport viewPort(0.0f, 0.0f, getProjectionSead(camera, 0).getAspect() * 720.0f, 720.0f);

    const sead::LookAtCamera& lookAt = getLookAtCamera(camera, 0);
    const sead::Projection& projection = getProjectionSead(camera, 0);

    f32 x = viewPort.getHalfSizeX();
    f32 y = viewPort.getHalfSizeY();
    projection.screenPosToCameraPos(&cameraPos, {layoutPos.x / x, layoutPos.y / y});

    if (scale > 0.0f) {
        f32 size = -scale / cameraPos.z;
        cameraPos *= size;
    }

    lookAt.cameraPosToWorldPosByMatrix(outWorldPos, cameraPos);
}

void calcWorldPosFromScreenPos(sead::Vector3f* outWorldPos, const IUseCamera* camera,
                               const sead::Vector2f& screenPos, const sead::Vector3f& scale) {
    sead::Vector2f layoutPos = {screenPos.x - getDisplayWidth() / 2.0f,
                                -(screenPos.y - getDisplayHeight() / 2.0f)};
    calcWorldPosFromLayoutPos(outWorldPos, camera, layoutPos, scale);
}

void calcWorldPosFromLayoutPos(sead::Vector3f* output, const IUseCamera*, const sead::Vector2f&,
                               const sead::Vector3f&);

void calcWorldPosFromScreenPosSub(sead::Vector3f* outWorldPos, const IUseCamera* camera,
                                  const sead::Vector2f& screenPosSub, f32 scale) {
    sead::Vector2f layoutPosSub = {screenPosSub.x, -screenPosSub.y};
    calcWorldPosFromLayoutPosSub(outWorldPos, camera, layoutPosSub, scale);
}

void calcWorldPosFromLayoutPosSub(sead::Vector3f* output, const IUseCamera*, const sead::Vector2f&,
                                  f32);

void calcWorldPosFromScreenPosSub(sead::Vector3f* outWorldPos, const IUseCamera* camera,
                                  const sead::Vector2f& screenPosSub, const sead::Vector3f& scale) {
    sead::Vector2f layoutPosSub = {screenPosSub.x, -screenPosSub.y};
    calcWorldPosFromLayoutPosSub(outWorldPos, camera, layoutPosSub, scale);
}

void calcWorldPosFromLayoutPosSub(sead::Vector3f* output, const IUseCamera*, const sead::Vector2f&,
                                  const sead::Vector3f&);
}  // namespace al

namespace ScreenFunction {
void calcWorldPositionFromCenterScreen(sead::Vector3f*, const sead::Vector2f&,
                                       const sead::Vector3f&, const sead::Camera&,
                                       const sead::Projection&, const sead::Viewport&);
}  // namespace ScreenFunction

// Note: New file?

namespace al {
void calcScreenPosFromWorldPos(sead::Vector2f* outScreenPos, const IUseCamera* camera,
                               const sead::Vector3f& worldPos) {
    calcLayoutPosFromWorldPos(outScreenPos, camera, worldPos);
    calcScreenPosFromLayoutPos(outScreenPos, *outScreenPos);
}

void calcLayoutPosFromWorldPos(sead::Vector2f* outLayoutPos, const IUseCamera* camera,
                               const sead::Vector3f& worldPos) {
    sead::Viewport viewPort(0.0f, 0.0f, getProjectionSead(camera, 0).getAspect() * 720.0f, 720.0f);

    getLookAtCamera(camera, 0).projectByMatrix(outLayoutPos, worldPos, getProjectionSead(camera, 0),
                                               viewPort);
}

void calcScreenPosFromWorldPosSub(sead::Vector2f* outScreenPos, const IUseCamera* camera,
                                  const sead::Vector3f& worldPosSub) {
    calcLayoutPosFromWorldPosSub(outScreenPos, camera, worldPosSub);
    calcScreenPosFromLayoutPosSub(outScreenPos, *outScreenPos);
}

void calcLayoutPosFromWorldPosSub(sead::Vector2f* outLayoutPos, const IUseCamera* camera,
                                  const sead::Vector3f& worldPosSub) {
    sead::Viewport viewPort(0.0f, 0.0f, 0.0f, 0.0f);

    getLookAtCamera(camera, getViewNumMax(camera) - 1)
        .projectByMatrix(outLayoutPos, worldPosSub,
                         getProjectionSead(camera, getViewNumMax(camera) - 1), viewPort);
}

void calcLayoutPosFromWorldPos(sead::Vector3f* outLayoutPos, const IUseCamera* camera,
                               const sead::Vector3f& worldPos) {
    sead::Viewport viewPort(0.0f, 0.0f, getProjectionSead(camera, 0).getAspect() * 720.0f, 720.0f);

    sead::Vector3f cameraPos;
    getLookAtCamera(camera, 0).worldPosToCameraPosByMatrix(&cameraPos, worldPos);

    sead::Vector2f layoutPos;
    getProjectionSead(camera, 0).project(&layoutPos, cameraPos, viewPort);

    outLayoutPos->set(layoutPos.x, layoutPos.y, cameraPos.z);
}

void calcLayoutPosFromWorldPosWithClampOutRange(sead::Vector3f* outLayoutPos,
                                                const IUseCamera* camera,
                                                const sead::Vector3f& worldPos, f32 range,
                                                s32 viewIdx) {
    calcLayoutPosFromWorldPosWithClampOutRange(outLayoutPos, getSceneCameraInfo(camera), worldPos,
                                               range, viewIdx);
}

void calcLayoutPosFromWorldPosWithClampOutRange(sead::Vector3f* outLayoutPos,
                                                const SceneCameraInfo* camera,
                                                const sead::Vector3f& worldPos, f32 range,
                                                s32 viewIdx) {
    sead::Viewport viewPort(0.0f, 0.0f, 1280.0f, 720.0f);

    sead::Vector3f cameraPos;
    getLookAtCamera(camera, viewIdx).worldPosToCameraPosByMatrix(&cameraPos, worldPos);

    if (-range < cameraPos.z && range > cameraPos.z) {
        if (cameraPos.z > 0.0f)
            cameraPos.z = range;
        else
            cameraPos.z = -range;
    }

    sead::Vector2f layoutPos;
    getProjectionSead(camera, viewIdx).project(&layoutPos, cameraPos, viewPort);

    outLayoutPos->set(layoutPos.x, layoutPos.y, cameraPos.z);
}

void calcLayoutPosFromWorldPosWithClampByScreen(sead::Vector3f* outLayoutPos,
                                                const IUseCamera* camera,
                                                const sead::Vector3f& worldPos) {
    calcLayoutPosFromWorldPos(outLayoutPos, getSceneCameraInfo(camera), worldPos, 0);

    outLayoutPos->x = sead::Mathf::clamp(outLayoutPos->x, -640.0f, 640.0f);
    outLayoutPos->y = sead::Mathf::clamp(outLayoutPos->y, -360.0f, 360.0f);
}

void calcLayoutPosFromWorldPos(sead::Vector3f* outLayoutPos, const SceneCameraInfo* cameraInfo,
                               const sead::Vector3f& worldPos, s32 viewIdx) {
    sead::Viewport viewPort(0.0f, 0.0f, 1280.0f, 720.0f);

    sead::Vector3f cameraPos;
    getLookAtCamera(cameraInfo, viewIdx).worldPosToCameraPosByMatrix(&cameraPos, worldPos);

    sead::Vector2f layoutPos;
    getProjectionSead(cameraInfo, viewIdx).project(&layoutPos, cameraPos, viewPort);

    outLayoutPos->set(layoutPos.x, layoutPos.y, cameraPos.z);
}

f32 calcScreenRadiusFromWorldRadius(const sead::Vector3f& worldPos, const IUseCamera* camera,
                                    f32 worldRadius) {
    sead::Viewport viewPort(0.0f, 0.0f, 1280.0f, 720.0f);

    sead::Vector3f cameraPos;
    getLookAtCamera(camera, 0).worldPosToCameraPosByMatrix(&cameraPos, worldPos);

    cameraPos.x = worldRadius;
    cameraPos.y = 0.0f;

    sead::Vector2f layoutPos;
    getProjectionSead(camera, 0).project(&layoutPos, cameraPos, viewPort);
    return layoutPos.x;
}

f32 calcScreenRadiusFromWorldRadiusSub(const sead::Vector3f& worldPos, const IUseCamera* camera,
                                       f32 worldRadius) {
    sead::Viewport viewPort(0.0f, 0.0f, 0.0f, 0.0f);

    sead::Vector3f cameraPos;
    getLookAtCamera(camera, getViewNumMax(camera) - 1)
        .worldPosToCameraPosByMatrix(&cameraPos, worldPos);

    cameraPos.x = worldRadius;
    cameraPos.y = 0.0f;

    sead::Vector2f layoutPos;
    getProjectionSead(camera, getViewNumMax(camera) - 1).project(&layoutPos, cameraPos, viewPort);
    return layoutPos.x;
}

f32 calcLayoutRadiusFromWorldRadius(const sead::Vector3f& worldPos, const IUseCamera* camera,
                                    f32 worldRadius) {
    sead::Vector3f cameraPos;
    getLookAtCamera(camera, 0).worldPosToCameraPosByMatrix(&cameraPos, worldPos);

    cameraPos.x = worldRadius;
    cameraPos.y = 0.0f;

    sead::Viewport viewPort(0.0f, 0.0f, getProjectionSead(camera, 0).getAspect() * 720.0f, 720.0f);

    sead::Vector2f layoutPos;
    getProjectionSead(camera, 0).project(&layoutPos, cameraPos, viewPort);
    return layoutPos.x;
}

bool calcCameraPosToWorldPosDirFromScreenPos(sead::Vector3f* outCameraPos, const IUseCamera* camera,
                                             const sead::Vector2f& screenPos, f32 scale) {
    SceneCameraInfo* cameraInfo = getSceneCameraInfo(camera);

    sead::Vector2f layoutPos = {screenPos.x - getDisplayWidth() / 2.0f,
                                -(screenPos.y - getDisplayHeight() / 2.0f)};

    sead::Vector3f worldPos;
    calcWorldPosFromLayoutPos(&worldPos, cameraInfo, layoutPos, scale, 0);

    outCameraPos->setSub(worldPos, getCameraPos(cameraInfo, 0));
    return tryNormalizeOrZero(outCameraPos);
}

bool calcCameraPosToWorldPosDirFromScreenPos(sead::Vector3f* outCameraPos, const IUseCamera* camera,
                                             const sead::Vector2f& screenPos,
                                             const sead::Vector3f& scale) {
    SceneCameraInfo* cameraInfo = getSceneCameraInfo(camera);

    sead::Vector2f layoutPos = {screenPos.x - getDisplayWidth() / 2.0f,
                                -(screenPos.y - getDisplayHeight() / 2.0f)};

    sead::Vector3f worldPos;
    calcWorldPosFromLayoutPos(&worldPos, cameraInfo, layoutPos, scale, 0);

    outCameraPos->setSub(worldPos, getCameraPos(cameraInfo, 0));
    return tryNormalizeOrZero(outCameraPos);
}

bool calcCameraPosToWorldPosDirFromScreenPos(sead::Vector3f* outCameraPos,
                                             const SceneCameraInfo* camera,
                                             const sead::Vector2f& screenPos,
                                             const sead::Vector3f& scale, s32 viewIdx) {
    sead::Vector2f layoutPos = {screenPos.x - (viewIdx == 1 ? 0.0f : 640.0f),
                                -(screenPos.y - (viewIdx == 1 ? 0.0f : 360.0f))};

    sead::Vector3f worldPos;
    calcWorldPosFromLayoutPos(&worldPos, camera, layoutPos, scale, viewIdx);

    outCameraPos->setSub(worldPos, getCameraPos(camera, viewIdx));
    return tryNormalizeOrZero(outCameraPos);
}

void calcCameraPosToWorldPosDirFromScreenPosSub(sead::Vector3f* outCameraPos,
                                                const IUseCamera* camera,
                                                const sead::Vector2f& screenPos, f32 scale) {
    sead::Vector2f layoutPos = {screenPos.x, -screenPos.y};

    sead::Vector3f worldPos;
    calcWorldPosFromLayoutPosSub(&worldPos, camera, layoutPos, scale);

    outCameraPos->setSub(worldPos, getCameraPos(camera, getViewNumMax(camera) - 1));
    tryNormalizeOrZero(outCameraPos);
}

void calcCameraPosToWorldPosDirFromScreenPosSub(sead::Vector3f* outCameraPos,
                                                const IUseCamera* camera,
                                                const sead::Vector2f& screenPos,
                                                const sead::Vector3f& scale) {
    sead::Vector2f layoutPos = {screenPos.x, -screenPos.y};

    sead::Vector3f worldPos;
    calcWorldPosFromLayoutPosSub(&worldPos, camera, layoutPos, scale);

    outCameraPos->setSub(worldPos, getCameraPos(camera, getViewNumMax(camera) - 1));
    tryNormalizeOrZero(outCameraPos);
}

void calcLineCameraToWorldPosFromScreenPos(sead::Vector3f* outLineCamera,
                                           sead::Vector3f* outWorldPos, const IUseCamera* camera,
                                           const sead::Vector2f& screenPos, f32 a, f32 b) {
    sead::Vector3f cameraPos;
    sead::Viewport viewPort(0.0f, 0.0f, getProjectionSead(camera, 0).getAspect() * 720.0f, 720.0f);

    const sead::LookAtCamera& lookAt = getLookAtCamera(camera, 0);
    const sead::Projection& projection = getProjectionSead(camera, 0);

    sead::Vector2f layoutPos = {(screenPos.x - 640.0f) / viewPort.getHalfSizeX(),
                                -(screenPos.y - 360.0f) / viewPort.getHalfSizeY()};
    projection.screenPosToCameraPos(&cameraPos, layoutPos);

    sead::Vector3f worldPos;
    lookAt.cameraPosToWorldPosByMatrix(&worldPos, cameraPos);

    sead::Vector3f camerakPos;
    camerakPos.setSub(worldPos, getCameraPos(camera, 0));
    tryNormalizeOrZero(&camerakPos);

    outLineCamera->setScaleAdd(a, camerakPos, getCameraPos(camera, 0));
    outWorldPos->set((b - a) * camerakPos);
}

void calcLineCameraToWorldPosFromScreenPos(sead::Vector3f* outLineCamera,
                                           sead::Vector3f* outWorldPos, const IUseCamera* camera,
                                           const sead::Vector2f& screenPos) {
    calcLineCameraToWorldPosFromScreenPos(outLineCamera, outWorldPos, camera, screenPos,
                                          getNear(camera, 0), getFar(camera, 0));
}

void calcLineCameraToWorldPosFromScreenPosSub(sead::Vector3f* outLineCamera,
                                              sead::Vector3f* outWorldPos, const IUseCamera* camera,
                                              const sead::Vector2f& screenPos, f32 near, f32 far);

void calcLineCameraToWorldPosFromScreenPosSub(sead::Vector3f* outLineCamera,
                                              sead::Vector3f* outWorldPos, const IUseCamera* camera,
                                              const sead::Vector2f& screenPos) {
    calcLineCameraToWorldPosFromScreenPosSub(outLineCamera, outWorldPos, camera, screenPos,
                                             getNear(camera, 0), getFar(camera, 0));
}

void calcWorldPosFromLayoutPos(sead::Vector3f* output, const SceneCameraInfo*,
                               const sead::Vector2f&, f32, s32);

void calcWorldPosFromLayoutPos(sead::Vector3f* output, const SceneCameraInfo*,
                               const sead::Vector2f&, const sead::Vector3f&, s32);

void calcWorldPosFromScreenPos(sead::Vector3f* outWorldPos, const SceneCameraInfo* cameraInfo,
                               const sead::Vector2f& screenPos, f32 scale, s32 viewIdx) {
    sead::Vector2f layoutPos = {screenPos.x - (viewIdx == 1 ? 0.0f : 640.0f),
                                -(screenPos.y - (viewIdx == 1 ? 0.0f : 360.0f))};

    calcWorldPosFromLayoutPos(outWorldPos, cameraInfo, layoutPos, scale, viewIdx);
}

void calcWorldPosFromScreenPos(sead::Vector3f* outWorldPos, const SceneCameraInfo* cameraInfo,
                               const sead::Vector2f& screenPos, const sead::Vector3f& scale,
                               s32 viewIdx) {
    sead::Vector2f layoutPos = {screenPos.x - (viewIdx == 1 ? 0.0f : 640.0f),
                                -(screenPos.y - (viewIdx == 1 ? 0.0f : 360.0f))};

    calcWorldPosFromLayoutPos(outWorldPos, cameraInfo, layoutPos, scale, viewIdx);
}

void calcLayoutPosFromWorldPos(sead::Vector2f* outLayoutPos, const SceneCameraInfo* cameraInfo,
                               const sead::Vector3f& worldPos, s32 viewIdx) {
    sead::Viewport viewPort(0.0f, 0.0f, 1280.0f, 720.0f);
    getLookAtCamera(cameraInfo, viewIdx)
        .projectByMatrix(outLayoutPos, worldPos, getProjectionSead(cameraInfo, viewIdx), viewPort);
}

void calcLineCameraToWorldPosFromScreenPos(sead::Vector3f* output1, sead::Vector3f* output2,
                                           const SceneCameraInfo*, const sead::Vector2f&, f32, f32,
                                           s32);

void calcLineCameraToWorldPosFromScreenPos(sead::Vector3f* output1, sead::Vector3f* output2,
                                           const SceneCameraInfo*, const sead::Vector2f&, s32);

void calcWorldPosFromScreenPos(sead::Vector3f* output, const SceneCameraInfo*,
                               const sead::Vector2f&, f32, s32);

bool calcCameraPosToWorldPosDirFromScreenPos(sead::Vector3f* output, const SceneCameraInfo*,
                                             const sead::Vector2f&, f32, s32);
}  // namespace al

#include "Library/Screen/ScreenFunction.h"

#include <gfx/seadProjection.h>

#include "Library/Camera/CameraUtil.h"
#include "Library/Camera/CameraViewInfo.h"
#include "Library/Camera/SceneCameraInfo.h"
#include "Library/Math/MathUtil.h"
#include "Library/Projection/Projection.h"

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
                                          const sead::Vector2f& layoutPos) {
    outScreenPos->x = layoutPos.x + getSubDisplayWidth() / 2.0f;
    outScreenPos->y = -layoutPos.y + getSubDisplayHeight() / 2.0f;
}

void calcLayoutPosFromScreenPos(sead::Vector2f* outLayoutPos, const sead::Vector2f& screenPos) {
    outLayoutPos->x = screenPos.x - getLayoutDisplayWidth() / 2.0f;
    outLayoutPos->y = -screenPos.y + getLayoutDisplayHeight() / 2.0f;
}

bool calcWorldPosFromScreen(sead::Vector3f* outWorldPos, const sead::Vector2f& screenPos,
                            const sead::Matrix34f& viewMtx, f32 zPos) {
    sead::Vector2f pos = screenPos;
    f32 screenScale = 360.0f * sead::Mathf::tan(sead::Mathf::pi() / 8.0f);
    zPos = zPos >= 0.0f ? zPos : screenScale;

    if (!outWorldPos)
        return true;

    f32 rate = zPos / screenScale;
    f32 negScale = -zPos;
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
                               const sead::Vector2f& screenPos, f32 zPos) {
    sead::Vector2f layoutPos = {screenPos.x - getDisplayWidth() / 2.0f,
                                -(screenPos.y - getDisplayHeight() / 2.0f)};
    calcWorldPosFromLayoutPos(outWorldPos, camera, layoutPos, zPos);
}

inline void normalizeCamera(sead::Vector3f* cameraPos, f32 zz) {
    if (zz < -0.0f) {
        const f32 inv_len = zz / cameraPos->z;
        cameraPos->x *= inv_len;
        cameraPos->y *= inv_len;
        cameraPos->z *= inv_len;
    }
}

void calcWorldPosFromLayoutPos(sead::Vector3f* outWorldPos, const IUseCamera* camera,
                               const sead::Vector2f& layoutPos, f32 zPos) {
    sead::Vector2f screenPos;
    sead::Vector3f cameraPos;
    sead::Viewport viewPort(0.0f, 0.0f, getProjectionSead(camera, 0).getAspect() * 720.0f, 720.0f);

    const sead::LookAtCamera& lookAt = getLookAtCamera(camera, 0);
    const sead::Projection& projection = getProjectionSead(camera, 0);

    screenPos = layoutPos;
    screenPos.x /= viewPort.getHalfSizeX();
    screenPos.y /= viewPort.getHalfSizeY();
    projection.screenPosToCameraPos(&cameraPos, screenPos);

    normalizeCamera(&cameraPos, -zPos);

    lookAt.cameraPosToWorldPosByMatrix(outWorldPos, cameraPos);
}

void calcWorldPosFromScreenPos(sead::Vector3f* outWorldPos, const IUseCamera* camera,
                               const sead::Vector2f& screenPos, const sead::Vector3f& worldPos) {
    sead::Vector2f layoutPos = {screenPos.x - getDisplayWidth() / 2.0f,
                                -(screenPos.y - getDisplayHeight() / 2.0f)};
    calcWorldPosFromLayoutPos(outWorldPos, camera, layoutPos, worldPos);
}

void calcWorldPosFromLayoutPos(sead::Vector3f* outWorldPos, const IUseCamera* camera,
                               const sead::Vector2f& layoutPos, const sead::Vector3f& worldPos) {
    sead::Vector2f screenPos;
    sead::Vector3f cameraPos;
    sead::Vector3f worldCameraPos;
    sead::Viewport viewPort(0.0f, 0.0f, getProjectionSead(camera, 0).getAspect() * 720.0f, 720.0f);

    const sead::LookAtCamera& lookAt = getLookAtCamera(camera, 0);
    const sead::Projection& projection = getProjectionSead(camera, 0);

    lookAt.worldPosToCameraPosByMatrix(&worldCameraPos, worldPos);
    f32 cameraZ = worldCameraPos.z;

    screenPos = layoutPos;
    screenPos.x /= viewPort.getHalfSizeX();
    screenPos.y /= viewPort.getHalfSizeY();
    projection.screenPosToCameraPos(&cameraPos, screenPos);

    normalizeCamera(&cameraPos, cameraZ);

    lookAt.cameraPosToWorldPosByMatrix(outWorldPos, cameraPos);
}

void calcWorldPosFromScreenPosSub(sead::Vector3f* outWorldPos, const IUseCamera* camera,
                                  const sead::Vector2f& screenPos, f32 zPos) {
    sead::Vector2f layoutPos = {screenPos.x, -screenPos.y};
    calcWorldPosFromLayoutPosSub(outWorldPos, camera, layoutPos, zPos);
}

void calcWorldPosFromLayoutPosSub(sead::Vector3f* outWorldPos, const IUseCamera* camera,
                                  const sead::Vector2f& layoutPos, f32 zPos) {
    sead::Vector2f screenPos;
    sead::Vector3f cameraPos;
    sead::Vector3f worldCameraPos;
    sead::Viewport viewPort(0.0f, 0.0f, 0.0f, 0.0f);

    const sead::LookAtCamera& lookAt = getLookAtCamera(camera, getViewNumMax(camera) - 1);
    const sead::Projection& projection = getProjectionSead(camera, getViewNumMax(camera) - 1);

    screenPos = layoutPos;
    screenPos.x /= viewPort.getHalfSizeX();
    screenPos.y /= viewPort.getHalfSizeY();
    projection.screenPosToCameraPos(&cameraPos, screenPos);

    normalizeCamera(&cameraPos, -zPos);

    lookAt.cameraPosToWorldPosByMatrix(outWorldPos, cameraPos);
}

void calcWorldPosFromScreenPosSub(sead::Vector3f* outWorldPos, const IUseCamera* camera,
                                  const sead::Vector2f& screenPosSub,
                                  const sead::Vector3f& worldPos) {
    sead::Vector2f layoutPosSub = {screenPosSub.x, -screenPosSub.y};
    calcWorldPosFromLayoutPosSub(outWorldPos, camera, layoutPosSub, worldPos);
}

void calcWorldPosFromLayoutPosSub(sead::Vector3f* outWorldPos, const IUseCamera* camera,
                                  const sead::Vector2f& layoutPos, const sead::Vector3f& worldPos) {
    sead::Vector2f screenPos;
    sead::Vector3f cameraPos;
    sead::Vector3f worldCameraPos;
    sead::Viewport viewPort(0.0f, 0.0f, 0.0f, 0.0f);

    const sead::LookAtCamera& lookAt = getLookAtCamera(camera, getViewNumMax(camera) - 1);
    const sead::Projection& projection = getProjectionSead(camera, getViewNumMax(camera) - 1);

    lookAt.worldPosToCameraPosByMatrix(&worldCameraPos, worldPos);
    f32 cameraZ = worldCameraPos.z;

    screenPos = layoutPos;
    screenPos.x /= viewPort.getHalfSizeX();
    screenPos.y /= viewPort.getHalfSizeY();
    projection.screenPosToCameraPos(&cameraPos, screenPos);

    normalizeCamera(&cameraPos, cameraZ);

    lookAt.cameraPosToWorldPosByMatrix(outWorldPos, cameraPos);
}

}  // namespace al

namespace ScreenFunction {

inline void normalizeCamera(sead::Vector3f* cameraPos, f32 zz) {
    if (zz < -0.0f) {
        const f32 inv_len = zz / cameraPos->z;
        cameraPos->x *= inv_len;
        cameraPos->y *= inv_len;
        cameraPos->z *= inv_len;
    }
}

void calcWorldPositionFromCenterScreen(sead::Vector3f* outWorldPos, const sead::Vector2f& layoutPos,
                                       const sead::Vector3f& worldPos, const sead::Camera& camera,
                                       const sead::Projection& projection,
                                       const sead::Viewport& viewPort) {
    sead::Vector2f screenPos;
    sead::Vector3f cameraPos;
    sead::Vector3f worldCameraPos;

    camera.worldPosToCameraPosByMatrix(&worldCameraPos, worldPos);
    f32 cameraZ = worldCameraPos.z;

    screenPos = layoutPos;
    screenPos.x /= viewPort.getHalfSizeX();
    screenPos.y /= viewPort.getHalfSizeY();

    projection.screenPosToCameraPos(&cameraPos, screenPos);

    normalizeCamera(&cameraPos, cameraZ);

    camera.cameraPosToWorldPosByMatrix(outWorldPos, cameraPos);
}

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
                                             const sead::Vector2f& screenPos, f32 zPos) {
    return calcCameraPosToWorldPosDirFromScreenPos(outCameraPos, getSceneCameraInfo(camera),
                                                   screenPos, zPos, 0);
}

bool calcCameraPosToWorldPosDirFromScreenPos(sead::Vector3f* outCameraPos,
                                             const SceneCameraInfo* cameraInfo,
                                             const sead::Vector2f& screenPos, f32 zPos,
                                             s32 viewIdx) {
    sead::Vector2f layoutPos = {screenPos.x - (viewIdx == 1 ? 0.0f : 640.0f),
                                -(screenPos.y - (viewIdx == 1 ? 0.0f : 360.0f))};
    sead::Vector3f worldPos;
    calcWorldPosFromLayoutPos(&worldPos, cameraInfo, layoutPos, zPos, viewIdx);

    outCameraPos->setSub(worldPos, getCameraPos(cameraInfo, viewIdx));
    return tryNormalizeOrZero(outCameraPos);
}

bool calcCameraPosToWorldPosDirFromScreenPos(sead::Vector3f* outCameraPos, const IUseCamera* camera,
                                             const sead::Vector2f& screenPos,
                                             const sead::Vector3f& zPos) {
    return calcCameraPosToWorldPosDirFromScreenPos(outCameraPos, getSceneCameraInfo(camera),
                                                   screenPos, zPos, 0);
}

bool calcCameraPosToWorldPosDirFromScreenPos(sead::Vector3f* outCameraPos,
                                             const SceneCameraInfo* camera,
                                             const sead::Vector2f& screenPos,
                                             const sead::Vector3f& zPos, s32 viewIdx) {
    sead::Vector2f layoutPos = {screenPos.x - (viewIdx == 1 ? 0.0f : 640.0f),
                                -(screenPos.y - (viewIdx == 1 ? 0.0f : 360.0f))};

    sead::Vector3f worldPos;
    calcWorldPosFromLayoutPos(&worldPos, camera, layoutPos, zPos, viewIdx);

    outCameraPos->setSub(worldPos, getCameraPos(camera, viewIdx));
    return tryNormalizeOrZero(outCameraPos);
}

void calcCameraPosToWorldPosDirFromScreenPosSub(sead::Vector3f* outCameraPos,
                                                const IUseCamera* camera,
                                                const sead::Vector2f& screenPos, f32 zPos) {
    sead::Vector2f layoutPos = {screenPos.x, -screenPos.y};

    sead::Vector3f worldPos;
    calcWorldPosFromLayoutPosSub(&worldPos, camera, layoutPos, zPos);

    outCameraPos->setSub(worldPos, getCameraPos(camera, getViewNumMax(camera) - 1));
    tryNormalizeOrZero(outCameraPos);
}

void calcCameraPosToWorldPosDirFromScreenPosSub(sead::Vector3f* outCameraPos,
                                                const IUseCamera* camera,
                                                const sead::Vector2f& screenPos,
                                                const sead::Vector3f& zPos) {
    sead::Vector2f layoutPos = {screenPos.x, -screenPos.y};

    sead::Vector3f worldPos;
    calcWorldPosFromLayoutPosSub(&worldPos, camera, layoutPos, zPos);

    outCameraPos->setSub(worldPos, getCameraPos(camera, getViewNumMax(camera) - 1));
    tryNormalizeOrZero(outCameraPos);
}

void calcLineCameraToWorldPosFromScreenPos(sead::Vector3f* outLineCamera,
                                           sead::Vector3f* outWorldPos, const IUseCamera* camera,
                                           const sead::Vector2f& _screenPos, f32 near, f32 far) {
    sead::Vector2f screenPos = {_screenPos.x - 640.0f, -(_screenPos.y- 360.0f)};
    sead::Vector3f cameraPos;
    sead::Viewport viewPort(0.0f, 0.0f, getProjectionSead(camera, 0).getAspect() * 720.0f, 720.0f);

    const sead::LookAtCamera& lookAt = getLookAtCamera(camera, 0);
    const sead::Projection& projection = getProjectionSead(camera, 0);

    screenPos.x /= viewPort.getHalfSizeX();
    screenPos.y /= viewPort.getHalfSizeY();
    projection.screenPosToCameraPos(&cameraPos, screenPos);

    sead::Vector3f worldPos;
    lookAt.cameraPosToWorldPosByMatrix(&worldPos, cameraPos);

    worldPos -= getCameraPos(camera, getViewNumMax(camera) - 1);
    tryNormalizeOrZero(&worldPos);

    outLineCamera->setScaleAdd(near, worldPos, getCameraPos(camera, 0));
    outWorldPos->set((near - far) * worldPos);
}

void calcLineCameraToWorldPosFromScreenPos(sead::Vector3f* outLineCamera,
                                           sead::Vector3f* outWorldPos, const IUseCamera* camera,
                                           const sead::Vector2f& screenPos) {
    calcLineCameraToWorldPosFromScreenPos(outLineCamera, outWorldPos, camera, screenPos,
                                          getNear(camera, 0), getFar(camera, 0));
}

void calcLineCameraToWorldPosFromScreenPosSub(sead::Vector3f* outLineCamera,
                                              sead::Vector3f* outWorldPos, const IUseCamera* camera,
                                              const sead::Vector2f& _screenPos, f32 near, f32 far) {
    sead::Vector2f screenPos = {_screenPos.x, -_screenPos.y};
    sead::Vector3f cameraPos;
    sead::Viewport viewPort(0.0f, 0.0f, 0.0f, 0.0f);

    const sead::LookAtCamera& lookAt = getLookAtCamera(camera, getViewNumMax(camera) - 1);
    const sead::Projection& projection = getProjectionSead(camera, getViewNumMax(camera) - 1);

    screenPos.x /= viewPort.getHalfSizeX();
    screenPos.y /= viewPort.getHalfSizeY();
    projection.screenPosToCameraPos(&cameraPos, screenPos);

    sead::Vector3f worldPos;
    lookAt.cameraPosToWorldPosByMatrix(&worldPos, cameraPos);

    worldPos-=getCameraPos(camera, getViewNumMax(camera) - 1);
    tryNormalizeOrZero(&worldPos);

    outLineCamera->setScaleAdd(near, worldPos, getCameraPos(camera, getViewNumMax(camera) - 1));
    outWorldPos->set((near - far) * worldPos);
}

void calcLineCameraToWorldPosFromScreenPosSub(sead::Vector3f* outLineCamera,
                                              sead::Vector3f* outWorldPos, const IUseCamera* camera,
                                              const sead::Vector2f& screenPos) {
    calcLineCameraToWorldPosFromScreenPosSub(outLineCamera, outWorldPos, camera, screenPos,
                                             getNear(camera, 0), getFar(camera, 0));
}

void calcWorldPosFromLayoutPos(sead::Vector3f* outWorldPos, const SceneCameraInfo* cameraInfo,
                               const sead::Vector2f& layoutPos, f32 zPos, s32 viewIdx) {
    sead::Vector2f screenPos;
    sead::Vector3f cameraPos;
    f32 viewPortHeight = (viewIdx == 1 ? 0.0f : 720.0f);
    sead::Viewport viewPort(
        0.0f, 0.0f, cameraInfo->getViewAt(viewIdx)->getProjection().getAspect() * viewPortHeight,
        viewPortHeight);

    const sead::LookAtCamera& lookAt = getLookAtCamera(cameraInfo, viewIdx);
    const sead::Projection& projection = getProjectionSead(cameraInfo, viewIdx);

    screenPos = layoutPos;
    screenPos.x /= viewPort.getHalfSizeX();
    screenPos.y /= viewPort.getHalfSizeY();
    projection.screenPosToCameraPos(&cameraPos, screenPos);

    normalizeCamera(&cameraPos, -zPos);

    lookAt.cameraPosToWorldPosByMatrix(outWorldPos, cameraPos);
}

void calcWorldPosFromLayoutPos(sead::Vector3f* outWorldPos, const SceneCameraInfo* cameraInfo,
                               const sead::Vector2f& layoutPos, const sead::Vector3f& worldPos,
                               s32 viewIdx) {
    sead::Vector2f screenPos;
    sead::Vector3f cameraPos;
    f32 viewPortHeight = (viewIdx == 1 ? 0.0f : 720.0f);
    sead::Viewport viewPort(
        0.0f, 0.0f, cameraInfo->getViewAt(viewIdx)->getProjection().getAspect() * viewPortHeight,
        viewPortHeight);

    const sead::LookAtCamera& lookAt = getLookAtCamera(cameraInfo, viewIdx);
    const sead::Projection& projection = getProjectionSead(cameraInfo, viewIdx);

    lookAt.worldPosToCameraPosByMatrix(&cameraPos, worldPos);
    f32 cameraZ = cameraPos.z;

    screenPos = layoutPos;
    screenPos.x /= viewPort.getHalfSizeX();
    screenPos.y /= viewPort.getHalfSizeY();
    projection.screenPosToCameraPos(&cameraPos, screenPos);

    normalizeCamera(&cameraPos, cameraZ);

    lookAt.cameraPosToWorldPosByMatrix(outWorldPos, cameraPos);
}

void calcWorldPosFromScreenPos(sead::Vector3f* outWorldPos, const SceneCameraInfo* cameraInfo,
                               const sead::Vector2f& screenPos, f32 zPos, s32 viewIdx) {
    sead::Vector2f layoutPos = {screenPos.x - (viewIdx == 1 ? 0.0f : 640.0f),
                                -(screenPos.y - (viewIdx == 1 ? 0.0f : 360.0f))};

    calcWorldPosFromLayoutPos(outWorldPos, cameraInfo, layoutPos, zPos, viewIdx);
}

void calcWorldPosFromScreenPos(sead::Vector3f* outWorldPos, const SceneCameraInfo* cameraInfo,
                               const sead::Vector2f& screenPos, const sead::Vector3f& zPos,
                               s32 viewIdx) {
    sead::Vector2f layoutPos = {screenPos.x - (viewIdx == 1 ? 0.0f : 640.0f),
                                -(screenPos.y - (viewIdx == 1 ? 0.0f : 360.0f))};

    calcWorldPosFromLayoutPos(outWorldPos, cameraInfo, layoutPos, zPos, viewIdx);
}

void calcLayoutPosFromWorldPos(sead::Vector2f* outLayoutPos, const SceneCameraInfo* cameraInfo,
                               const sead::Vector3f& worldPos, s32 viewIdx) {
    sead::Viewport viewPort(0.0f, 0.0f, 1280.0f, 720.0f);
    getLookAtCamera(cameraInfo, viewIdx)
        .projectByMatrix(outLayoutPos, worldPos, getProjectionSead(cameraInfo, viewIdx), viewPort);
}

void calcLineCameraToWorldPosFromScreenPos(sead::Vector3f* outLineCamera,
                                           sead::Vector3f* outWorldPos,
                                           const SceneCameraInfo* cameraInfo,
                                           const sead::Vector2f& screenPos, f32 near, f32 far,
                                           s32 viewIdx);

void calcLineCameraToWorldPosFromScreenPos(sead::Vector3f* outLineCamera,
                                           sead::Vector3f* outWorldPos,
                                           const SceneCameraInfo* cameraInfo,
                                           const sead::Vector2f& screenPos, s32 viewIdx) {
    calcLineCameraToWorldPosFromScreenPos(outLineCamera, outWorldPos, cameraInfo, screenPos,
                                          getNear(cameraInfo, 0), getFar(cameraInfo, 0), viewIdx);
}

}  // namespace al

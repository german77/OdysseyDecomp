#include "Library/Layout/LayoutActorUtil.h"

#include <nn/ui2d/Layout.h>
#include <nn/ui2d/Pane.h>

#include "Library/Layout/IUseLayout.h"
#include "Library/Layout/LayoutActorUtil.h"
#include "Library/Layout/LayoutKeeper.h"
#include "Library/Layout/LayoutPaneGroup.h"
#include "Library/Matrix/MatrixUtil.h"

namespace al {
bool killLayoutIfActive(LayoutActor*);
bool appearLayoutIfDead(LayoutActor*);
bool isActive(const LayoutActor*);
bool isDead(const LayoutActor*);
void calcTrans(sead::Vector3f*, const IUseLayout*);
sead::Vector2f getLocalTrans(const IUseLayout*);
sead::Vector2f* getLocalTransPtr(const IUseLayout*);
void calcScale(sead::Vector3f*, const IUseLayout*);
f32 getLocalScale(const IUseLayout*);
void setLocalTrans(IUseLayout*, const sead::Vector3f&);
void setLocalTrans(IUseLayout*, const sead::Vector2f&);
void setLocalScale(IUseLayout*, f32);
void setLocalScale(IUseLayout*, const sead::Vector2f&);
void setLocalAlpha(IUseLayout*, f32);

void calcPaneTrans(sead::Vector3f* outTrans, const IUseLayout* user, const char* name) {
    sead::Matrix34f mtx;
    calcPaneMtx(&mtx, user, name);

    outTrans->x = mtx.getTranslation().x;
    outTrans->y = mtx.getTranslation().y;
    outTrans->z = mtx.getTranslation().z;
}

void calcPaneMtx(sead::Matrix34f* outMtx, const IUseLayout* user, const char* name) {
    makeMtx34f(outMtx, getPaneMtx(user, name));
}

void calcPaneTrans(sead::Vector2f* outTrans, const IUseLayout* user, const char* name) {
    sead::Matrix34f mtx;
    calcPaneMtx(&mtx, user, name);

    outTrans->set(mtx.getTranslation().x, mtx.getTranslation().y);
}

void calcPaneScale(sead::Vector3f* outScale, const IUseLayout* user, const char* name) {
    sead::Matrix34f mtx;
    calcPaneMtx(&mtx, user, name);

    sead::Vector3f scale = {mtx.getBase(0).length(), mtx.getBase(1).length(),
                            mtx.getBase(2).length()};
    outScale->set(scale);
}

void calcPaneSize(sead::Vector3f*, const IUseLayout*, const char*);

const nn::util::neon::MatrixColumnMajor4x3fType& getPaneMtx(const IUseLayout* user,
                                                            const char* name) {
    return user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true)->GetMtx();
}

const nn::util::neon::MatrixColumnMajor4x3fType& getPaneMtxRaw(const IUseLayout* user,
                                                               const char* name) {
    return user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true)->GetMtx();
}

f32 getGlobalAlpha(const IUseLayout* user, const char* name) {
    return user->getLayoutKeeper()
        ->getLayout()
        ->GetPane()
        ->FindPaneByName(name, true)
        ->GetGlobalAlpha();
}

void setPaneLocalTrans(IUseLayout* user, const char* name, const sead::Vector2f& trans) {
    user->getLayoutKeeper()
        ->getLayout()
        ->GetPane()
        ->FindPaneByName(name, true)
        ->SetPosition({{{trans.x, trans.y, 0}}});
}

void setPaneLocalTrans(IUseLayout* user, const char* name, const sead::Vector3f& trans) {
    user->getLayoutKeeper()
        ->getLayout()
        ->GetPane()
        ->FindPaneByName(name, true)
        ->SetPosition({{{trans.x, trans.y, trans.z}}});
}

void setPaneLocalRotate(IUseLayout* user, const char* name, const sead::Vector3f& rotation) {
    user->getLayoutKeeper()
        ->getLayout()
        ->GetPane()
        ->FindPaneByName(name, true)
        ->SetRotation({{{rotation.x, rotation.y, rotation.z}}});
}

void setPaneLocalScale(IUseLayout* user, const char* name, const sead::Vector2f& scale) {
    user->getLayoutKeeper()
        ->getLayout()
        ->GetPane()
        ->FindPaneByName(name, true)
        ->SetScale({{{scale.x, scale.y}}});
}

void setPaneLocalSize(IUseLayout* user, const char* name, const sead::Vector2f& size) {
    user->getLayoutKeeper()
        ->getLayout()
        ->GetPane()
        ->FindPaneByName(name, true)
        ->SetSize({{{size.x, size.y}}});
}

void setPaneLocalAlpha(IUseLayout* user, const char* name, f32 alpha) {
    user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true)->SetAlpha(alpha);
}

sead::Vector3f getPaneLocalTrans(const IUseLayout* user, const char* name) {
    const nn::util::Float3& trans =
        user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true)->GetPosition();
    return {trans.x, trans.y, trans.z};
}

void getPaneLocalSize(sead::Vector2f* outSize, const IUseLayout* user, const char* name) {
    const nn::util::Float2& size =
        user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true)->GetSize();
    outSize->set(size.x, size.y);
}

sead::Vector3f getPaneLocalRotate(const IUseLayout* user, const char* name) {
    const nn::util::Float3& rotation =
        user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true)->GetRotation();
    return {rotation.x, rotation.y, rotation.z};
}

sead::Vector2f getPaneLocalScale(const IUseLayout* user, const char* name) {
    const nn::util::Float2& scale =
        user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true)->GetScale();
    return {scale.x, scale.y};
}

sead::Vector2f getTextBoxDrawRectSize(const IUseLayout*, const char*);

void showPane(IUseLayout* user, const char* name) {
    showPaneRecursive(user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true));
}

void hidePane(IUseLayout* user, const char* name) {
    hidePaneRecursive(user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true));
}

void showPaneNoRecursive(IUseLayout* user, const char* name) {
    user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true)->Show();
}

void hidePaneNoRecursive(IUseLayout* user, const char* name) {
    user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true)->Hide();
}

bool isHidePane(const IUseLayout* user, const char* name) {
    return !user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true)->IsShow();
}

void showPaneRoot(IUseLayout* user) {
    showPaneRecursive(user->getLayoutKeeper()->getLayout()->GetPane());
}

void hidePaneRoot(IUseLayout* user) {
    hidePaneRecursive(user->getLayoutKeeper()->getLayout()->GetPane());
}

void showPaneRootNoRecursive(IUseLayout* user) {
    user->getLayoutKeeper()->getLayout()->GetPane()->Show();
}

void hidePaneRootNoRecursive(IUseLayout* user) {
    user->getLayoutKeeper()->getLayout()->GetPane()->Hide();
}

bool isHidePaneRoot(const IUseLayout* user) {
    return !user->getLayoutKeeper()->getLayout()->GetPane()->IsShow();
}

bool isExistPane(const IUseLayout* user, const char* name) {
    return user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true) != nullptr;
}

bool isContainPointPane(const IUseLayout*, const char*, const sead::Vector2f&);
void findHitPaneFromLayoutPos(const IUseLayout*, const sead::Vector2f&);
bool isExistHitPaneFromLayoutPos(const IUseLayout*, const sead::Vector2f&);
void findHitPaneFromScreenPos(const IUseLayout*, const sead::Vector2f&);
bool isExistHitPaneFromScreenPos(const IUseLayout*, const sead::Vector2f&);
bool isTouchPosInPane(const IUseLayout*, const char*);
void setCursorPanePos(IUseLayout*, const IUseLayout*);
void setPaneVtxColor(const IUseLayout*, const char*, const sead::Color4u8&);
bool isTriggerTouchPane(const IUseLayout*, const char*);
bool isHoldTouchPane(const IUseLayout*, const char*);
bool isReleaseTouchPane(const IUseLayout*, const char*);
s32 getPaneChildNum(const IUseLayout*, const char*);
const char* getPaneChildName(const IUseLayout*, const char*, s32);
void setPaneStringLength(IUseLayout*, const char*, const char16*, u16, u16);
void setPaneString(IUseLayout*, const char*, const char16*, u16);
void setPaneCounterDigit1(IUseLayout*, const char*, s32, u16);
void setPaneCounterDigit2(IUseLayout*, const char*, s32, u16);
void setPaneCounterDigit3(IUseLayout*, const char*, s32, u16);
void setPaneCounterDigit4(IUseLayout*, const char*, s32, u16);
void setPaneCounterDigit5(IUseLayout*, const char*, s32, u16);
void setPaneCounterDigit6(IUseLayout*, const char*, s32, u16);
void setPaneNumberDigit1(IUseLayout*, const char*, s32, u16);
void setPaneNumberDigit2(IUseLayout*, const char*, s32, u16);
void setPaneNumberDigit3(IUseLayout*, const char*, s32, u16);
void setPaneNumberDigit4(IUseLayout*, const char*, s32, u16);
void setPaneNumberDigit5(IUseLayout*, const char*, s32, u16);
void setPaneStringFormat(IUseLayout*, const char*, const char*, ...);
void setTextPositionCenterH(IUseLayout*, const char*);
void initPaneMessage(IUseLayout*, const char*, const MessageHolder*, const char*, u32);
void setPaneSystemMessage(LayoutActor*, const char*, const char*, const char*);
void setPaneStageMessage(LayoutActor*, const char*, const char*, const char*);
const char16* getPaneStringBuffer(const IUseLayout*, const char*);
s32 getPaneStringBufferLength(const IUseLayout*, const char*);
void setTextBoxPaneFont(const LayoutActor*, const char*, const char*);
void adjustPaneSizeToTextSizeAll(const LayoutActor*);
void requestCaptureRecursive(const LayoutActor*);
void setRubyScale(const LayoutActor*, f32);
nn::ui2d::TextureInfo* createTextureInfo();
nn::ui2d::TextureInfo* createTextureInfo(const agl::TextureData&);
nn::ui2d::TextureInfo* createTextureInfo(const IUseLayout*, const char*);
void getPaneTextureInfo(nn::ui2d::TextureInfo*, const IUseLayout*, const char*);
nn::ui2d::TextureInfo* createTextureInfo(const char*, const char*, const char*);
void updateTextureInfo(nn::ui2d::TextureInfo*, const agl::TextureData&);
void setPaneTexture(IUseLayout*, const char*, const nn::ui2d::TextureInfo*);
void registerLayoutPartsActor(LayoutActor*, LayoutActor*);
void updateLayoutPaneRecursive(LayoutActor*);
s32 getLayoutPaneGroupNum(LayoutActor*);
LayoutPaneGroup* getLayoutPaneGroup(LayoutActor*, s32);
}  // namespace al

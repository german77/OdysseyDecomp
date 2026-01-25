#include "Library/Layout/LayoutActorUtil.h"

#include <nn/ui2d/Layout.h>
#include <nn/ui2d/Pane.h>
#include <nn/ui2d/Util.h>

#include "Library/Controller/InputFunction.h"
#include "Library/Layout/IUseLayout.h"
#include "Library/Layout/LayoutActor.h"
#include "Library/Layout/LayoutActorUtil.h"
#include "Library/Layout/LayoutKeeper.h"
#include "Library/Layout/LayoutPaneGroup.h"
#include "Library/Math/MathUtil.h"
#include "Library/Matrix/MatrixUtil.h"

namespace al {
bool killLayoutIfActive(LayoutActor* layout) {
    if (layout->isAlive()) {
        layout->kill();
        return true;
    }
    return false;
}

bool appearLayoutIfDead(LayoutActor* layout) {
    if (!layout->isAlive()) {
        layout->appear();
        return true;
    }
    return false;
}

bool isActive(const LayoutActor* layout) {
    return layout->isAlive();
}

bool isDead(const LayoutActor* layout) {
    return !layout->isAlive();
}

void calcTrans(sead::Vector3f* outTrans, const IUseLayout* user) {
    const nn::util::MatrixT4x3fType& mtx =
        user->getLayoutKeeper()->getLayout()->GetPane()->GetMtx();
    outTrans->x = mtx.m[0][3];
    outTrans->y = mtx.m[1][3];
    outTrans->z = mtx.m[2][3];
}

sead::Vector3f getLocalTrans(const IUseLayout* user) {
    const nn::util::Float3& trans = user->getLayoutKeeper()->getLayout()->GetPane()->GetPosition();
    return {trans.x, trans.y, trans.z};
}

const sead::Vector3f* getLocalTransPtr(const IUseLayout* user) {
    const nn::util::Float3& trans = user->getLayoutKeeper()->getLayout()->GetPane()->GetPosition();
    return reinterpret_cast<const sead::Vector3f*>(&trans);
}

void calcScale(sead::Vector3f* outScale, const IUseLayout* user) {
    const nn::util::MatrixT4x3fType& mtx =
        user->getLayoutKeeper()->getLayout()->GetPane()->GetMtx();

    sead::Vector3f scale;  // = {mtx.getBase(0).length(), mtx.getBase(1).length(),
                           //   mtx.getBase(2).length()};
    outScale->set(scale);
}

sead::Vector2f getLocalScale(const IUseLayout* user) {
    const nn::util::Float2& scale = user->getLayoutKeeper()->getLayout()->GetPane()->GetScale();
    return {scale.x, scale.y};
}

void setLocalTrans(IUseLayout* user, const sead::Vector3f& trans) {
    user->getLayoutKeeper()->getLayout()->GetPane()->SetPosition({{{trans.x, trans.y, trans.z}}});
}

void setLocalTrans(IUseLayout* user, const sead::Vector2f& trans) {
    user->getLayoutKeeper()->getLayout()->GetPane()->SetPosition({{{trans.x, trans.y, 0.0f}}});
}

void setLocalScale(IUseLayout* user, f32 scale) {
    user->getLayoutKeeper()->getLayout()->GetPane()->SetScale({{{scale, scale}}});
}

void setLocalScale(IUseLayout* user, const sead::Vector2f& scale) {
    user->getLayoutKeeper()->getLayout()->GetPane()->SetScale({{{scale.x, scale.y}}});
}

void setLocalAlpha(IUseLayout* user, f32 alpha){
    user->getLayoutKeeper()->getLayout()->GetPane()->SetAlpha(alpha);
}

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

const nn::util::MatrixT4x3fType& getPaneMtx(const IUseLayout* user, const char* name) {
    return user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true)->GetMtx();
}

const nn::util::MatrixT4x3fType& getPaneMtxRaw(const IUseLayout* user, const char* name) {
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
        ->SetSize({size.x, size.y});
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
    const nn::ui2d::Size& size =
        user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true)->GetSize();
    outSize->set(size.width, size.height);
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
    return !user->getLayoutKeeper()
                ->getLayout()
                ->GetPane()
                ->FindPaneByName(name, true)
                ->IsVisible();
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
    return !user->getLayoutKeeper()->getLayout()->GetPane()->IsVisible();
}

bool isExistPane(const IUseLayout* user, const char* name) {
    return user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true) != nullptr;
}

bool isContainPointPane(const IUseLayout* user, const char* name, const sead::Vector2f& point) {
    nn::ui2d::Pane* pane =
        user->getLayoutKeeper()->getLayout()->GetPane()->FindPaneByName(name, true);
    const nn::util::MatrixT4x3fType& mtx = pane->GetMtx();
    f32 aa = mtx.m[1][1];

    if (!isNearZero(mtx.m[0][0])) {
        if (!isNearZero(aa))
            return nn::ui2d::IsContain(pane, {{{point.x, point.y}}});
    }
    return false;
}

void findHitPaneFromLayoutPos(const IUseLayout*, const sead::Vector2f&);
bool isExistHitPaneFromLayoutPos(const IUseLayout*, const sead::Vector2f&);
void findHitPaneFromScreenPos(const IUseLayout*, const sead::Vector2f&);
bool isExistHitPaneFromScreenPos(const IUseLayout*, const sead::Vector2f&);

bool isTouchPosInPane(const IUseLayout* user, const char* name) {
    sead::Vector2f pos = {0.0f, 0.0f};
    calcTouchLayoutPos(&pos);
    return isContainPointPane(user, name, pos);
}

void setCursorPanePos(IUseLayout* user, const IUseLayout* user2) {
    sead::Vector2f size;
    getPaneLocalSize(&size, user2, "CursorPosition");
    sead::Matrix34f mtx;
    calcPaneMtx(&mtx, user2, "CursorPosition");

    const nn::util::MatrixT4x3fType& mtx2 =
        user->getLayoutKeeper()->getLayout()->GetPane()->GetMtx();

    sead::Vector3f pos;
    pos.x = (size.x * mtx.m[0][0] + size.y * mtx.m[0][1] + mtx.m[0][2] * 0.0f) * 0.5f;
    pos.y = (size.x * mtx.m[1][0] + size.y * mtx.m[1][1] + mtx.m[1][2] * 0.0f) * 0.5f;
    pos.z = (size.x * mtx2.m[2][0] + size.y * mtx2.m[2][1] + mtx2.m[2][2] * 0.0f) * 0.5f;

    setPaneLocalTrans(user, "CursorTL", pos);
    setPaneLocalTrans(user, "CursorTR", pos);
    setPaneLocalTrans(user, "CursorBL", pos);
    setPaneLocalTrans(user, "CursorBR", pos);
}

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

#pragma once

#include <basis/seadTypes.h>
#include <math/seadVector.h>

namespace eui {
class Animator;
}

namespace nn::font {
class Font;

template <typename T>
class TagProcessorBase;
}  // namespace nn::font

namespace nn::ui2d {
class Layout;
class Pane;
class TextBox;
class TextureInfo;
}  // namespace nn::ui2d

namespace al {
class IUseMessageSystem;
class MessageHolder;
class MessageSystem;
class ReplaceTagProcessorBase;

class LayoutPaneGroup {
public:
    LayoutPaneGroup(const char* groupName);
    void startAnim(const char* animName);
    eui::Animator* getAnimator(const char* animName) const;
    void setAnimFrame(f32 frame);
    void setAnimFrameRate(f32 frameRate);
    f32 getAnimFrame() const;
    f32 getAnimFrameMax() const;
    f32 getAnimFrameMax(const char* animName) const;
    f32 getAnimFrameRate() const;
    bool isAnimExist(const char* animName) const;
    eui::Animator* tryGetAnimator(const char* animName) const;
    bool isAnimEnd() const;
    bool isAnimOneTime() const;
    bool isAnimOneTime(const char* animName) const;
    bool isAnimPlaying() const;
    const char* getPlayingAnimName() const;
    void pushAnimName(const char* animName);
    void createAnimator(nn::ui2d::Layout* layout);
    void animate(bool);

private:
    void* filler[5];
};

void setTextBoxString(nn::ui2d::TextBox*, const char16*, u16);
void setTextBoxStringLength(nn::ui2d::TextBox*, const char16*, u16, u16);
void setTextBoxTagProcessor(nn::ui2d::TextBox*, nn::font::TagProcessorBase<u16>*);
void initTextBoxPane(nn::ui2d::TextBox*, const MessageHolder*, const char*, u32);
void reallocateTextBoxStringBuffer(nn::ui2d::TextBox*, u32);
void initTextBoxRecursive(nn::ui2d::Pane*, const MessageHolder*, const char*, u32);
void initTextBoxRecursiveWithSelfTextId(nn::ui2d::Pane*, u32, const MessageSystem*,
                                        const nn::ui2d::Layout*, const nn::ui2d::Layout*);
void replaceTextBoxMessage(nn::ui2d::TextBox*, const MessageHolder*, const char*);
void replaceTextBoxMessage(nn::ui2d::TextBox*, const char16*, const ReplaceTagProcessorBase*,
                           const IUseMessageSystem*);
void setTextBoxNumberTagArgRecursive(nn::ui2d::Pane*, int, int);
void calcPaneRectPos(sead::Vector2f*, sead::Vector2f*, const nn::ui2d::Pane*);
void getPaneName(const nn::ui2d::Pane*);
void showPaneRecursive(nn::ui2d::Pane*);
void hidePaneRecursive(nn::ui2d::Pane*);
void setTextBoxPaneFont(nn::ui2d::Pane*, const nn::font::Font*);
void requestCaptureRecursive(nn::ui2d::Pane*);
}  // namespace al

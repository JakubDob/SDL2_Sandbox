#pragma once
#include "../Types.h"
#include "../opengl/Shader.h"
#include "../ecs/systems/Renderer.h"
#include "../DrawableText.h"
#include "../TextureAtlasFrame.h"
#include "../InputTarget.h"
#include <iostream>
#include <functional>
#include <vector>
#include <string>

namespace Gui {
class GuiElement : public InputTarget {
   public:
    GuiElement();
    void render(Renderer& renderer);
    void recalculate();
    inline void update() {
        if (shouldRecalculate) {
            recalculate();
        }
    }
    inline Vec4 const& getRenderQuad() const { return renderQuad; }
    inline Vec4 const& getClickableArea() {
        if (!visible) return zeroQuad;
        if (shouldRecalculate) recalculate();
        return renderQuad;
    }
    inline void setPosition(int x, int y) {
        this->x = x;
        this->y = y;
        if (onSetPositionCb) onSetPositionCb(x, y);
        shouldRecalculate = true;
    }
    inline void setTextAlign(TextAlignment t) {
        this->textAlign = t;
        shouldRecalculate = true;
    }
    inline void setContentWidth(int w) {
        this->model.contentWidth = w;
        shouldRecalculate = true;
    }
    inline void setContentHeight(int h) {
        this->model.contentHeight = h;
        shouldRecalculate = true;
    }
    inline void setPaddingLeft(int p) {
        this->model.paddingLeft = p;
        shouldRecalculate = true;
    }
    inline void setPaddingRight(int p) {
        this->model.paddingRight = p;
        shouldRecalculate = true;
    }
    inline void setPaddingTop(int p) {
        this->model.paddingTop = p;
        shouldRecalculate = true;
    }
    inline void setPaddingBottom(int p) {
        this->model.paddingBottom = p;
        shouldRecalculate = true;
    }
    inline void setMarginLeft(int m) {
        this->model.marginLeft = m;
        shouldRecalculate = true;
    }
    inline void setMarginRight(int m) {
        this->model.marginRight = m;
        shouldRecalculate = true;
    }
    inline void setMarginTop(int m) {
        this->model.marginTop = m;
        shouldRecalculate = true;
    }
    inline void setMarginBottom(int m) {
        this->model.marginBottom = m;
        shouldRecalculate = true;
    }
    inline void setBackgroundColor(Vec4 c) {
        bgColor = c;
        shouldRecalculate = true;
    }
    inline Vec4 getBackgroundColor() const { return bgColor; }
    inline int getPaddedWidth() {
        if (shouldRecalculate) recalculate();
        return renderQuad[2];
    }
    inline int getPaddedHeight() {
        if (shouldRecalculate) recalculate();
        return renderQuad[3];
    }
    inline int getTotalWidth() {
        if (shouldRecalculate) recalculate();
        return width;
    }
    inline int getTotalHeight() {
        if (shouldRecalculate) recalculate();
        return height;
    }
    inline int getContentWidth() const { return model.contentWidth; }
    inline float getContentWidthF() const {
        return static_cast<float>(model.contentWidth);
    }
    inline int getContentHeight() const { return model.contentHeight; }
    inline int getContentHeightF() const {
        return static_cast<float>(model.contentHeight);
    }
    inline int getContentX() const {
        return x + model.marginLeft + model.paddingLeft;
    }
    inline int getContentY() const {
        return y + model.marginTop + model.paddingTop;
    }
    inline int getPaddingX() const { return x + model.marginLeft; }
    inline int getPaddingY() const { return y + model.marginTop; }
    inline int getPaddingLeft() const { return model.paddingLeft; }
    inline int getPaddingRight() const { return model.paddingRight; }
    inline int getPaddingTop() const { return model.paddingTop; }
    inline int getPaddingBottom() const { return model.paddingBottom; }
    inline void setDrawableText(DrawableText const& drawable) {
        drawableText = &drawable;
    }
    inline void setResizableWidth(bool value) { resizableWidth = value; }
    inline void setResizableHeight(bool value) { resizableHeight = value; }
    inline void setVisible(bool value) { visible = value; }
    inline int getX() const { return x; }
    inline int getY() const { return y; }
    inline void addExtraRenderComponent(ColoredRect cr) {
        extraRenderComponents.emplace_back(cr);
    }
    inline void onSetPosition(std::function<void(int x, int y)> cb) {
        onSetPositionCb = cb;
    }
    void resize(BoxCorner corner, int dw, int dh);
    inline GuiElement* upcast() { return this; }
    inline auto const& getExtraRenderComponents() const {
        return extraRenderComponents;
    }
    inline void setTextContent(std::string const& text) { this->text = text; }
    RenderCommand& getRenderCommand() { return renderCommand; };
    inline void setRenderCommand(RenderCommand const& rc) {
        renderCommand = rc;
    }
    void setTexture(Texture const& texture);
    void setTexture(Texture const& texture, TextureAtlasFrame const& frame);
    inline std::string const& getTextContent() const { return text; }
    virtual ~GuiElement() {}

    virtual bool currentlyInteracting(int x, int y) override;
    virtual bool isVisible() const override;

   protected:
    virtual bool setFocus(bool value) override;

   private:
    // x,y including margins
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    bool resizableWidth = true;
    bool resizableHeight = true;
    bool shouldRecalculate = true;
    bool visible = true;
    bool usesAtlasFrame = false;
    bool m_currentlyInteracting = false;
    Vec4 renderQuad;
    Vec4 zeroQuad{};
    Vec2 textPos{};
    std::string text;
    DrawableText const* drawableText = nullptr;
    BoxModel model;
    Vec4 bgColor = {1.f, 0.f, 1.f, 1.f};
    TextAlignment textAlign = TextAlignment::CENTER;
    std::vector<ColoredRect> extraRenderComponents;
    // handle positioning of additional elements like scrollbar thumb
    std::function<void(int, int)> onSetPositionCb = nullptr;
    RenderCommand renderCommand;
    Texture const* texture = nullptr;
    TextureAtlasFrame const* atlasFrame = nullptr;
};
}  // namespace Gui

#pragma once
#include "GuiTitlebar.h"
#include "GuiContainer.h"
#include "GuiScrollbar.h"
#include "GuiWindowBorder.h"
#include "../InputTarget.h"
#include "../AssetsManager.h"
#include "../Types.h"
#include "../opengl/Shader.h"
#include "../ecs/systems/Renderer.h"
#include "../DrawableText.h"
#include <vector>

namespace Gui {
class GuiWindow : public InputTarget {
   public:
    GuiWindow(int x, int y, int w, int h, std::string const& windowName,
              DrawableText const& drawableText);
    virtual ~GuiWindow(){};
    void render(Renderer& renderer, Shader const& shader, float viewportHeight);
    void setPosition(int x, int y);
    void resize(BoxCorner corner, int dw, int dh);
    inline void setTitle(std::string const& title,
                         TextAlignment align = TextAlignment::LEFT) {
        titlebar.setTitle(std::move(title), align);
    }
    void setElementIndex(GuiElement* element, unsigned index);
    void setElementIndex(unsigned indexA, unsigned indexB);
    inline int getX() const { return border.getX(); }
    inline int getY() const { return border.getY(); }
    inline int getWidth() { return border.getTotalWidth(); }
    inline int getHeight() { return border.getTotalHeight(); }
    inline void setResizable(bool value) { resizable = value; }
    inline void setDraggable(bool value) { draggable = value; }
    inline void setScrollable(bool value) { scrollable = value; }
    inline int getContainerWidth() const { return container.getWidth(); }
    inline int getContainerHeight() const { return container.getHeight(); }
    inline int getContentWidth() const { return contentQuad[2]; }
    inline int getContentHeight() const { return contentQuad[3]; }
    inline void scrollHorizontally(int dx) {
        container.setX(contentQuad[0] - dx);
    }
    inline void scrollVertically(int dy) {
        container.setY(contentQuad[1] - dy);
    }
    inline void scroll(int dx, int dy) {
        container.setPosition(contentQuad[0] - dx, contentQuad[1] - dy);
    }
    inline void setVisible(bool visible) { this->visible = visible; };
    inline std::vector<GuiElement*> const& getWindowElements() const {
        return windowElements;
    }
    inline GuiContainer const& getContainer() const { return container; }
    void setContainer(GuiContainer&& c);
    void addElement(GuiElement* element);
    void setLayout(ElementLayout layout);
    GeometryRect getScissorRect(float viewportHeight) const;
    inline virtual bool isVisible() const override final { return visible; }
    // TODO: currently border functions as a background rectangle, split the
    // shape into proper border with rounded edges(triangle fan) and a
    // background rect that requires extending renderer to handle triangle
    // meshes with a separate vao and a render command to select primitive type
    inline void setBackgroundColor(Vec4 const& color) {
        this->border.setBackgroundColor(color);
    }
    inline void setBackgroundTexture(Texture const& texture) {
        this->border.setTexture(texture);
    }
    inline void setBackgroundTexture(Texture const& texture,
                                     TextureAtlasFrame const& frame) {
        this->border.setTexture(texture, frame);
    }
    inline void setTitlebarAlignment(TextAlignment align) {
        titlebar.setTextAlign(align);
    }

   protected:
    void broadcastToWindowElements(UiEvent const& e) const;
    virtual bool currentlyInteracting(int x, int y) override final;
    inline virtual bool setFocus(bool value) override final {
        hasFocus = value;
        return true;
    }
    virtual bool handleEvent(Gui::UiEvent const& event) override final;

   private:
    bool resizable = true;
    bool draggable = true;
    bool scrollable = true;
    bool visible = true;
    bool hasFocus = false;
    int id = 0;
    // defines what's rendered on the screen
    Vec4 renderQuad;
    // defines the size of the content area
    Vec4 contentQuad;
    GuiTitlebar titlebar;
    GuiContainer container;
    GuiScrollbar horizontalScrollbar;
    GuiScrollbar verticalScrollbar;
    GuiWindowBorder border;
    GuiElement* windowElementFocus = nullptr;
    std::vector<GuiElement*> windowElements;
};
}  // namespace Gui
#pragma once
#include "GuiElement.h"
#include "../Types.h"
#include "../opengl/Shader.h"
#include "../ecs/systems/Renderer.h"
#include <vector>
#include <memory>
namespace Gui {
class GuiContainer {
   public:
    GuiContainer& operator=(GuiContainer&& o) noexcept;
    inline void add(GuiElement* e) {
        elements.push_back(std::unique_ptr<GuiElement>(e));
        recalculate();
    }
    inline void setLayout(ElementLayout layout) {
        this->layout = layout;
        recalculate();
    }
    inline void setPosition(int x, int y) {
        this->x = x;
        this->y = y;
        recalculate();
    }
    inline void setX(int x) {
        this->x = x;
        recalculate();
    }
    inline void setY(int y) {
        this->y = y;
        recalculate();
    }
    inline void matchBiggestSibling(bool value) { matchBiggest = value; }
    void setElementIndex(GuiElement* element, unsigned index);
    void setElementIndex(unsigned indexA, unsigned indexB);
    inline int getWidth() const { return width; }
    inline int getHeight() const { return height; }
    inline int getX() const { return x; }
    inline int getY() const { return y; }
    inline auto const& getElements() const { return elements; }
    void render(Renderer& renderer, Shader const& shader,
                GeometryRect const& scissor, bool hasFocus = false) const;
    void recalculate();
    void broadcastEventToAll(UiEvent const& e);
    bool broadcastEventToTargets(int pointerX, int pointerY, UiEvent const& e);

   private:
    void matchElementWidth();
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    bool matchBiggest = true;
    ElementLayout layout = ElementLayout::VERTICAL;
    std::vector<std::unique_ptr<GuiElement>> elements;
};
}  // namespace Gui

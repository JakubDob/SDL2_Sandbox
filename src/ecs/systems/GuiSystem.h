#pragma once
#include "../../gui/GuiElement.h"
#include "../../gui/GuiWindow.h"
#include "Renderer.h"
#include <vector>

namespace Gui {
class GuiSystem {
   public:
    void add(GuiWindow& window);
    void add(GuiElement& guiElement);
    void render(Renderer& renderer, Shader const& shader, float viewportHeight);

   private:
    std::vector<GuiElement*> guiElements;
    std::vector<GuiWindow*> guiWindows;
};
}  // namespace Gui

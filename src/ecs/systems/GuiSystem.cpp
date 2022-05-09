#include "GuiSystem.h"

namespace Gui {

void GuiSystem::add(GuiWindow& window) { guiWindows.push_back(&window); }

void GuiSystem::add(GuiElement& guiElement) {
    guiElements.push_back(&guiElement);
}

void GuiSystem::render(Renderer& renderer, Shader const& shader,
                       float viewportHeight) {
    for (auto* window : guiWindows) {
        window->render(renderer, shader, viewportHeight);
    }
    for (auto* element : guiElements) {
        element->getRenderCommand().shader = &shader;
        element->render(renderer);
    }
}
}  // namespace Gui

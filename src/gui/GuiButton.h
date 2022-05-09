#pragma once
#include "GuiElement.h"
#include "../AssetsManager.h"
#include <functional>

namespace Gui {
class GuiButton : public GuiElement {
   public:
    GuiButton(std::string const& label, DrawableText const& drawableText,
              std::function<void()> onClick);
    GuiButton(std::string const& label, DrawableText const& drawableText);
    void setOnClick(std::function<void()> fn);

   private:
    virtual bool handleEvent(UiEvent const& e) override final;
    std::function<void()> onClick;
    // perform action only when pressed and released inside the button area
    bool pointerDown = false;
    Vec4 color = {0.5f, 0.5f, 0.5f, 1.f};
};
}  // namespace Gui

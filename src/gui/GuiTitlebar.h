#pragma once
#include "GuiElement.h"

namespace Gui {
class GuiWindow;
class GuiTitlebar : public GuiElement {
   public:
    GuiTitlebar(std::string const& title, DrawableText const& drawableText,
                int width, GuiWindow* parentWindow,
                Gui::TextAlignment align = Gui::TextAlignment::LEFT);
    void setTitle(std::string const& title,
                  Gui::TextAlignment align = Gui::TextAlignment::LEFT);

   private:
    virtual bool handleEvent(UiEvent const& e) override;
    GuiWindow* parentWindow;
};
}  // namespace Gui
#pragma once
#include "GuiElement.h"
#include "GuiConstants.h"

namespace Gui {
class GuiWindow;

class GuiWindowBorder : public GuiElement {
   public:
    GuiWindowBorder(GuiWindow* window, int width, int height,
                    int minWidth = GuiConstant::WindowBorder::minWidth,
                    int minHeight = GuiConstant::WindowBorder::minHeight);
    void resizeWindow(BoxCorner corner, int dw, int dh);
    bool pointerNearCorner(int x, int y, BoxCorner* result = nullptr);
    virtual bool handleEvent(UiEvent const& e) override;

   private:
    GuiWindow* window;
    BoxCorner focusedCorner = BoxCorner::NONE;
    bool focused = false;
    int minWidth;
    int minHeight;
};
}  // namespace Gui

#include "GuiWindowBorder.h"
#include "GuiWindow.h"
#include "BoxResizer.h"

namespace Gui {
GuiWindowBorder::GuiWindowBorder(GuiWindow* window, int width, int height,
                                 int minWidth, int minHeight)
    : window(window), minWidth(minWidth), minHeight(minHeight) {
    setContentWidth(width);
    setContentHeight(height);
    setPaddingBottom(GuiConstant::WindowBorder::padding);
    setPaddingTop(GuiConstant::WindowBorder::padding);
    setPaddingLeft(GuiConstant::WindowBorder::padding);
    setPaddingRight(GuiConstant::WindowBorder::padding);
}
bool GuiWindowBorder::handleEvent(UiEvent const& e) {
    if (e.type == UiEventType::DRAG) {
        if (focused || pointerNearCorner(e.move.x, e.move.y, &focusedCorner)) {
            focused = true;
            resizeWindow(focusedCorner, e.move.dx, e.move.dy);
        }
    } else if (e.type == UiEventType::POINTER_UP) {
        focused = false;
    }
    return true;
}
void GuiWindowBorder::resizeWindow(BoxCorner corner, int dw, int dh) {
    if (getTotalWidth() < minWidth &&
        ((corner == BoxCorner::BOTTOM_RIGHT && dw < 0) ||
         (corner == BoxCorner::BOTTOM_LEFT && dw > 0))) {
        dw = 0;
    }
    if (getTotalHeight() < minHeight && dh < 0) {
        dh = 0;
    }
    window->resize(corner, dw, dh);
}

bool GuiWindowBorder::pointerNearCorner(int x, int y, BoxCorner* result) {
    if (x > getPaddingX() && x < getContentX()) {
        if (y > getPaddingY() && y < getContentY()) {
            if (result) *result = BoxCorner::TOP_LEFT;
            return true;
        }
        if (y > getContentY() + getContentHeight() &&
            y < getY() + getTotalHeight()) {
            if (result) *result = BoxCorner::BOTTOM_LEFT;
            return true;
        }
    }
    if (x > getContentX() + getContentWidth() && x < getX() + getTotalWidth()) {
        if (y > getPaddingY() && y < getContentY()) {
            if (result) *result = BoxCorner::TOP_RIGHT;
            return true;
        }
        if (y > getContentY() + getContentHeight() &&
            y < getY() + getTotalHeight()) {
            if (result) *result = BoxCorner::BOTTOM_RIGHT;
            return true;
        }
    }
    return false;
}
}  // namespace Gui
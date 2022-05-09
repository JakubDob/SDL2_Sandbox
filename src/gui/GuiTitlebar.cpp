#include "GuiTitlebar.h"
#include "GuiWindow.h"
#include "GuiConstants.h"

namespace Gui {
GuiTitlebar::GuiTitlebar(std::string const& title,
                         DrawableText const& drawableText, int width,
                         GuiWindow* parentWindow, Gui::TextAlignment align)
    : parentWindow(parentWindow) {
    setTitle(title, align);
    setContentHeight(GuiConstant::Titlebar::height);
    setContentWidth(width);
    setDrawableText(drawableText);
}

void GuiTitlebar::setTitle(std::string const& title, Gui::TextAlignment align) {
    setTextContent(title);
    setTextAlign(align);
    setResizableHeight(false);
}

bool GuiTitlebar::handleEvent(UiEvent const& e) {
    if (e.type == UiEventType::DRAG) {
        parentWindow->setPosition(parentWindow->getX() + e.move.dx,
                                  parentWindow->getY() + e.move.dy);
    }
    return true;
}
}  // namespace Gui
#include "GuiButton.h"
#include "../Utils.h"
#include "GuiConstants.h"
#include "../../DrawableText.h"

namespace Gui {
GuiButton::GuiButton(std::string const& label, DrawableText const& drawableText,
                     std::function<void()> onClick)
    : onClick(onClick) {
    auto textSize = drawableText.getTextSize(label);
    int w = std::max(static_cast<int>(textSize[0]), GuiConstant::Button::width);
    int h =
        std::max(static_cast<int>(textSize[1]), GuiConstant::Button::height);
    setContentWidth(w);
    setContentHeight(h);
    this->setDrawableText(drawableText);
    this->setTextContent(label);
    setBackgroundColor({0.f, 0.3f, 0.6f, 1.f});
    this->color = getBackgroundColor();
}

GuiButton::GuiButton(std::string const& label, DrawableText const& drawableText)
    : GuiButton(label, drawableText, nullptr) {}

bool GuiButton::handleEvent(UiEvent const& e) {
    if (e.type == UiEventType::POINTER_DOWN) {
        pointerDown = true;
        auto bgc = color;
        bgc[0] /= 2;
        bgc[1] /= 2;
        bgc[2] /= 2;
        setBackgroundColor(bgc);
    } else if (e.type == UiEventType::POINTER_UP) {
        if (pointerDown) {
            if (utils::pointInsideRect(e.press.x, e.press.y,
                                       getClickableArea())) {
                onClick();
            }
            pointerDown = false;
            setBackgroundColor(color);
        }
    }
    return true;
}

void GuiButton::setOnClick(std::function<void()> fn) { onClick = fn; }
}  // namespace Gui
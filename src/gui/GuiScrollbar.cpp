#include "GuiScrollbar.h"
#include "GuiWindow.h"
#include "../Utils.h"

namespace Gui {
GuiScrollbar::GuiScrollbar(Orientation o, int windowWidth, int windowHeight,
                           GuiWindow* window, GuiScrollbar* other)
    : window(window),
      other(other),
      o(o),
      windowWidth(windowWidth),
      windowHeight(windowHeight) {
    auto& thumbW = thumb[2];
    auto& thumbH = thumb[3];

    if (o == Orientation::VERTICAL) {
        setContentWidth(GuiConstant::Scrollbar::minThumbCrossAxisLen);
        setContentHeight(windowHeight);
        setResizableWidth(false);
        thumbW = crossAxisLen;
        thumbH = scrollAxisLen;
        deadZoneOwner = true;
        addExtraRenderComponent(ColoredRect({1.f, 0.f, 0.f, 1.f}, deadZone));
    } else {
        setContentWidth(windowWidth);
        setContentHeight(GuiConstant::Scrollbar::minThumbCrossAxisLen);
        setResizableHeight(false);
        thumbW = scrollAxisLen;
        thumbH = crossAxisLen;
    }

    setBackgroundColor({0.f, 1.f, 0.f, 1.f});
    addExtraRenderComponent(ColoredRect({1.f, 1.f, 0.f, 1.f}, thumb));
    onSetPosition([&](int x, int y) {
        setThumbPosition();
        deadZone[0] = getX();
        deadZone[1] = getY() + getContentHeight();
    });
}

Vec4 const& GuiScrollbar::getDeadZone() const {
    if (deadZoneOwner) {
        return deadZone;
    } else {
        return other->deadZone;
    }
}

bool GuiScrollbar::handleEvent(UiEvent const& e) {
    auto& thumbX = thumb[0];
    auto& thumbY = thumb[1];

    if (e.type == UiEventType::CONTAINER_SIZE_CHANGE) {
        containerHeight = e.size.h;
        containerWidth = e.size.w;
        setThumbPosition();
    } else if (e.type == UiEventType::WINDOW_SIZE_CHANGE) {
        windowWidth = e.size.w;
        windowHeight = e.size.h;
        setThumbPosition();
    }

    else if (e.type == UiEventType::DRAG) {
        if (o == Orientation::VERTICAL) {
            int maxY = getY() + getContentHeight() - thumb[3];
            int minY = getY();
            thumbOffset += e.move.dy;
            thumbY = getY() + thumbOffset;
            if (thumbY > maxY) {
                thumbY = maxY;
                thumbOffset = getContentHeight() - thumb[3];
            } else if (thumbY < minY) {
                thumbY = minY;
                thumbOffset = 0;
            }
            scrollOffset = (containerHeight - getContentHeightF()) /
                           (getContentHeightF() - scrollAxisLen) * thumbOffset;
            window->scrollVertically(static_cast<int>(scrollOffset));
        } else {
            int maxX = getX() + getContentWidthF() - thumb[2];
            int minX = getX();
            thumbOffset += e.move.dx;
            thumbX = getX() + thumbOffset;
            if (thumbX > maxX) {
                thumbX = maxX;
                thumbOffset = getContentWidthF() - thumb[2];
            } else if (thumbX < minX) {
                thumbX = minX;
                thumbOffset = 0;
            }
            scrollOffset = (containerWidth - getContentWidthF()) /
                           (getContentWidthF() - scrollAxisLen) * thumbOffset;
            window->scrollHorizontally(static_cast<int>(scrollOffset));
        }
    }
    return true;
}

void GuiScrollbar::adjustContentLength() {
    if (o == Orientation::VERTICAL) {
        other->setVisible(containerWidth > other->getContentWidthF());
        if (other->isVisible()) {
            setContentHeight(windowHeight - other->getTotalHeight());
        } else {
            setContentHeight(windowHeight);
        }
    } else {
        other->setVisible(containerHeight > other->getContentHeightF());
        if (other->isVisible()) {
            setContentWidth(windowWidth - other->getTotalWidth());
        } else {
            setContentWidth(windowWidth);
        }
    }
}
void GuiScrollbar::setThumbPosition() {
    auto& thumbX = thumb[0];
    auto& thumbY = thumb[1];
    auto& thumbW = thumb[2];
    auto& thumbH = thumb[3];
    adjustContentLength();
    if (o == Orientation::VERTICAL) {
        float Hf = getContentHeightF();
        //||thumbOffset > 0 allows for scrolling back when resizing the window
        //very quickly
        if (isVisible() || thumbOffset > 0) {
            thumbX = getX();
            scrollAxisLen = (Hf * Hf) / containerHeight;
            if (scrollAxisLen < GuiConstant::Scrollbar::minThumbScrollAxisLen) {
                scrollAxisLen = GuiConstant::Scrollbar::minThumbScrollAxisLen;
            }
            thumbH = scrollAxisLen;
            // scroll up when the thumb is at the bottom of the window
            if (thumbY + thumbH > getY() + getContentHeight()) {
                thumbOffset = getContentHeight() - thumbH;
                if (thumbOffset < 0) thumbOffset = 0;
                if (Hf - scrollAxisLen != 0) {
                    scrollOffset = (containerHeight - Hf) /
                                   (Hf - scrollAxisLen) * thumbOffset;
                    window->scrollVertically(scrollOffset);
                }

            } else {
                // scroll offset should be an invariant when resizing to avoid
                // jumps, so adjust the thumb offset alone
                thumbOffset =
                    static_cast<int>(scrollOffset / ((containerHeight - Hf) /
                                                     (Hf - scrollAxisLen)));
                if (thumbOffset < 0) thumbOffset = 0;
            }
            // setting thumb.y prevents jittering
            thumbY = getY() + thumbOffset;
        }
    } else {
        float Wf = getContentWidthF();
        if (isVisible() || thumbOffset > 0) {
            thumbY = getY();
            scrollAxisLen = (Wf * Wf) / containerWidth;
            if (scrollAxisLen < GuiConstant::Scrollbar::minThumbScrollAxisLen) {
                scrollAxisLen = GuiConstant::Scrollbar::minThumbScrollAxisLen;
            }
            thumbW = scrollAxisLen;
            if (thumbX + thumbW > getX() + getContentWidth()) {
                thumbOffset = getContentWidth() - thumbW;
                if (thumbOffset < 0) thumbOffset = 0;
                if (Wf - scrollAxisLen != 0) {
                    scrollOffset = (containerWidth - Wf) /
                                   (Wf - scrollAxisLen) *
                                   static_cast<float>(thumbOffset);
                    window->scrollHorizontally(scrollOffset);
                }
            } else {
                thumbOffset =
                    static_cast<int>(scrollOffset / ((containerWidth - Wf) /
                                                     (Wf - scrollAxisLen)));
                if (thumbOffset < 0) thumbOffset = 0;
            }
            thumbX = getX() + thumbOffset;
        }
    }
    if (isVisible() && other->isVisible()) {
        int h = 0;
        deadZoneOwner ? h = other->getContentHeight() : getContentHeight();
        setDeadZone(getX(), getY() + getContentHeight(), getContentWidth(), h);
    } else {
        unsetDeadZone();
    }
}
}  // namespace Gui

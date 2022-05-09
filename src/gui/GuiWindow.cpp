#include "GuiWindow.h"
#include "BoxResizer.h"
#include "../Utils.h"
#include "GuiConstants.h"

namespace Gui {
GuiWindow::GuiWindow(int x, int y, int w, int h, std::string const& windowName,
                     DrawableText const& drawableText)
    : titlebar(windowName, drawableText, w, this),
      horizontalScrollbar(Orientation::HORIZONTAL, w,
                          h - GuiConstant::Titlebar::height, this,
                          &verticalScrollbar),
      verticalScrollbar(Orientation::VERTICAL, w,
                        h - GuiConstant::Titlebar::height, this,
                        &horizontalScrollbar),
      border(this, w, h) {
    renderQuad[2] = w;
    renderQuad[3] = h;
    contentQuad[2] = w;
    contentQuad[3] = h - GuiConstant::Titlebar::height;
    border.setBackgroundColor(Vec4({.078, .196, .0207, 1}));
    setPosition(x, y);
    // 100 -> random big number (render on top of everything else)
    border.getRenderCommand().layer = 0;
    horizontalScrollbar.getRenderCommand().layer = 100;
    verticalScrollbar.getRenderCommand().layer = 100;
    titlebar.getRenderCommand().layer = 100;
    windowElements.emplace_back(horizontalScrollbar.upcast());
    windowElements.emplace_back(verticalScrollbar.upcast());
    windowElements.emplace_back(border.upcast());
    windowElements.emplace_back(titlebar.upcast());

    static int instanceId = 0;
    this->id = ++instanceId;

    for (auto& e : windowElements) {
        e->getRenderCommand().id = id;
        e->getRenderCommand().windowElement = true;
    }
}

GeometryRect GuiWindow::getScissorRect(float viewportHeight) const {
    return Vec4(renderQuad[0], viewportHeight - renderQuad[1] - renderQuad[3],
                renderQuad[2], renderQuad[3]);
}
void GuiWindow::render(Renderer& renderer, Shader const& shader,
                       float viewportHeight) {
    if (!visible) {
        return;
    }
    auto const scissor = getScissorRect(viewportHeight);
    for (auto& element : windowElements) {
        auto& rc = element->getRenderCommand();
        rc.state.scissorState.set(scissor);
        rc.shader = &shader;
        rc.hud = true;
        rc.focusLayer = hasFocus;
    }
    border.getRenderCommand().state.scissorState.enabled = false;
    for (auto& element : windowElements) {
        element->render(renderer);
    }

    container.render(renderer, shader, scissor, hasFocus);
}

void GuiWindow::setPosition(int x, int y) {
    auto& contentQuadX = contentQuad[0];
    auto& contentQuadY = contentQuad[1];
    auto& contentQuadW = contentQuad[2];
    auto& contentQuadH = contentQuad[3];
    border.setPosition(x, y);
    renderQuad[0] = x + border.getPaddingLeft();
    renderQuad[1] = y + border.getPaddingTop();
    contentQuadX = renderQuad[0];
    contentQuadY = renderQuad[1] + titlebar.getTotalHeight();
    scroll(horizontalScrollbar.getScrollOffset(),
           verticalScrollbar.getScrollOffset());
    titlebar.setPosition(renderQuad[0], renderQuad[1]);
    verticalScrollbar.setPosition(
        contentQuadX + contentQuadW - horizontalScrollbar.getTotalHeight(),
        contentQuadY);
    horizontalScrollbar.setPosition(
        contentQuadX,
        contentQuadY + contentQuadH - verticalScrollbar.getTotalWidth());
}

void GuiWindow::setElementIndex(GuiElement* element, unsigned index) {
    container.setElementIndex(element, index);
}

void GuiWindow::setElementIndex(unsigned indexA, unsigned indexB) {
    container.setElementIndex(indexA, indexB);
}

bool GuiWindow::currentlyInteracting(int x, int y) {
    return windowElementFocus || border.currentlyInteracting(x, y);
}

bool GuiWindow::handleEvent(UiEvent const& e) {
    switch (e.type) {
        case UiEventType::POINTER_UP:
            windowElementFocus = nullptr;
            container.broadcastEventToAll(e);
            border.handleEvent(e);
            break;

        case UiEventType::POINTER_DOWN:
            if (windowElementFocus) {
                windowElementFocus->handleEvent(e);
                return true;
            }
            if (scrollable) {
                if (utils::pointInsideRect(e.press.x, e.press.y,
                                           horizontalScrollbar.getDeadZone())) {
                    return true;
                } else if (utils::pointInsideRect(
                               e.press.x, e.press.y,
                               horizontalScrollbar.getClickableArea())) {
                    windowElementFocus = horizontalScrollbar.upcast();
                    return true;
                } else if (utils::pointInsideRect(
                               e.press.x, e.press.y,
                               verticalScrollbar.getClickableArea())) {
                    windowElementFocus = verticalScrollbar.upcast();
                    return true;
                }
            }
            if (draggable) {
                if (utils::pointInsideRect(e.press.x, e.press.y,
                                           titlebar.getClickableArea())) {
                    windowElementFocus = titlebar.upcast();
                    return true;
                }
            }

            if (border.pointerNearCorner(e.press.x, e.press.y)) {
                windowElementFocus = border.upcast();
                return true;
            }
            if (utils::pointInsideRect(e.press.x, e.press.y, contentQuad)) {
                container.broadcastEventToTargets(e.press.x, e.press.y, e);
            }
            return border.currentlyInteracting(e.press.x, e.press.y);

        case UiEventType::DRAG:
            if (windowElementFocus) {
                windowElementFocus->handleEvent(e);
                return true;
            }
            return container.broadcastEventToTargets(e.move.x, e.move.y, e);
        default:
            return false;
    }
    return true;
}

void GuiWindow::broadcastToWindowElements(UiEvent const& e) const {
    for (auto* el : windowElements) {
        el->handleEvent(e);
    }
}

void GuiWindow::resize(BoxCorner corner, int dw, int dh) {
    if (!resizable) {
        return;
    }
    BoxResizer::resize(renderQuad[0], renderQuad[1], renderQuad[2],
                       renderQuad[3], corner, dw, dh);
    BoxResizer::resize(contentQuad[0], contentQuad[1], contentQuad[2],
                       contentQuad[3], corner, dw, dh);
    titlebar.resize(corner, dw, dh);
    verticalScrollbar.resize(corner, dw, dh);
    horizontalScrollbar.resize(corner, dw, dh);
    border.resize(corner, dw, dh);
    setPosition(border.getX(), border.getY());
    UiEvent e(UiEventType::WINDOW_SIZE_CHANGE);
    e.size.w = contentQuad[2];
    e.size.h = contentQuad[3];
    broadcastToWindowElements(e);
}

void GuiWindow::setContainer(GuiContainer&& c) {
    container = std::move(c);
    UiEvent e(UiEventType::CONTAINER_SIZE_CHANGE);
    e.size.w = container.getWidth();
    e.size.h = container.getHeight();
    broadcastToWindowElements(e);
}

void GuiWindow::addElement(GuiElement* element) {
    element->getRenderCommand().layer = 1;
    element->getRenderCommand().windowElement = true;
    element->getRenderCommand().id = id;
    container.add(element);
    UiEvent e(UiEventType::CONTAINER_SIZE_CHANGE);
    e.size.w = container.getWidth();
    e.size.h = container.getHeight();
    broadcastToWindowElements(e);
}
void GuiWindow::setLayout(ElementLayout layout) {
    container.setLayout(layout);
    UiEvent e(UiEventType::CONTAINER_SIZE_CHANGE);
    e.size.w = container.getWidth();
    e.size.h = container.getHeight();
    broadcastToWindowElements(e);
}
}  // namespace Gui
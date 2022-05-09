#include "GuiContainer.h"
#include "../Utils.h"

namespace Gui {
GuiContainer& GuiContainer::operator=(GuiContainer&& o) noexcept {
    this->elements = std::move(o.elements);
    this->x = o.x;
    this->y = o.y;
    this->width = o.width;
    this->height = o.height;
    this->layout = o.layout;
    return *this;
}

void GuiContainer::setElementIndex(GuiElement* element, unsigned index) {
    auto result = std::find_if(elements.begin(), elements.end(),
                               [&](std::unique_ptr<GuiElement> const& e) {
                                   return e.get() == element;
                               });
    if (result != elements.end()) {
        result->swap(elements[index]);
    }
}

void GuiContainer::setElementIndex(unsigned indexA, unsigned indexB) {
    elements[indexA].swap(elements[indexB]);
}

void GuiContainer::recalculate() {
    this->width = 0;
    this->height = 0;
    if (layout == ElementLayout::HORIZONTAL) {
        int xOffset = 0;
        for (auto const& e : elements) {
            int h = e->getTotalHeight();
            int w = e->getTotalWidth();
            if (h > this->height) {
                this->height = h;
            }
            this->width += w;
            e->setPosition(x + xOffset, y);
            xOffset += e->getTotalWidth();
        }
        if (matchBiggest) {
            for (auto& e : elements) {
                auto totalH = e->getTotalHeight();
                auto contentH = e->getContentHeight();
                e->setContentHeight(this->height + contentH - totalH);
            }
        }
    } else if (layout == ElementLayout::VERTICAL) {
        int yOffset = 0;
        for (auto const& e : elements) {
            int h = e->getTotalHeight();
            int w = e->getTotalWidth();
            if (w > this->width) {
                this->width = w;
            }
            this->height += h;
            e->setPosition(x, y + yOffset);
            yOffset += e->getTotalHeight();
        }
        if (matchBiggest) {
            for (auto& e : elements) {
                auto totalW = e->getTotalWidth();
                auto contentW = e->getContentWidth();
                e->setContentWidth(this->width + contentW - totalW);
            }
        }
    }
}

void GuiContainer::render(Renderer& renderer, Shader const& shader,
                          GeometryRect const& scissor, bool hasFocus) const {
    for (auto& e : elements) {
        auto& rc = e->getRenderCommand();
        rc.state.scissorState.set(scissor);
        rc.focusLayer = hasFocus;
        rc.shader = &shader;
        rc.hud = true;
        e->render(renderer);
    }
}

bool GuiContainer::broadcastEventToTargets(int pointerX, int pointerY,
                                           UiEvent const& event) {
    bool some = false;
    for (auto const& e : elements) {
        auto const& r = e->getClickableArea();
        if (utils::pointInsideRect(pointerX, pointerY, r)) {
            e->handleEvent(event);
            some = true;
        }
    }
    return some;
}

void GuiContainer::broadcastEventToAll(UiEvent const& event) {
    for (auto const& e : elements) {
        e->handleEvent(event);
    }
}
}  // namespace Gui

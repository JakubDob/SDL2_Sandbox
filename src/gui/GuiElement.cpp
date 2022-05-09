#include "GuiElement.h"
#include "BoxResizer.h"
#include "../Utils.h"

namespace Gui {
GuiElement::GuiElement() {
    renderCommand.hud = true;
    renderCommand.layer = 0;
}
void GuiElement::recalculate() {
    this->renderQuad[0] = x + model.marginLeft;
    this->renderQuad[1] = y + model.marginTop;
    this->width = model.contentWidth + model.paddingLeft + model.paddingRight +
                  model.marginLeft + model.marginRight;
    this->height = model.contentHeight + model.paddingTop +
                   model.paddingBottom + model.marginTop + model.marginBottom;
    this->renderQuad[2] =
        model.contentWidth + model.paddingLeft + model.paddingRight;
    this->renderQuad[3] =
        model.contentHeight + model.paddingBottom + model.paddingTop;

    if (drawableText) {
        auto textSize = drawableText->getTextSize(text);
        textPos[1] = renderQuad[1] + model.paddingTop +
                     (model.contentHeight - textSize[1]) / 2;
        if (textAlign == TextAlignment::CENTER) {
            textPos[0] = renderQuad[0] + model.paddingLeft +
                         (model.contentWidth - textSize[0]) / 2;
        } else if (textAlign == TextAlignment::LEFT) {
            textPos[0] = renderQuad[0] + model.paddingLeft;
        } else if (textAlign == TextAlignment::RIGHT) {
            textPos[0] = renderQuad[0] + renderQuad[2] - textSize[0] -
                         model.paddingRight;
        }
    }
    shouldRecalculate = false;
}
// renders with current render command
void GuiElement::render(Renderer& renderer) {
    if (!visible) {
        return;
    }
    if (shouldRecalculate) {
        recalculate();
    }

    if (usesAtlasFrame) {
        auto mesh = atlasFrame->createMesh(TextureAtlasFrame::topLeft,
                                           renderQuad[2], renderQuad[3], false);
        mesh.translate({renderQuad[0], renderQuad[1]});
        mesh.setColor(getBackgroundColor());
        renderCommand.meshId = renderer.addMesh(mesh);
    } else {
        auto mesh = QuadMesh(QuadMesh::topLeft, renderQuad);
        mesh.setColor(getBackgroundColor());
        mesh.resetTexCoords();
        renderCommand.meshId = renderer.addMesh(mesh);
    }

    renderCommand.texture = texture;
    renderer.addRenderCommand(renderCommand);

    for (auto const& cr : extraRenderComponents) {
        RenderCommand extraRc = renderCommand;
        QuadMesh mesh(QuadMesh::topLeft, cr.rect);
        mesh.setColor(cr.color);
        extraRc.texture = nullptr;
        auto extraMeshId = renderer.addMesh(mesh);
        extraRc.meshId = extraMeshId;
        extraRc.layer += 2;
        renderer.addRenderCommand(extraRc);
    }

    if (drawableText) {
        RenderCommand textRc = renderCommand;
        textRc.layer++;
        drawableText->render(text, textPos, &textRc);
    }
}

void GuiElement::resize(BoxCorner corner, int dw, int dh) {
    if (!resizableHeight) {
        dh = 0;
    }
    if (!resizableWidth) {
        dw = 0;
    }
    if (dh == 0 && dw == 0) {
        return;
    }
    BoxResizer::resize(x, y, model.contentWidth, model.contentHeight, corner,
                       dw, dh);
    shouldRecalculate = true;
}

void GuiElement::setTexture(Texture const& texture) {
    this->texture = &texture;
    usesAtlasFrame = false;
}

void GuiElement::setTexture(Texture const& texture,
                            TextureAtlasFrame const& frame) {
    this->texture = &texture;
    this->atlasFrame = &frame;
    usesAtlasFrame = true;
}

bool GuiElement::currentlyInteracting(int x, int y) {
    if (utils::pointInsideRect(x, y, getClickableArea())) {
        m_currentlyInteracting = true;
    } else {
        m_currentlyInteracting = false;
    }
    return m_currentlyInteracting;
}

bool GuiElement::isVisible() const { return visible; }

bool GuiElement::setFocus(bool value) { return false; }

}  // namespace Gui
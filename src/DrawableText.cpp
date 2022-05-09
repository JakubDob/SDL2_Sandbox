#include "DrawableText.h"

DrawableText::DrawableText(FontAtlas const& fontAtlas, Renderer& renderer,
                           Shader const& shader)
    : fontAtlas(fontAtlas), renderer(renderer), shader(shader) {}

void DrawableText::render(std::string const& text, Vec2 const& position,
                          RenderCommand* renderCommand,
                          scalar_t maxLineWidth) const {
    auto const& frames = fontAtlas.getFrames();
    auto const& fontLineSkip = fontAtlas.getFontLineSkip();

    float xOffset = 0, yOffset = 0;
    for (char c : text) {
        if (c == '\n') {
            yOffset += fontLineSkip;
            xOffset = 0;
            continue;
        }
        if (maxLineWidth && xOffset >= maxLineWidth) {
            yOffset += fontLineSkip;
            xOffset = 0;
        }
        auto const& frame = frames[c];
        auto mesh = frame.createMesh(TextureAtlasFrame::topLeft);
        mesh.translate({xOffset + position[0], yOffset + position[1]});
        xOffset += frame.getWidth();
        auto meshId = renderer.addMesh(mesh);

        if (renderCommand) {
            renderCommand->meshId = meshId;
            renderCommand->texture = &fontAtlas.getTexture();
            renderCommand->shader = &shader;
            renderer.addRenderCommand(*renderCommand);
        } else {
            RenderCommand rc(meshId, shader);
            rc.hud = true;
            rc.texture = &fontAtlas.getTexture();
            renderer.addRenderCommand(rc);
        }
    }
}

Vec2 DrawableText::getTextSize(std::string const& text,
                               scalar_t maxLineWidth) const {
    auto const& frames = fontAtlas.getFrames();
    auto const& fontLineSkip = fontAtlas.getFontLineSkip();
    float xOffset = 0, yOffset = 0;
    for (char c : text) {
        if (c == '\n') {
            yOffset += fontLineSkip;
            xOffset = 0;
            continue;
        }
        if (maxLineWidth && xOffset >= maxLineWidth) {
            yOffset += fontLineSkip;
            xOffset = 0;
        }
        auto const& frame = frames[c];
        xOffset += frame.getWidth();
    }
    yOffset += fontLineSkip;
    return {xOffset, yOffset};
}
#pragma once
#include "ecs/systems/Renderer.h"
#include "FontAtlas.h"
#include "opengl/Shader.h"

class DrawableText {
   public:
    DrawableText(FontAtlas const& fontAtlas, Renderer& renderer,
                 Shader const& shader);
    void render(std::string const& text, Vec2 const& position = {0, 0},
                RenderCommand* renderCommand = nullptr,
                scalar_t maxLineWidth = 0) const;
    Vec2 getTextSize(std::string const& text, scalar_t maxLineWidth = 0) const;

   private:
    FontAtlas const& fontAtlas;
    Renderer& renderer;
    Shader const& shader;
};
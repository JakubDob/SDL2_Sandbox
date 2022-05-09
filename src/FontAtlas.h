#pragma once
#include "opengl/Texture.h"
#include "Types.h"
#include "TextureAtlasFrame.h"
#include <memory>
#include <vector>

class FontAtlas {
   public:
    FontAtlas(std::unique_ptr<Texture>&& atlas,
              std::vector<TextureAtlasFrame>&& texRects, scalar_t fontSize);
    Texture const& getTexture() const;
    scalar_t getFontLineSkip() const;
    TextureAtlasFrame const& getFrame(char glyph) const;
    std::vector<TextureAtlasFrame> const& getFrames() const;

   private:
    std::vector<TextureAtlasFrame> texRects;
    std::unique_ptr<Texture> atlas;
    scalar_t fontLineSkip;
};

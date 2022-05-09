#include "FontAtlas.h"

FontAtlas::FontAtlas(std::unique_ptr<Texture>&& atlas,
                     std::vector<TextureAtlasFrame>&& texRects,
                     scalar_t fontLineSkip)
    : atlas(std::move(atlas)),
      texRects(std::move(texRects)),
      fontLineSkip(fontLineSkip) {}

// texture coordinates within the atlas
TextureAtlasFrame const& FontAtlas::getFrame(char glyph) const {
    return texRects[static_cast<int>(glyph)];
}

std::vector<TextureAtlasFrame> const& FontAtlas::getFrames() const {
    return texRects;
}

Texture const& FontAtlas::getTexture() const { return *atlas.get(); }

scalar_t FontAtlas::getFontLineSkip() const { return fontLineSkip; }
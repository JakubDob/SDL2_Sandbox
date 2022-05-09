#include "TextureAtlas.h"

TextureAtlasFrame const& TextureAtlas::getFrame(
    std::string const& frameName) const {
    return frames.at(frameName);
}

void TextureAtlas::addFrame(std::string const& frameName,
                            TextureAtlasFrame const& f) {
    frames[frameName] = f;
}
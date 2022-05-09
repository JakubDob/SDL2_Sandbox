#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "opengl/Texture.h"
#include "Types.h"
#include "TextureAtlasFrame.h"

class TextureAtlas {
   public:
    TextureAtlas(std::unique_ptr<Texture>&& texture, std::string const& name)
        : texture(std::move(texture)), name(name){};
    void addFrame(std::string const& name, TextureAtlasFrame const& f);
    TextureAtlasFrame const& getFrame(std::string const& frameName) const;
    inline Texture const& getTexture() const { return *texture; }
    inline std::string const& getName() const { return name; }

   private:
    std::unordered_map<std::string, TextureAtlasFrame> frames;
    std::unique_ptr<Texture> texture;
    std::string name;
};
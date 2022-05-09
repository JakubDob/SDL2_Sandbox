#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "CleanupHelpers.h"
#include "EngineConstants.h"
#include "opengl/Texture.h"
#include "Types.h"
#include "opengl/Shader.h"
#include "TextureAtlas.h"
#include "FontAtlas.h"
#include "DrawableText.h"

class AssetsManager {
   public:
    AssetsManager();
    // load functions expect trailing slash in x_FolderPath/
    void loadTexture(std::string const& textureFolderPath,
                     std::string const& textureNameWithExt);
    void loadFont(std::string const& fontFolderPath,
                  std::string const& fontNameWithExt);
    void loadShader(std::string const& shaderFolderPath,
                    std::string const& shaderNameWithExt);
    void loadAtlas(std::string const& atlasFolderPath,
                   std::string const& atlasNameWithExt);
    Texture const& create1x1RGBATexture(uint32_t color, std::string name);
    Texture const& create1x1RGBATexture(uint8_t red, uint8_t green,
                                        uint8_t blue, uint8_t alpha,
                                        std::string name);
    Texture const& getTexture(std::string const& name) const;
    TextureAtlas const& getAtlas(std::string const& name) const;
    std::string const& getShaderSource(std::string const& shaderName) const;
    Shader& getShaderProgram(std::string const& shaderName) const;
    Shader& saveShaderProgram(std::string const& programName,
                              std::string const& vertexShaderName,
                              std::string const& fragmentShaderName);
    inline TTF_Font* getTtfFont(std::string const& name) const {
        return ttfFonts.at(name).get();
    }
    FontAtlas const& getFontAtlas(std::string const& atlasName,
                                  std::string const& fontName,
                                  unsigned fontSize, Vec4 const& color);
    DrawableText const& getDrawableText(std::string const& name);
    DrawableText const& saveDrawableText(std::string const& name,
                                         FontAtlas const& fontAtlas,
                                         Renderer& renderer,
                                         Shader const& shader);

   private:
    std::string getFileNameWithoutExtension(std::string const& fileName);
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
    std::unordered_map<std::string, std::unique_ptr<TTF_Font, DestroyFont>>
        ttfFonts;
    std::unordered_map<std::string, TextureAtlas> atlases;
    std::unordered_map<std::string, std::string> shaderSources;
    std::unordered_map<std::string, std::unique_ptr<Shader>> shaderPrograms;
    std::unordered_map<std::string, FontAtlas> fontAtlases;
    std::unordered_map<std::string, DrawableText> drawableTexts;

    Uint32 rmask, gmask, bmask, amask;
};
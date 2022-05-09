#include "AssetsManager.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <sys/types.h>

#include <array>
#include <iostream>
#include <nlohmann/json.hpp>
#include <glad/glad.h>

#include "EngineConstants.h"
#include "Utils.h"
#include "Debug.h"

AssetsManager::AssetsManager() {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
}

void AssetsManager::loadTexture(std::string const& textureFolderPath,
                                std::string const& textureNameWithExt) {
    SDL_Surface* loadedSurface = nullptr;
    auto texturePath = textureFolderPath + textureNameWithExt;
    loadedSurface = IMG_Load(texturePath.c_str());

    if (!loadedSurface) {
        throw std::runtime_error(texturePath + " failed to load.");
    }

    utils::rotateSurfaceUpsideDown(loadedSurface);
    SDL_LockSurface(loadedSurface);
    int format = loadedSurface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
    textures.emplace(std::make_pair(
        getFileNameWithoutExtension(textureNameWithExt),
        std::make_unique<Texture>(format, loadedSurface->w, loadedSurface->h,
                                  loadedSurface->pixels)));
    SDL_UnlockSurface(loadedSurface);
}

void AssetsManager::loadShader(std::string const& shaderFolderPath,
                               std::string const& shaderNameWithExt) {
    auto shaderPath = shaderFolderPath + shaderNameWithExt;
    auto* file = SDL_RWFromFile(shaderPath.c_str(), "r");
    if (!file) {
        throw std::runtime_error(shaderPath + " failed to load.");
    }
    std::array<char, 4096> buffer{};
    SDL_RWread(file, buffer.data(), buffer.size(), sizeof(char));
    SDL_RWclose(file);
    shaderSources.insert(std::make_pair(
        getFileNameWithoutExtension(shaderNameWithExt), buffer.data()));
}

void AssetsManager::loadFont(std::string const& fontFolderPath,
                             std::string const& fontNameWithExt) {
    auto fontPath = fontFolderPath + fontNameWithExt;
    TTF_Font* ttf = nullptr;
    ttf = TTF_OpenFont(fontPath.c_str(), 16);
    if (!ttf) {
        LOGI("FAIL %s", fontPath.c_str());
        throw std::runtime_error(fontPath + " failed to load.");
    }
    ttfFonts.insert(std::make_pair(
        fontNameWithExt, std::unique_ptr<TTF_Font, DestroyFont>(ttf)));
}

void AssetsManager::loadAtlas(std::string const& atlasFolderPath,
                              std::string const& atlasNameWithExt) {
    // this function loads both img and description in one call, so skip when
    // trying to load description
    if (atlasNameWithExt.find(".json", atlasNameWithExt.size() - 5) !=
        std::string::npos) {
        return;
    }
    auto atlasPath = atlasFolderPath + atlasNameWithExt;
    SDL_Surface* loadedSurface = nullptr;
    loadedSurface = IMG_Load(atlasPath.c_str());
    if (!loadedSurface) {
        throw std::runtime_error(atlasPath + " failed to load.");
    }
    int format = 0;
    int bpp = loadedSurface->format->BytesPerPixel;
    if (bpp == 4) {
        format = GL_RGBA;
    } else if (bpp == 3) {
        format = GL_RGB;
    } else if (bpp == 1) {
        format = GL_SRGB8;
    }

    utils::rotateSurfaceUpsideDown(loadedSurface);

    auto jsonPath = atlasFolderPath +
                    getFileNameWithoutExtension(atlasNameWithExt) + ".json";
    auto* file = SDL_RWFromFile(jsonPath.c_str(), "r");
    if (!file) {
        throw std::runtime_error(jsonPath + " failed to load.");
    }
    std::vector<char> buffer(65536);
    SDL_RWread(file, buffer.data(), buffer.size(), sizeof(char));
    SDL_RWclose(file);

    SDL_LockSurface(loadedSurface);
    TextureAtlas atlas(
        std::make_unique<Texture>(format, loadedSurface->w, loadedSurface->h,
                                  loadedSurface->pixels, true),
        getFileNameWithoutExtension(atlasNameWithExt));
    auto j = nlohmann::json::parse(buffer.data());
    float totalWidth = j["meta"]["size"]["w"];
    float totalHeight = j["meta"]["size"]["h"];
    for (auto const& [frameName, frameData] : j["frames"].items()) {
        float x = frameData["frame"]["x"];
        float y = frameData["frame"]["y"];
        float w = frameData["frame"]["w"];
        float h = frameData["frame"]["h"];
        float offsetX = frameData["spriteSourceSize"]["x"];
        float offsetY = frameData["spriteSourceSize"]["y"];
        float sourceW = frameData["sourceSize"]["w"];
        float sourceH = frameData["sourceSize"]["h"];
        float normW = w / totalWidth;
        float normH = h / totalHeight;
        float normX = x / totalWidth;
        float normY = 1.f - (y / totalHeight) - normH;  // flip y
        TextureAtlasFrame frame({offsetX, offsetY, sourceW, sourceH},
                                {normX, normY, normW, normH}, {w, h});
        atlas.addFrame(getFileNameWithoutExtension(frameName), frame);
    }

    atlases.emplace(std::make_pair(
        getFileNameWithoutExtension(atlasNameWithExt), std::move(atlas)));
    SDL_UnlockSurface(loadedSurface);
}

FontAtlas const& AssetsManager::getFontAtlas(std::string const& atlasName,
                                             std::string const& fontName,
                                             unsigned fontSize,
                                             Vec4 const& color) {
    auto found = fontAtlases.find(atlasName);
    if (found != fontAtlases.end()) {
        return found->second;
    }

    int glyphCount = 127;
    auto* font = ttfFonts[fontName].get();
    TTF_SetFontSize(font, fontSize);
    int maxCharWidth = 0, maxCharHeight = 0;
    std::vector<std::unique_ptr<SDL_Surface, DestroySurface>> glyphSurfaces;
    glyphSurfaces.reserve(glyphCount);
    for (uint16_t i = 0; i < glyphCount; ++i) {
        glyphSurfaces.emplace_back(
            TTF_RenderGlyph_Blended(font, i, utils::colorVecToSdlColor(color)));
        if (glyphSurfaces.back()) {
            utils::rotateSurfaceUpsideDown(glyphSurfaces.back().get());
            maxCharWidth = std::max(maxCharWidth, glyphSurfaces.back()->w);
            maxCharHeight = std::max(maxCharHeight, glyphSurfaces.back()->h);
        }
    }
    int maxSize = sqrt(glyphSurfaces.size()) + 1;

    int atlasWidth = maxCharWidth * maxSize;
    int atlasHeight = maxCharHeight * maxSize;
    auto atlasSurface =
        std::unique_ptr<SDL_Surface, DestroySurface>(SDL_CreateRGBSurface(
            0, atlasWidth, atlasHeight, 32, rmask, gmask, bmask, amask));

    SDL_Rect dest{};
    int validGlyphs = 0;
    std::vector<TextureAtlasFrame> frames;
    frames.reserve(glyphCount);
    for (size_t i = 0; i < glyphSurfaces.size(); ++i) {
        if (glyphSurfaces[i]) {
            dest.w = glyphSurfaces[i]->w;
            dest.h = glyphSurfaces[i]->h;
            SDL_BlitSurface(glyphSurfaces[i].get(), NULL, atlasSurface.get(),
                            &dest);
            float normW = static_cast<float>(dest.w) / atlasWidth;
            float normH = static_cast<float>(dest.h) / atlasHeight;
            float normX = static_cast<float>(dest.x) / atlasWidth;
            float normY = static_cast<float>(dest.y) / atlasHeight;
            float destW = static_cast<float>(dest.w);
            float destH = static_cast<float>(dest.h);

            frames.emplace_back(TextureAtlasFrame({0, 0, destW, destH},
                                                  {normX, normY, normW, normH},
                                                  {destW, destH}));

            if (++validGlyphs % maxSize == 0) {
                dest.x = 0;
                dest.y += maxCharHeight;
            }
            dest.x += dest.w;
        } else {
            frames.emplace_back(TextureAtlasFrame());
        }
    }

    return fontAtlases
        .emplace(std::pair(
            atlasName, FontAtlas(std::make_unique<Texture>(atlasSurface.get()),
                                 std::move(frames), TTF_FontLineSkip(font))))
        .first->second;
}

TextureAtlas const& AssetsManager::getAtlas(std::string const& name) const {
    return atlases.at(name);
}

std::string AssetsManager::getFileNameWithoutExtension(
    std::string const& fileName) {
    auto lastDot = fileName.find_last_of('.');
    if (lastDot == std::string::npos) {
        return fileName;
    }
    return fileName.substr(0, lastDot);
}

Texture const& AssetsManager::getTexture(std::string const& name) const {
    return *textures.at(name).get();
}
std::string const& AssetsManager::getShaderSource(
    std::string const& shaderName) const {
    return shaderSources.at(shaderName);
}

Texture const& AssetsManager::create1x1RGBATexture(uint32_t color,
                                                   std::string name) {
    return *textures
                .emplace(std::make_pair(
                    name, std::make_unique<Texture>(GL_RGBA, 1, 1, &color)))
                .first->second;
}

Texture const& AssetsManager::create1x1RGBATexture(uint8_t red, uint8_t green,
                                                   uint8_t blue, uint8_t alpha,
                                                   std::string name) {
    uint32_t color = 0;
    color |= red;
    color |= green << 8;
    color |= blue << 16;
    color |= alpha << 24;
    return *textures
                .emplace(std::make_pair(
                    name, std::make_unique<Texture>(GL_RGBA, 1, 1, &color)))
                .first->second;
}

Shader& AssetsManager::getShaderProgram(std::string const& shaderName) const {
    return *shaderPrograms.at(shaderName);
}

Shader& AssetsManager::saveShaderProgram(
    std::string const& programName, std::string const& vertexShaderName,
    std::string const& fragmentShaderName) {
    return *shaderPrograms
                .emplace(std::make_pair(
                    programName, std::make_unique<Shader>(
                                     getShaderSource(vertexShaderName),
                                     getShaderSource(fragmentShaderName))))
                .first->second;
}

DrawableText const& AssetsManager::saveDrawableText(std::string const& name,
                                                    FontAtlas const& fontAtlas,
                                                    Renderer& renderer,
                                                    Shader const& shader) {
    return drawableTexts
        .emplace(std::pair(name, DrawableText(fontAtlas, renderer, shader)))
        .first->second;
}
DrawableText const& AssetsManager::getDrawableText(std::string const& name) {
    return drawableTexts.at(name);
}
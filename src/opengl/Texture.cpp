#include "Texture.h"
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <iostream>

Texture::Texture(int format, int w, int h, void const* data, bool atlas)
    : width(w), height(h) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (atlas) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format,
                     GL_UNSIGNED_BYTE, data);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}
Texture::Texture(SDL_Surface* surface) {
    if (!surface) {
        return;
    }
    width = surface->w;
    height = surface->h;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned format = GL_RGBA;
    SDL_LockSurface(surface);
    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format,
                 GL_UNSIGNED_BYTE, surface->pixels);
    SDL_UnlockSurface(surface);
    glGenerateMipmap(GL_TEXTURE_2D);
}

bool Texture::sdlSwizzle(unsigned channel, unsigned surfaceMask) {
    unsigned swizzle;
    unsigned rmask, gmask, bmask, amask;
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
    if (surfaceMask == rmask)
        swizzle = GL_RED;
    else if (surfaceMask == gmask)
        swizzle = GL_GREEN;
    else if (surfaceMask == bmask)
        swizzle = GL_BLUE;
    else if (surfaceMask == amask)
        swizzle = GL_ALPHA;
    else {
        throw std::runtime_error("Unrecognized surface mask" +
                                 std::to_string(surfaceMask));
        return false;
    }
    glTexParameteri(GL_TEXTURE_2D, channel, swizzle);
    return true;
}
Texture::Texture(Texture&& o) noexcept
    : id(o.id), width(o.width), height(o.height) {
    o.id = 0;
    o.width = 0;
    o.height = 0;
}
Texture& Texture::operator=(Texture&& o) noexcept {
    if (this != &o) {
        glDeleteTextures(1, &id);
        this->id = o.id;
        this->width = o.width;
        this->height = o.height;

        o.id = 0;
        o.width = 0;
        o.height = 0;
    }
    return *this;
}
void Texture::setActiveAndBind(int number) const {
    glActiveTexture(GL_TEXTURE0 + number);
    glBindTexture(GL_TEXTURE_2D, id);
}
void Texture::bind() const { glBindTexture(GL_TEXTURE_2D, id); }
void Texture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
Texture::~Texture() { glDeleteTextures(1, &id); }

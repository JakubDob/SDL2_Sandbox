#pragma once
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <memory>
#include "Types.h"

class Window {
   public:
    struct DestroyWindow {
        void operator()(SDL_Window* w) { SDL_DestroyWindow(w); }
    };
    Window(char const* title, int width, int height,
           int flags = SDL_WINDOW_SHOWN);
    inline SDL_Surface* getSurface() const { return surface; }
    inline SDL_Window* getWindow() const { return window.get(); }
    inline float getWidthF() const { return width; }
    inline float getHeightF() const { return height; }
    inline int getWidthI() const { return static_cast<int>(width); }
    inline int getHeightI() const { return static_cast<int>(height); }
    inline void setWidth(int width) {
        this->width = static_cast<float>(width);
        this->aspectRatio = this->width / this->height;
    }
    inline void setHeight(int height) {
        this->height = static_cast<float>(height);
        this->aspectRatio = this->width / this->height;
    }
    inline float getAspectRatioF() const { return aspectRatio; };
    inline Vec2 getSize() const { return {width, height}; }

   private:
    std::unique_ptr<SDL_Window, DestroyWindow> window;
    SDL_Surface* surface;
    float width;
    float height;
    float aspectRatio;
    SDL_GLContext context;
};

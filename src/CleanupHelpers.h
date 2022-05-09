#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct DestroyTexture {
    void operator()(SDL_Texture* t) { SDL_DestroyTexture(t); }
};
struct DestroySurface {
    void operator()(SDL_Surface* s) { SDL_FreeSurface(s); }
};
struct DestroyFont {
    void operator()(TTF_Font* f) { TTF_CloseFont(f); }
};

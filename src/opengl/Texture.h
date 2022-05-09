#pragma once
#include <string>

struct SDL_Surface;
class Texture {
   public:
    Texture(int format, int w, int h, void const* data, bool atlas = false);
    Texture(SDL_Surface* surface);
    inline unsigned getId() const noexcept { return id; }
    inline int getWidth() const noexcept { return width; }
    inline int getHeight() const noexcept { return height; }
    inline float getWidthF() const noexcept {
        return static_cast<float>(width);
    }
    inline float getHeightF() const noexcept {
        return static_cast<float>(height);
    }
    void setActiveAndBind(int number) const;
    void bind() const;
    void unbind() const;
    bool sdlSwizzle(unsigned channel, unsigned surfaceMask);
    ~Texture();

    Texture& operator=(Texture const&) = delete;
    Texture(Texture const&) = delete;
    Texture& operator=(Texture&&) noexcept;
    Texture(Texture&&) noexcept;

   private:
    unsigned id = 0;
    int width = 0;
    int height = 0;
};
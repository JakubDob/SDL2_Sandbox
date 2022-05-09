#include "Window.h"
#include "Debug.h"
#include <string>
#include <stdexcept>
#include <iostream>

Window::Window(char const* title, int width, int height, int flags) {
#ifndef _MSVC_LANG
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    width = current.w;
    height = current.h;
#endif
#ifdef RENDER_DEBUG  // RenderDoc fails with OpenGL ES 3.0/3.1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
    window = std::unique_ptr<SDL_Window, DestroyWindow>(
        SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, width, height, flags));
    context = SDL_GL_CreateContext(getWindow());
    if (!context) {
        throw std::runtime_error(SDL_GetError());
    }
    if (SDL_GL_MakeCurrent(getWindow(), context) < 0) {
        throw std::runtime_error(SDL_GetError());
    }
    if (!gladLoadGLES2Loader(SDL_GL_GetProcAddress)) {
        throw std::runtime_error("Failed to load gladLoadGLES2Loader");
    }
    GLubyte const* version = glGetString(GL_VERSION);
    if (version == 0) {
        throw std::runtime_error("Unable to get the correct OpenGL ES version");
    }
#ifndef _MSVC_LANG
    // set fullscreen for mobile, TODO better conditional than msvc lang
    SDL_SetWindowFullscreen(window.get(), SDL_WINDOW_FULLSCREEN);
#endif
    SDL_GetWindowSize(window.get(), &width, &height);
    this->width = static_cast<float>(width);
    this->height = static_cast<float>(height);
    this->aspectRatio = this->width / this->height;
}

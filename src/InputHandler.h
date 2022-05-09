#pragma once
#include <SDL2/SDL.h>
#include <functional>
#include <vector>
#include "KeyToggle.h"
#include "InputTarget.h"

enum class PointerEvent { POINTER_UP, POINTER_DOWN };
enum class MultiGestureEvent { PINCH_CLOSE, PINCH_OPEN, ROTATE };

class InputHandler {
   public:
    void addKey(SDL_Scancode key, std::function<void()> action,
                bool toggable = false);
    void addMultiGestureCallback(
        MultiGestureEvent event,
        std::function<void(SDL_MultiGestureEvent const&)> action);
    void addPointerClickCallback(
        PointerEvent type,
        std::function<void(SDL_MouseButtonEvent const&)> action);
    void addPointerMoveCallback(
        std::function<void(SDL_MouseMotionEvent const&)> action);
    void registerInputTarget(InputTarget& target);
    void processEvent(SDL_Event const& e);
    void checkKeys();

   private:
    void checkMultiGestures(SDL_Event const& e) const;
    void checkPointerClickEvents(SDL_Event const& e) const;
    void checkPointerMoveEvents(SDL_Event const& e) const;
    bool checkInputTargets(SDL_Event const& e);
    struct KeyboardInputHelper {
        utility::KeyToggle toggled;
        std::function<void()> action;
        bool toggable;
    };
    struct MultiGestureInputHelper {
        MultiGestureEvent eventType;
        std::function<void(SDL_MultiGestureEvent const&)> action;
    };
    struct PointerClickInputHelper {
        PointerEvent eventType;
        std::function<void(SDL_MouseButtonEvent const&)> action;
    };

    std::vector<KeyboardInputHelper> keys;
    std::vector<PointerClickInputHelper> pointerClickInputs;
    std::vector<MultiGestureInputHelper> multiGestures;
    std::vector<std::function<void(SDL_MouseMotionEvent const&)>>
        pointerMoveInputs;
    std::vector<InputTarget*> inputTargets;
    InputTarget* focused = nullptr;
    uint8_t const* keystate = nullptr;
    int pointerPressed = false;
};
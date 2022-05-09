#include "InputHandler.h"
#include "Types.h"
#include <cmath>
#include "Utils.h"

void InputHandler::processEvent(SDL_Event const& e) {
    // check pointer targets first (GUI)
    if (checkInputTargets(e)) {
        return;
    }
    // then deeper layers
    checkPointerClickEvents(e);
    checkPointerMoveEvents(e);
    checkMultiGestures(e);
}
void InputHandler::addKey(SDL_Scancode key, std::function<void()> action,
                          bool toggable) {
    this->keys.push_back(
        {utility::KeyToggle(key, &keystate), action, toggable});
}
void InputHandler::addMultiGestureCallback(
    MultiGestureEvent event,
    std::function<void(SDL_MultiGestureEvent const&)> action) {
    this->multiGestures.push_back({event, action});
}
void InputHandler::addPointerClickCallback(
    PointerEvent type,
    std::function<void(SDL_MouseButtonEvent const&)> action) {
    this->pointerClickInputs.push_back({type, action});
}
void InputHandler::addPointerMoveCallback(
    std::function<void(SDL_MouseMotionEvent const&)> action) {
    this->pointerMoveInputs.emplace_back(action);
}
void InputHandler::registerInputTarget(InputTarget& target) {
    inputTargets.emplace_back(&target);
}
void InputHandler::checkKeys() {
    keystate = SDL_GetKeyboardState(NULL);
    for (auto& k : keys) {
        if (k.toggable) {
            if (k.toggled) {
                k.action();
            }
        } else {
            if (keystate[k.toggled.getKey()]) {
                k.action();
            }
        }
    }
}

void InputHandler::checkPointerClickEvents(SDL_Event const& e) const {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        for (auto& t : pointerClickInputs) {
            if (t.eventType == PointerEvent::POINTER_DOWN) {
                t.action(e.button);
            }
        }
    } else if (e.type == SDL_MOUSEBUTTONUP) {
        for (auto& t : pointerClickInputs) {
            if (t.eventType == PointerEvent::POINTER_UP) {
                t.action(e.button);
            }
        }
    }
}

void InputHandler::checkPointerMoveEvents(SDL_Event const& e) const {
    if (e.type == SDL_MOUSEMOTION) {
        for (auto& action : pointerMoveInputs) {
            action(e.motion);
        }
    }
}

void InputHandler::checkMultiGestures(SDL_Event const& e) const {
    if (e.type == SDL_MULTIGESTURE) {
        if (std::fabs(e.mgesture.dDist) > 0.002) {
            if (e.mgesture.dDist > 0) {
                for (auto const& g : multiGestures) {
                    if (g.eventType == MultiGestureEvent::PINCH_OPEN) {
                        g.action(e.mgesture);
                    }
                }
            } else {
                for (auto const& g : multiGestures) {
                    if (g.eventType == MultiGestureEvent::PINCH_CLOSE) {
                        g.action(e.mgesture);
                    }
                }
            }
        } else if (std::fabs(e.mgesture.dTheta) > 3.14 / 180.0) {
            for (auto const& g : multiGestures) {
                if (g.eventType == MultiGestureEvent::ROTATE) {
                    g.action(e.mgesture);
                }
            }
        }
    }
}

bool InputHandler::checkInputTargets(SDL_Event const& e) {
    bool some = false;
    if (focused) {
        // focused window is always first
        for (int i = 0; i < inputTargets.size(); ++i) {
            if (inputTargets[i] == focused) {
                auto* tmp = inputTargets[0];
                inputTargets[0] = focused;
                inputTargets[i] = tmp;
                break;
            }
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        pointerPressed = true;
        for (auto const& t : inputTargets) {
            if (!t->isVisible()) {
                continue;
            }
            Gui::UiEvent event(Gui::UiEventType::POINTER_DOWN);
            event.press.x = e.button.x;
            event.press.y = e.button.y;
            // send the event to the focused window, if it responds, do not
            // propagate the event
            if (t->currentlyInteracting(e.button.x, e.button.y)) {
                t->handleEvent(event);
                some = true;
                if (t->setFocus(true)) {
                    if (focused && focused != t) {
                        focused->setFocus(false);
                    }
                    focused = t;
                    break;
                }
            }
        }
    } else if (e.type == SDL_MOUSEBUTTONUP) {
        pointerPressed = false;
        for (auto& t : inputTargets) {
            if (!t->isVisible()) {
                continue;
            }
            if (t->currentlyInteracting(e.button.x, e.button.y)) {
                some = true;
            }
            Gui::UiEvent event(Gui::UiEventType::POINTER_UP);
            event.press.x = e.button.x;
            event.press.y = e.button.y;
            t->handleEvent(event);
        }
    } else if (e.type == SDL_MOUSEMOTION) {
        if (pointerPressed) {
            for (auto& t : inputTargets) {
                if (!t->isVisible()) {
                    continue;
                }
                if (t->currentlyInteracting(e.motion.x, e.motion.y)) {
                    some = true;
                    Gui::UiEvent event(Gui::UiEventType::DRAG);
                    event.move.x = e.motion.x;
                    event.move.y = e.motion.y;
                    event.move.dx = e.motion.xrel;
                    event.move.dy = e.motion.yrel;

                    t->handleEvent(event);
                    break;
                }
            }
        }
    }
    return some;
}
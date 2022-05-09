#pragma once
#include "Types.h"
#include <vector>
/**
 * @brief Provides interface for classes that handle user input.
 * Further propagation of inputs stops if any InputTarget was hit.
 * Used to mark gui windows which have priority over background elements.
 */
class InputTarget {
   public:
    virtual bool handleEvent(Gui::UiEvent const& event) = 0;
    virtual bool currentlyInteracting(int x, int y) = 0;
    virtual bool isVisible() const = 0;
    virtual bool setFocus(
        bool value) = 0;  // sets focus if possible, returns false otherwise
    virtual ~InputTarget() = default;
};
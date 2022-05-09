#include "KeyToggle.h"

namespace utility {
KeyToggle::operator bool() {
    if ((*kstate)[key]) {
        if (!state) {
            state = true;
            toggled = !toggled;
            return true;
        }
    } else {
        state = false;
    }
    return false;
}
}  // namespace utility

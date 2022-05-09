#pragma once
#include <cstdint>
namespace utility {
class KeyToggle {
   public:
    explicit KeyToggle(int sdlScancode, uint8_t const** kstate)
        : key(sdlScancode), kstate(kstate) {}
    explicit operator bool();
    // the key was switched on/off by pressing it
    inline bool isToggled() { return toggled; };
    inline int getKey() { return key; };

   private:
    // prevents toggle while holding the key
    bool state = false;
    bool toggled = false;
    int key = 0;
    uint8_t const** kstate = nullptr;
};
}  // namespace utility

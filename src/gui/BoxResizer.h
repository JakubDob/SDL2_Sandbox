#pragma once
#include "../Types.h"
namespace Gui {
class BoxResizer {
   public:
    inline static void resize(scalar_t& x, scalar_t& y, scalar_t& w,
                              scalar_t& h, BoxCorner corner, scalar_t dw,
                              scalar_t dh) {
        if (corner == BoxCorner::BOTTOM_LEFT) {
            w -= dw;
            x += dw;
            h += dh;
        } else if (corner == BoxCorner::BOTTOM_RIGHT) {
            w += dw;
            h += dh;
        }
    }
    inline static void resize(int& x, int& y, int& w, int& h, BoxCorner corner,
                              int dw, int dh) {
        if (corner == BoxCorner::BOTTOM_LEFT) {
            w -= dw;
            x += dw;
            h += dh;
        } else if (corner == BoxCorner::BOTTOM_RIGHT) {
            w += dw;
            h += dh;
        }
    }
};
}  // namespace Gui

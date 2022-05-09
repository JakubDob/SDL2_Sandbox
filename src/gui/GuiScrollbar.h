#pragma once
#include "GuiElement.h"
#include "../Types.h"
#include "GuiConstants.h"

namespace Gui {
class GuiWindow;
class GuiScrollbar : public GuiElement {
   public:
    GuiScrollbar(Orientation o, int windowWidth, int windowHeight,
                 GuiWindow* window, GuiScrollbar* other);
    float getScrollOffset() const { return scrollOffset; }
    int getThumbOffset() const { return thumbOffset; }
    Vec4 const& getDeadZone() const;

   private:
    virtual bool handleEvent(UiEvent const& e) override;
    void setThumbPosition();
    void adjustContentLength();
    inline void setDeadZone(int x, int y, int w, int h) {
        deadZone[0] = x;
        deadZone[1] = y;
        deadZone[2] = w;
        deadZone[3] = h;
    }
    inline void unsetDeadZone() {
        deadZone[2] = 0;
        deadZone[3] = 0;
    }
    GuiWindow* window;
    GuiScrollbar* other;
    Orientation o;
    Vec4 thumb;
    // square made when two scrollbars are visible at the same time
    Vec4 deadZone = {0.f, 0.f, 0.f, 0.f};
    bool deadZoneOwner = false;
    // scroll axis = axis along the direction of scrolling
    // (horizontal/vertical), cross axis is the opposite
    float scrollAxisLen = 0;
    int crossAxisLen = GuiConstant::Scrollbar::minThumbCrossAxisLen;
    // how much slider is moved
    int thumbOffset = 0;
    // how much container is moved
    float scrollOffset = 0;
    int windowWidth = 0;
    int windowHeight = 0;
    float containerHeight = 0;
    float containerWidth = 0;
};
}  // namespace Gui

#pragma once
#include "../Types.h"
/**
 * @brief
 * Margins clear the space around an element. They do not affect the
 * clickable/renderable area. Padding affects the size of clickable/renderable
 * area. Both margin and padding affect the total width and height of an
 * element.
 */
namespace Gui {
struct BoxModel {
    int contentWidth = 0;
    int contentHeight = 0;
    int paddingTop = 0;
    int paddingBottom = 0;
    int paddingLeft = 0;
    int paddingRight = 0;
    int marginTop = 0;
    int marginBottom = 0;
    int marginLeft = 0;
    int marginRight = 0;
};

struct ColoredRect {
    ColoredRect(Vec4 c, Vec4& r) : color(c), rect(r) {}
    Vec4 color;
    Vec4& rect;
};

enum class BoxCorner { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };

enum class Orientation { HORIZONTAL, VERTICAL };
enum class ElementLayout { HORIZONTAL, VERTICAL };

enum class TextAlignment { LEFT, RIGHT, CENTER };

enum class WindowEventType {
    POINTER_DOWN,
    POINTER_UP,
    DRAG,
    CONTAINER_SIZE_CHANGE,
    WINDOW_SIZE_CHANGE
};

struct PointerPressEvent {
    // pointer up / down
    WindowEventType type;
    int x, y;
};

struct PointerMoveEvent {
    // drag
    WindowEventType type;
    int x, y, dx, dy;
};

struct SizeChangeEvent {
    // window/container
    WindowEventType type;
    int w, h;
};

union WindowEvent {
    WindowEvent(WindowEventType type) : type(type) {}
    WindowEventType type;
    PointerPressEvent press;
    PointerMoveEvent move;
    SizeChangeEvent size;
};
}  // namespace Gui
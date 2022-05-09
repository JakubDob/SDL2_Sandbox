#pragma once
#include "Matrix.h"

using scalar_t = float;
using Speed = scalar_t;
using Time = scalar_t;
using DegreeAngle = scalar_t;
using Vec2 = math::Matrix<scalar_t, 2, 1>;
using Vec3 = math::Matrix<scalar_t, 3, 1>;
using Vec4 = math::Matrix<scalar_t, 4, 1>;
using Mat2 = math::Matrix<scalar_t, 2, 2>;
using Mat3 = math::Matrix<scalar_t, 3, 3>;
using Mat4 = math::Matrix<scalar_t, 4, 4>;
using Position2D = Vec2;
using Velocity2D = Vec2;
using Direction2D = Vec2;
using Force2D = Vec2;
using Point2D = Vec2;
using Displacement2D = Vec2;

using byte = uint8_t;
using addr_t = uintptr_t;
using offset_t = ptrdiff_t;
using XYUV = Vec4;
using GeometryRect = Vec4;  // rectangle with (0,0) at top left corner (x,y,w,h)
using TextureRect = Vec4;  // rectangle specifying texture coordinates (x,y,w,h)

enum class Shape { POLYGON, CIRCLE };

enum class ColliderType { PHYSICS, HITBOX };

enum class VertexName { TOP_LEFT, TOP_RIGHT, BOTTOM_RIGHT, BOTTOM_LEFT };

template <typename... Ts>
struct TypeSequence {};

namespace ecs {
using EntityId = size_t;  // index
using EntityVersion = size_t;
using ComponentSig = uint64_t;
using ComponentId = size_t;

}  // namespace ecs
namespace Engine {
struct Rect {
    int x, y, w, h;
};
struct Color {
    float r, g, b, a;
};
}  // namespace Engine

namespace utils {
template <typename T>
class Iterator {
   public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;

    constexpr Iterator() : ptr(nullptr) {}
    constexpr Iterator(pointer ptr) : ptr(ptr) {}
    constexpr Iterator& operator++() noexcept {
        ++ptr;
        return *this;
    }
    constexpr Iterator operator++(int) noexcept {
        auto tmp = *this;
        ++ptr;
        return tmp;
    }
    constexpr Iterator& operator--() noexcept {
        --ptr;
        return *this;
    }
    constexpr Iterator operator--(int) noexcept {
        auto tmp = *this;
        --ptr;
        return tmp;
    }
    constexpr Iterator& operator+=(int offset) noexcept {
        ptr += offset;
        return *this;
    }
    constexpr Iterator& operator-=(int offset) noexcept {
        ptr -= offset;
        return *this;
    }
    constexpr Iterator operator-(int i) const noexcept {
        auto tmp = *this;
        tmp -= i;
        return tmp;
    }
    constexpr Iterator operator+(int i) const noexcept {
        auto tmp = *this;
        tmp += i;
        return tmp;
    }
    constexpr difference_type operator-(Iterator const& o) const noexcept {
        return ptr - o.ptr;
    }
    constexpr reference operator*() noexcept { return *ptr; }
    constexpr pointer operator->() const noexcept { return ptr; }
    constexpr bool operator==(Iterator const& o) const noexcept {
        return ptr == o.ptr;
    }
    constexpr bool operator!=(Iterator const& o) const noexcept {
        return !(*this == o);
    }
    constexpr bool operator>(Iterator const& o) const noexcept {
        return ptr > o.ptr;
    }
    constexpr bool operator<(Iterator const& o) const noexcept {
        return ptr < o.ptr;
    }
    constexpr bool operator>=(Iterator const& o) const noexcept {
        return !(*this < o);
    }
    constexpr bool operator<=(Iterator const& o) const noexcept {
        return !(*this > o);
    }
    pointer ptr;
};

template <typename T>
class ConstIterator {
   public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;

    constexpr ConstIterator() : ptr(nullptr) {}
    constexpr ConstIterator(Iterator<T> o) : ptr(o.ptr) {}
    constexpr ConstIterator& operator++() noexcept {
        ++ptr;
        return *this;
    }
    constexpr ConstIterator operator++(int) noexcept {
        auto tmp = *this;
        ++ptr;
        return tmp;
    }
    constexpr ConstIterator& operator--() noexcept {
        --ptr;
        return *this;
    }
    constexpr ConstIterator operator--(int) noexcept {
        auto tmp = *this;
        --ptr;
        return tmp;
    }
    constexpr ConstIterator& operator+=(int offset) noexcept {
        ptr += offset;
        return *this;
    }
    constexpr ConstIterator& operator-=(int offset) noexcept {
        ptr -= offset;
        return *this;
    }
    constexpr ConstIterator operator-(int i) const noexcept {
        auto tmp = *this;
        tmp -= i;
        return tmp;
    }
    constexpr ConstIterator operator+(int i) const noexcept {
        auto tmp = *this;
        tmp += i;
        return tmp;
    }
    constexpr difference_type operator-(ConstIterator const& o) const noexcept {
        return ptr - o.ptr;
    }
    constexpr difference_type operator+(ConstIterator const& o) const noexcept {
        return ptr + o.ptr;
    }
    constexpr reference operator*() const noexcept { return *ptr; }
    constexpr pointer const operator->() const noexcept { return ptr; }
    constexpr bool operator==(ConstIterator const& o) const noexcept {
        return ptr == o.ptr;
    }
    constexpr bool operator!=(ConstIterator const& o) const noexcept {
        return !(*this == o);
    }
    constexpr bool operator>(ConstIterator const& o) const noexcept {
        return ptr > o.ptr;
    }
    constexpr bool operator<(ConstIterator const& o) const noexcept {
        return ptr < o.ptr;
    }
    constexpr bool operator>=(ConstIterator const& o) const noexcept {
        return !(*this < o);
    }
    constexpr bool operator<=(ConstIterator const& o) const noexcept {
        return !(*this > o);
    }
    pointer ptr;
};
}  // namespace utils

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

enum class BoxCorner { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT, NONE };

enum class Orientation { HORIZONTAL, VERTICAL };
enum class ElementLayout { HORIZONTAL, VERTICAL };

enum class TextAlignment { LEFT, RIGHT, CENTER };

enum class UiEventType {
    POINTER_DOWN,
    POINTER_UP,
    DRAG,
    CONTAINER_SIZE_CHANGE,
    WINDOW_SIZE_CHANGE
};

struct PointerPressEvent {
    // pointer up / down
    UiEventType type;
    int x, y;
};

struct PointerMoveEvent {
    // drag
    UiEventType type;
    int x, y, dx, dy;
};

struct SizeChangeEvent {
    // window/container
    UiEventType type;
    int w, h;
};

// contains both window events(resize, drag) and click events
union UiEvent {
    UiEvent(UiEventType type) : type(type) {}
    UiEventType type;
    PointerPressEvent press;
    PointerMoveEvent move;
    SizeChangeEvent size;
};
}  // namespace Gui
#include "BoxCollider.h"
#include "../../Utils.h"

BoxCollider::BoxCollider(scalar_t width, scalar_t height, ColliderType type,
                         Vec2 center)
    : BaseCollider2D(center, type) {
    if (width <= 0 || height <= 0) {
        throw std::out_of_range("Width and height must be > 0");
    }

    Vec4 v[4];
    scalar_t div = 2;
    v[0][0] = -width / div + center[0];
    v[0][1] = height / div + center[1];
    v[1][0] = width / div + center[0];
    v[1][1] = height / div + center[1];
    v[2][0] = width / div + center[0];
    v[2][1] = -height / div + center[1];
    v[3][0] = -width / div + center[0];
    v[3][1] = -height / div + center[1];

    for (int i = 0; i < 4; ++i) {
        v[i][3] = 1;
        addModelSpaceData(ColliderData(
            v[i],
            utils::counterclockwiseNormalNormalized(v[(i + 1) % 4] - v[i])));
    }
}
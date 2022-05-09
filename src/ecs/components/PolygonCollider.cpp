#include "PolygonCollider.h"
#include "../../Utils.h"

PolygonCollider::PolygonCollider(std::vector<Vec4> const& clockwiseVertices,
                                 ColliderType type, Vec2 center)
    : BaseCollider2D(center, type) {
    auto sz = clockwiseVertices.size();
    Vec4 oVertA, oVertB;
    for (size_t i = 0; i < sz; ++i) {
        oVertA = clockwiseVertices[i];
        oVertA[0] += center[0];
        oVertA[1] += center[1];
        oVertB = clockwiseVertices[(i + 1) % sz];
        oVertB[0] += center[0];
        oVertB[1] += center[1];
        addModelSpaceData(ColliderData(
            oVertA, utils::counterclockwiseNormalNormalized(oVertB - oVertA)
                        .normalize()));
    }
}
#include "CircleCollider.h"
#include "BoxCollider.h"

CircleCollider::CircleCollider(scalar_t radius, ColliderType type, Vec2 center)
    : BaseCollider2D(center, type, Shape::CIRCLE, radius) {
    /*Model space data is needed to compute a bounding box. It is used to check
    potential collisions. In the actual collision testing (using SAT) only a
    radius is required.*/
    BoxCollider bc(radius * 2, radius * 2, type, center);
    auto data = bc.getModelSpaceData();
    for (auto d : data) {
        addModelSpaceData(std::move(d));
    }
}
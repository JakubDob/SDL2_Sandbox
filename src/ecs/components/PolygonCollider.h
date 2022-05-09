#pragma once
#include "BaseCollider2D.h"
#include "../../Types.h"
class PolygonCollider : public BaseCollider2D {
   public:
    PolygonCollider(std::vector<Vec4> const& clockwiseVertices,
                    ColliderType type, Vec2 center = Vec2({0, 0}));
};
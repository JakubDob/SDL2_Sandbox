#pragma once
#include "../../Types.h"
#include "BaseCollider2D.h"

class BoxCollider : public BaseCollider2D {
   public:
    BoxCollider(scalar_t width, scalar_t height, ColliderType type,
                Vec2 center = Vec2({0, 0}));
};
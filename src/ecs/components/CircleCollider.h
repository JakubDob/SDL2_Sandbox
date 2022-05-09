#pragma once
#include "BaseCollider2D.h"

class CircleCollider : public BaseCollider2D {
   public:
    CircleCollider(scalar_t radius, ColliderType type,
                   Vec2 center = Vec2({0, 0}));
};
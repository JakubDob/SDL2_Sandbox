#pragma once
#include "CollisionSystem2D.h"
#include "../../Types.h"

namespace ecs {
class PhysicsSystem {
   public:
    void update(CollisionSystem2D const& cs, ecs::EcsContainer& ecsContainer,
                scalar_t dt);
    inline void enableGravity(bool value) { gravityEnabled = value; }
    inline void setGravity(Vec2 const& value) { gravity = value; }
    inline void toggleGravity() { gravityEnabled = !gravityEnabled; }

   private:
    Vec2 gravity = {0, -10.0};
    bool gravityEnabled = true;
};
}  // namespace ecs
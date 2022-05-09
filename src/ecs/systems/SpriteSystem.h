#pragma once
#include "Renderer.h"
#include "../EcsContainer.h"

namespace ecs {
class SpriteSystem {
   public:
    void update(EcsContainer& container, Renderer& renderer,
                Shader const& shader, Mat4 const& projectionView, scalar_t dt);
};
}  // namespace ecs

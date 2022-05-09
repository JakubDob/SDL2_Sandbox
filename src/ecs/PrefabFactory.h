#pragma once
#include "EcsContainer.h"
#include "../Types.h"
#include "../opengl/Texture.h"

class TextureAtlas;
namespace ecs {
class PrefabFactory {
   public:
    static Entity createStaticSprite(ecs::EcsContainer& ecsContainer,
                                     Vec4 color, scalar_t width,
                                     scalar_t height);
    static Entity createStaticSprite(ecs::EcsContainer& ecsContainer,
                                     Texture const& texture, scalar_t width,
                                     scalar_t height);
    static Entity createStaticSprite(ecs::EcsContainer& ecsContainer,
                                     TextureAtlas const& atlas,
                                     std::string frameName, scalar_t width,
                                     scalar_t height,
                                     bool preserveAspectRatio = true);
    static Entity createAnimatedSprite(ecs::EcsContainer& ecsContainer,
                                       TextureAtlas const& atlas,
                                       scalar_t width, scalar_t height);
};
}  // namespace ecs
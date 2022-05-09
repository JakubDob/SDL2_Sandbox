#pragma once
#include "Renderer.h"
#include "../EcsContainer.h"
#include "../../opengl/Shader.h"
#include <vector>
#include <functional>

namespace ecs {
class HealthBarSystem {
   public:
    using CallbackType = std::function<void(EcsContainer&, Entity)>;
    void update(EcsContainer& ecsContainer, Renderer& renderer,
                Shader const& shader);
    // TODO: event system / unsubscribe option
    size_t onDeadEntity(CallbackType fn);

   private:
    std::vector<Entity> deadEntities;
    std::vector<CallbackType> onDeadCbs;
};
}  // namespace ecs

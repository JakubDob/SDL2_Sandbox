#include "HealthBarSystem.h"
#include "../components/HealthBar.h"
#include "../components/Transform2D.h"

namespace ecs {
void HealthBarSystem::update(EcsContainer& ecsContainer, Renderer& renderer,
                             Shader const& shader) {
    auto& components =
        ecsContainer.getEntitiesWithComponents<HealthBar, Transform2D>();
    for (auto& [healthBar, transform] : components) {
        if (healthBar->getCurrentValue() == 0) {
            deadEntities.emplace_back(healthBar->getEntity());
        }

        bool flipY = transform->modelToWorld()[0] < 0;
        auto meshId = renderer.addMesh(healthBar->getMesh(flipY));
        renderer.getMesh(meshId).transformPosition(transform->modelToWorld());
        RenderCommand rc(meshId, shader);
        rc.texture = nullptr;
        rc.opaque = true;
        renderer.addRenderCommand(rc);
    }

    for (auto const& cb : onDeadCbs) {
        for (auto const& entity : deadEntities) {
            cb(ecsContainer, entity);
        }
    }
    deadEntities.clear();
}

size_t HealthBarSystem::onDeadEntity(CallbackType cb) {
    onDeadCbs.emplace_back(cb);
    return onDeadCbs.size() - 1;
}
}  // namespace ecs
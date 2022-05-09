#include "SpriteSystem.h"
#include "../components/Transform2D.h"
#include "../components/StaticSprite.h"
#include "../components/AnimatedSprite.h"
namespace ecs {
void SpriteSystem::update(EcsContainer& container, Renderer& renderer,
                          Shader const& shader, Mat4 const& projectionView,
                          scalar_t dt) {
    auto& staticComponents =
        container.getEntitiesWithComponents<Transform2D, StaticSprite>();
    auto& animatedComponents =
        container.getEntitiesWithComponents<Transform2D, AnimatedSprite>();

    for (auto& [transform, sprite] : staticComponents) {
        auto meshId = renderer.addMesh(sprite->getMesh());
        renderer.getMesh(meshId).transformPosition(transform->modelToWorld());
        renderer.getMesh(meshId).setDepth(transform->getNdcDepth());
        RenderCommand rc(meshId, shader);
        rc.texture = sprite->getTexture();
        rc.state.depthState.enabled = true;
        rc.state.depthState.depth = transform->getNdcDepth();
        rc.opaque = sprite->isOpaque();
        renderer.addRenderCommand(rc);
    }

    for (auto& [transform, sprite] : animatedComponents) {
        sprite->update(container, sprite->getEntity(), dt);
        sprite->play(container, sprite->getEntity());
        auto meshId = renderer.addMesh(sprite->getMesh());
        renderer.getMesh(meshId).transformPosition(transform->modelToWorld());
        renderer.getMesh(meshId).setDepth(transform->getNdcDepth());
        RenderCommand rc(meshId, shader);
        rc.texture = sprite->getTexture();
        rc.state.depthState.enabled = true;
        rc.state.depthState.depth = transform->getNdcDepth();
        rc.opaque = sprite->isOpaque();
        renderer.addRenderCommand(rc);
    }
}
}  // namespace ecs

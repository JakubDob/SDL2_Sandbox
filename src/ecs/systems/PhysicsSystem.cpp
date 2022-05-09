#include "PhysicsSystem.h"
#include "../components/Physics2D.h"
#include "../components/Transform2D.h"
#include "../components/Collider2D.h"
#include <cmath>

namespace ecs {
void PhysicsSystem::update(CollisionSystem2D const& cs,
                           ecs::EcsContainer& ecsContainer, scalar_t dt) {
    for (auto& transform : ForEachComponent<Transform2D>(ecsContainer)) {
        auto const& entity = transform.getEntity();
        auto* physics = ecsContainer.getComponent<Physics2D>(entity);
        auto* collider = ecsContainer.getComponent<Collider2D>(entity);
        if (physics) {
            if (!physics->isStatic()) {
                auto vel = physics->getLinearVelocity();
                auto pos = transform.getPosition();
                transform.rotate(physics->getAngularVelocity() * dt);

                if (!physics->isGrounded() && gravityEnabled) {
                    physics->setLinearVelocity(vel + gravity * dt);
                } else {
                    physics->addToRestVelocity(-gravity[1] * dt);
                    if (physics->getRestVelocity() > -gravity[1] * dt * 2) {
                        physics->setGrounded(false);
                    }
                }
                transform.setPosition(pos + physics->getLinearVelocity() * dt);
            }
        }
        if (collider) {
            collider->update(transform.modelToWorld(),
                             transform.normalsRotation(),
                             transform.getScaleFactor());
        }
    }
}
}  // namespace ecs
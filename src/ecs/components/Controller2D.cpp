#include "Controller2D.h"
#include "Physics2D.h"
#include "Transform2D.h"

void Controller2D::moveUp(ecs::EcsContainer& ecsContainer, Time dt) {
    if (!ecsContainer.exists(getEntity())) {
        return;
    }
    auto* physics = ecsContainer.getComponent<Physics2D>(getEntity());
    if (physics) {
        physics->applyForce({0, movementSpeed * dt});
        return;
    }
    auto* transform = ecsContainer.getComponent<Transform2D>(getEntity());
    if (transform) {
        transform->translate({0, movementSpeed * dt * 0.5f});
    }
}
void Controller2D::moveDown(ecs::EcsContainer& ecsContainer, Time dt) {
    if (!ecsContainer.exists(getEntity())) {
        return;
    }
    auto* physics = ecsContainer.getComponent<Physics2D>(getEntity());
    if (physics) {
        physics->applyForce({0, -movementSpeed * dt});
        return;
    }
    auto* transform = ecsContainer.getComponent<Transform2D>(getEntity());
    if (transform) {
        transform->translate({0, -movementSpeed * dt * 0.5f});
    }
}
void Controller2D::moveLeft(ecs::EcsContainer& ecsContainer, Time dt) {
    if (!ecsContainer.exists(getEntity())) {
        return;
    }

    auto* physics = ecsContainer.getComponent<Physics2D>(getEntity());
    if (physics) {
        auto lv = physics->getLinearVelocity();
        if (lv[0] > 0) {
            physics->setLinearVelocity({0, lv[1]});
        }
        physics->applyForce({-movementSpeed * dt, 0});
        return;
    }
    auto* transform = ecsContainer.getComponent<Transform2D>(getEntity());
    if (transform) {
        transform->translate({-movementSpeed * dt * 0.5f, 0});
    }
}
void Controller2D::moveRight(ecs::EcsContainer& ecsContainer, Time dt) {
    if (!ecsContainer.exists(getEntity())) {
        return;
    }

    auto* physics = ecsContainer.getComponent<Physics2D>(getEntity());
    if (physics) {
        auto lv = physics->getLinearVelocity();
        if (lv[0] < 0) {
            physics->setLinearVelocity({0, lv[1]});
        }
        physics->applyForce({movementSpeed * dt, 0});
        return;
    }
    auto* transform = ecsContainer.getComponent<Transform2D>(getEntity());
    if (transform) {
        transform->translate({movementSpeed * dt * 0.5f, 0});
    }
}
void Controller2D::scaleUp(ecs::EcsContainer& ecsContainer, Time dt) {
    if (!ecsContainer.exists(getEntity())) {
        return;
    }
    auto* transform = ecsContainer.getComponent<Transform2D>(getEntity());
    if (transform) {
        transform->scale(scaleFactor * dt * 0.5f);
    }
}
void Controller2D::scaleDown(ecs::EcsContainer& ecsContainer, Time dt) {
    if (!ecsContainer.exists(getEntity())) {
        return;
    }
    auto* transform = ecsContainer.getComponent<Transform2D>(getEntity());
    if (transform) {
        transform->scale(-scaleFactor * dt * 0.5f);
    }
}
void Controller2D::rotateClockwise(ecs::EcsContainer& ecsContainer, Time dt) {
    if (!ecsContainer.exists(getEntity())) {
        return;
    }
    auto* transform = ecsContainer.getComponent<Transform2D>(getEntity());
    if (transform) {
        transform->rotate(-rotationSpeed * dt * 0.5f);
    }
}
void Controller2D::rotateCounterClockwise(ecs::EcsContainer& ecsContainer,
                                          Time dt) {
    if (!ecsContainer.exists(getEntity())) {
        return;
    }
    auto* transform = ecsContainer.getComponent<Transform2D>(getEntity());
    if (transform) {
        transform->rotate(rotationSpeed * dt * 0.5f);
    }
}

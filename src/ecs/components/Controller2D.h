#pragma once
#include "../../Types.h"
#include "../EcsComponentList.h"
#include "../EcsContainer.h"

class Controller2D
    : public ecs::ComponentBase<Controller2D, ecs::Controller2DTag,
                                ecs::ComponentTags> {
   public:
    void moveUp(ecs::EcsContainer& ecsContainer, scalar_t dt);
    void moveDown(ecs::EcsContainer& ecsContainer, scalar_t dt);
    void moveLeft(ecs::EcsContainer& ecsContainer, scalar_t dt);
    void moveRight(ecs::EcsContainer& ecsContainer, scalar_t dt);
    void scaleUp(ecs::EcsContainer& ecsContainer, scalar_t dt);
    void scaleDown(ecs::EcsContainer& ecsContainer, scalar_t dt);
    void rotateClockwise(ecs::EcsContainer& ecsContainer, scalar_t dt);
    void rotateCounterClockwise(ecs::EcsContainer& ecsContainer, scalar_t dt);

    scalar_t rotationSpeed = 100;
    scalar_t translationSpeed = 5;
    scalar_t movementSpeed = 30;
    scalar_t scaleFactor = 8;
    int attackType = 0;
};
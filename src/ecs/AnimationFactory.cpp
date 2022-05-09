#include "AnimationFactory.h"
#include "components/Controller2D.h"
#include "components/Transform2D.h"
#include "components/Physics2D.h"

namespace ecs {
AnimatedSprite::FsmType AnimationFactory::createBasicAnimation() {
    AnimatedSprite::FsmType fsm;
    using namespace animation;

    auto attackToIdle = [](ecs::EcsContainer& ecsContainer, Entity entity) {
        auto* sprite = ecsContainer.getComponent<AnimatedSprite>(entity);
        auto* controller = ecsContainer.getComponent<Controller2D>(entity);
        if (controller && sprite && sprite->getFullPlayCount() >= 1) {
            controller->attackType = 0;
            return true;
        }
        return false;
    };

    fsm.setUniversalStateRule<ATTACK_0>(
        [](ecs::EcsContainer& ecsContainer, Entity entity) {
            auto* controller = ecsContainer.getComponent<Controller2D>(entity);
            return controller && controller->attackType == 1;
        });

    fsm.setUniversalStateRule<ATTACK_1>(
        [](ecs::EcsContainer& ecsContainer, Entity entity) {
            auto* controller = ecsContainer.getComponent<Controller2D>(entity);
            return controller && controller->attackType == 2;
        });

    fsm.clearBidirectionalRules<ATTACK_0, ATTACK_1>();

    fsm.rule<ATTACK_0, IDLE>(attackToIdle);
    fsm.rule<ATTACK_1, IDLE>(attackToIdle);

    fsm.rule<IDLE, WALK>([](ecs::EcsContainer& ecsContainer, Entity entity) {
        auto* physics = ecsContainer.getComponent<Physics2D>(entity);
        auto* transform = ecsContainer.getComponent<Transform2D>(entity);
        if (physics && transform) {
            if (math::dot(transform->up(), physics->getLinearVelocity()) <= 0) {
                auto speed = physics->getSpeed();
                if (speed > 0.3f) {
                    return true;
                }
            }
        }
        return false;
    });

    fsm.rule<IDLE, JUMP>([](ecs::EcsContainer& ecsContainer, Entity entity) {
        auto* physics = ecsContainer.getComponent<Physics2D>(entity);
        auto* transform = ecsContainer.getComponent<Transform2D>(entity);
        if (physics && transform) {
            if (math::dot(transform->up(), physics->getLinearVelocity()) > 0) {
                return true;
            }
        }
        return false;
    });

    fsm.rule<WALK, JUMP>([](ecs::EcsContainer& ecsContainer, Entity entity) {
        auto* physics = ecsContainer.getComponent<Physics2D>(entity);
        auto* transform = ecsContainer.getComponent<Transform2D>(entity);
        if (physics && transform) {
            if (math::dot(transform->up(), physics->getLinearVelocity()) > 0) {
                return true;
            }
        }
        return false;
    });

    fsm.rule<WALK, RUN>([](ecs::EcsContainer& ecsContainer, Entity entity) {
        auto* physics = ecsContainer.getComponent<Physics2D>(entity);
        if (physics) {
            if (physics->getSpeed() > 8) {
                return true;
            }
        }
        return false;
    });

    fsm.rule<RUN, WALK>([](ecs::EcsContainer& ecsContainer, Entity entity) {
        auto* physics = ecsContainer.getComponent<Physics2D>(entity);
        if (physics) {
            if (physics->getSpeed() < 8) {
                return true;
            }
        }
        return false;
    });

    fsm.rule<JUMP, WALK>([](ecs::EcsContainer& ecsContainer, Entity entity) {
        auto* physics = ecsContainer.getComponent<Physics2D>(entity);
        auto* transform = ecsContainer.getComponent<Transform2D>(entity);
        if (physics && transform) {
            if (math::dot(transform->up(), physics->getLinearVelocity()) <= 0 &&
                physics->getSpeed() > 0.3f) {
                return true;
            }
        }
        return false;
    });

    fsm.rule<JUMP, IDLE>([](ecs::EcsContainer& ecsContainer, Entity entity) {
        auto* physics = ecsContainer.getComponent<Physics2D>(entity);
        if (physics) {
            if (physics->getSpeed() < 0.2f) {
                return true;
            }
        }
        return false;
    });

    fsm.rule<WALK, IDLE>([](ecs::EcsContainer& ecsContainer, Entity entity) {
        auto* physics = ecsContainer.getComponent<Physics2D>(entity);
        if (physics) {
            if (physics->getSpeed() < 0.2f) {
                return true;
            }
        }
        return false;
    });

    return fsm;
}
}  // namespace ecs
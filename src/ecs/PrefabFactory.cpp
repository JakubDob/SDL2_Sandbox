#include "PrefabFactory.h"
#include "components/Collider2D.h"
#include "components/BoxCollider.h"
#include "components/PolygonCollider.h"
#include "components/CircleCollider.h"
#include "components/Controller2D.h"
#include "components/Physics2D.h"
#include "components/Transform2D.h"
#include "components/HealthBar.h"
#include "components/StaticSprite.h"
#include "components/AnimatedSprite.h"
#include "../opengl/VertexArray.h"
#include "../opengl/Texture.h"
#include "AnimationFactory.h"
#include "../TextureAtlas.h"

// temp class for testing
// TODO: refactor
namespace ecs {

Entity PrefabFactory::createStaticSprite(ecs::EcsContainer& ecsContainer,
                                         Vec4 color, scalar_t width,
                                         scalar_t height) {
    Entity entity = ecsContainer.createEntity();
    StaticSprite sprite(color, width, height);
    Collider2D collider;
    collider.add<BoxCollider>(width, height, ColliderType::PHYSICS);
    if (color == Vec4({1, 0, 0, 1})) {
        collider.setHitboxCallback(
            0, [ch = CircularBuffer<CollisionHistory, 4>()](
                   EcsContainer& ecs, Entity entity) mutable {
                auto* hp = ecs.getComponent<HealthBar>(entity);
                if (hp) {
                    bool hit = false;
                    auto& buff = ch.getBuffer();
                    auto result =
                        std::find_if(buff.begin(), buff.end(),
                                     [entity](CollisionHistory const& a) {
                                         return a.entity == entity;
                                     });
                    if (result != buff.end()) {
                        if (result->timeStamp.resetIfOlderThanMs(1000)) {
                            hit = true;
                        }
                    } else {
                        ch.push({entity});
                        hit = true;
                    }
                    if (hit) {
                        auto dmg =
                            utils::RandomMatrix<scalar_t>::instance().getScalar(
                                100, 200);
                        hp->changeHp(-dmg);
                        std::cout << "Hit for " << dmg << ".\n";
                    }
                }
            });
    }
    ecsContainer.addComponent<Collider2D>(entity, std::move(collider));
    ecsContainer.addComponent<StaticSprite>(entity, std::move(sprite));
    ecsContainer.addComponent<Transform2D>(entity);
    // ecsContainer.addComponent<Physics2D>(entity);
    ecsContainer.addComponent<Controller2D>(entity);
    ecsContainer.addComponent<HealthBar>(entity, 100, height);
    return entity;
}
Entity PrefabFactory::createStaticSprite(ecs::EcsContainer& ecsContainer,
                                         Texture const& texture, scalar_t width,
                                         scalar_t height) {
    Entity entity = ecsContainer.createEntity();
    StaticSprite sprite(texture, width, height);
    Collider2D collider;
    collider.add<BoxCollider>(width, height, ColliderType::PHYSICS);
    ecsContainer.addComponent<Collider2D>(entity, std::move(collider));
    ecsContainer.addComponent<StaticSprite>(entity, std::move(sprite));
    ecsContainer.addComponent<Transform2D>(entity);
    ecsContainer.addComponent<Physics2D>(entity)->setStatic(true);
    ecsContainer.addComponent<Controller2D>(entity);
    return entity;
}
Entity PrefabFactory::createStaticSprite(ecs::EcsContainer& ecsContainer,
                                         TextureAtlas const& atlas,
                                         std::string frameName, scalar_t width,
                                         scalar_t height,
                                         bool preserveAspectRatio) {
    Entity entity = ecsContainer.createEntity();
    StaticSprite sprite(atlas.getTexture(), atlas.getFrame(frameName), width,
                        height, preserveAspectRatio);
    Collider2D collider;
    collider.add<BoxCollider>(width, height, ColliderType::PHYSICS);
    ecsContainer.addComponent<Collider2D>(entity, std::move(collider));
    ecsContainer.addComponent<StaticSprite>(entity, std::move(sprite));
    ecsContainer.addComponent<Transform2D>(entity);
    ecsContainer.addComponent<Physics2D>(entity);
    ecsContainer.addComponent<Controller2D>(entity);
    return entity;
}

Entity PrefabFactory::createAnimatedSprite(ecs::EcsContainer& ecsContainer,
                                           TextureAtlas const& atlas,
                                           scalar_t width, scalar_t height) {
    Entity entity = ecsContainer.createEntity();
    AnimatedSprite sprite(AnimationFactory::createBasicAnimation(),
                          atlas.getTexture(), width, height);
    Collider2D collider;
    HealthBar healthBar(1000, height);
    collider.add<BoxCollider>(width, height, ColliderType::PHYSICS);
    sprite.setFps(10);
    auto& atlasName = atlas.getName();

    if (atlasName.find("numbers") != std::string::npos) {
        sprite.setFramesFor<animation::IDLE>(
            atlas.getFrame("0"), atlas.getFrame("1"), atlas.getFrame("2"),
            atlas.getFrame("3"), atlas.getFrame("4"), atlas.getFrame("5"),
            atlas.getFrame("6"), atlas.getFrame("7"), atlas.getFrame("8"));
    } else if (atlasName == "stickman") {
        sprite.setFramesFor<animation::IDLE>(
            atlas.getFrame("idle0"), atlas.getFrame("idle1"),
            atlas.getFrame("idle2"), atlas.getFrame("idle3"),
            atlas.getFrame("idle4"), atlas.getFrame("idle5"));
        sprite.setFramesFor<animation::WALK>(
            atlas.getFrame("walk0"), atlas.getFrame("walk1"),
            atlas.getFrame("walk2"), atlas.getFrame("walk3"),
            atlas.getFrame("walk4"), atlas.getFrame("walk5"));
    } else if (atlasName.find("runner") != std::string::npos) {
        sprite.setFramesFor<animation::IDLE>(atlas.getFrame("idle0"));
        sprite.setFramesFor<animation::WALK>(
            atlas.getFrame("walk0"), atlas.getFrame("walk1"),
            atlas.getFrame("walk2"), atlas.getFrame("walk3"),
            atlas.getFrame("walk4"), atlas.getFrame("walk5"),
            atlas.getFrame("walk6"), atlas.getFrame("walk7"),
            atlas.getFrame("walk8"), atlas.getFrame("walk9"),
            atlas.getFrame("walk10"));
        sprite.setFramesFor<animation::RUN>(
            atlas.getFrame("run0"), atlas.getFrame("run1"),
            atlas.getFrame("run2"), atlas.getFrame("run3"),
            atlas.getFrame("run4"), atlas.getFrame("run5"),
            atlas.getFrame("run6"), atlas.getFrame("run7"),
            atlas.getFrame("run8"), atlas.getFrame("run9"));
    } else if (atlasName.find("wizard1") != std::string::npos) {
        sprite.setFramesFor<animation::IDLE>(
            atlas.getFrame("Idle0"), atlas.getFrame("Idle1"),
            atlas.getFrame("Idle2"), atlas.getFrame("Idle3"),
            atlas.getFrame("Idle4"), atlas.getFrame("Idle5"),
            atlas.getFrame("Idle6"), atlas.getFrame("Idle7"));
        sprite.setFramesFor<animation::WALK>(
            atlas.getFrame("Move0"), atlas.getFrame("Move1"),
            atlas.getFrame("Move2"), atlas.getFrame("Move3"),
            atlas.getFrame("Move4"), atlas.getFrame("Move5"),
            atlas.getFrame("Move6"), atlas.getFrame("Move7"));
        sprite.setFramesFor<animation::ATTACK_0>(
            atlas.getFrame("Attack0"), atlas.getFrame("Attack1"),
            atlas.getFrame("Attack2"), atlas.getFrame("Attack3"),
            atlas.getFrame("Attack4"), atlas.getFrame("Attack5"),
            atlas.getFrame("Attack6"), atlas.getFrame("Attack7"));
    } else if (atlasName == "wizard2") {
        sprite.setFramesFor<animation::IDLE>(
            atlas.getFrame("Idle0"), atlas.getFrame("Idle1"),
            atlas.getFrame("Idle2"), atlas.getFrame("Idle3"),
            atlas.getFrame("Idle4"), atlas.getFrame("Idle5"),
            atlas.getFrame("Idle6"), atlas.getFrame("Idle7"));
        sprite.setFramesFor<animation::WALK>(
            atlas.getFrame("Run0"), atlas.getFrame("Run1"),
            atlas.getFrame("Run2"), atlas.getFrame("Run3"),
            atlas.getFrame("Run4"), atlas.getFrame("Run5"),
            atlas.getFrame("Run6"), atlas.getFrame("Run7"));
        sprite.setFramesFor<animation::ATTACK_0>(
            atlas.getFrame("Attack10"), atlas.getFrame("Attack11"),
            atlas.getFrame("Attack12"), atlas.getFrame("Attack13"),
            atlas.getFrame("Attack14"), atlas.getFrame("Attack15"),
            atlas.getFrame("Attack16"), atlas.getFrame("Attack17"));
        sprite.setFramesFor<animation::ATTACK_1>(
            atlas.getFrame("Attack20"), atlas.getFrame("Attack21"),
            atlas.getFrame("Attack22"), atlas.getFrame("Attack23"),
            atlas.getFrame("Attack24"), atlas.getFrame("Attack25"),
            atlas.getFrame("Attack26"), atlas.getFrame("Attack27"));
    }
    sprite.removeUnusedStates();

    auto flipWhenMovingLeft = [](ecs::EcsContainer& ecsContainer, Entity entity,
                                 int frame) {
        auto* physics = ecsContainer.getComponent<Physics2D>(entity);
        auto* transform = ecsContainer.getComponent<Transform2D>(entity);
        if (physics && transform && physics->getLinearVelocity()[0] != 0) {
            if (physics->getLinearVelocity()[0] < 0) {
                transform->setFlipY(true);
            } else {
                transform->setFlipY(false);
            }
        }
    };

    sprite.getFiniteStateMachine().onState<animation::WALK>(flipWhenMovingLeft);
    sprite.getFiniteStateMachine().onState<animation::RUN>(flipWhenMovingLeft);

    if (atlasName == "wizard2") {
        float h = 3;
        collider.add<BoxCollider>(width / 2, height / h, ColliderType::PHYSICS,
                                  Vec2(0, -1.0f / h));
        collider.add<BoxCollider>(width / 2, height / h, ColliderType::HITBOX,
                                  Vec2(0.6f, 0.01f));
        collider.add<BoxCollider>(width / 2, height / h, ColliderType::HITBOX,
                                  Vec2(0.5f, -0.25f));
        collider.setActive(1, false);
        collider.setActive(2, false);

        auto hitForRandomDmg = [](ecs::EcsContainer& ecsContainer,
                                  Entity entity) {
            auto* hp = ecsContainer.getComponent<HealthBar>(entity);
            if (hp) {
                auto dmg = utils::RandomMatrix<scalar_t>::instance().getScalar(
                    10, 300);
                hp->changeHp(-dmg);
                std::cout << "Hit an enemy with entity " << entity.getId()
                          << " for " << dmg << "\n";
            } else {
                std::cout << "Entity does not have hp bar\n";
            }
        };
        collider.setHitboxCallback(1, hitForRandomDmg);
        collider.setHitboxCallback(2, hitForRandomDmg);

        sprite.getFiniteStateMachine().onState<animation::ATTACK_0>(
            [](ecs::EcsContainer& ecsContainer, ecs::Entity entity, int frame) {
                if (frame == 3) {
                    ecsContainer.getComponent<Collider2D>(entity)->setActive(
                        2, true);
                } else if (frame == 4) {
                    ecsContainer.getComponent<Collider2D>(entity)->setActive(
                        1, true);
                    ecsContainer.getComponent<Collider2D>(entity)->setActive(
                        2, false);
                } else if (frame == 5) {
                    ecsContainer.getComponent<Collider2D>(entity)->setActive(
                        1, false);
                }
            });
        sprite.getFiniteStateMachine().onState<animation::ATTACK_1>(
            [](ecs::EcsContainer& ecsContainer, ecs::Entity entity, int frame) {
                if (frame == 4) {
                    ecsContainer.getComponent<Collider2D>(entity)->setActive(
                        1, true);
                } else if (frame == 5) {
                    ecsContainer.getComponent<Collider2D>(entity)->setActive(
                        2, true);
                    ecsContainer.getComponent<Collider2D>(entity)->setActive(
                        1, false);
                } else if (frame == 6) {
                    ecsContainer.getComponent<Collider2D>(entity)->setActive(
                        2, false);
                }
            });
        sprite.getFiniteStateMachine().onChange<animation::ATTACK_0>(
            [](ecs::EcsContainer& ecsContainer, ecs::Entity entity, int frame) {
                ecsContainer.getComponent<Collider2D>(entity)->setActive(1,
                                                                         false);
                ecsContainer.getComponent<Collider2D>(entity)->setActive(2,
                                                                         false);
            });
        sprite.getFiniteStateMachine().onChange<animation::ATTACK_1>(
            [](ecs::EcsContainer& ecsContainer, ecs::Entity entity, int frame) {
                ecsContainer.getComponent<Collider2D>(entity)->setActive(1,
                                                                         false);
                ecsContainer.getComponent<Collider2D>(entity)->setActive(2,
                                                                         false);
            });
        sprite.onEnd<animation::ATTACK_0>([](ecs::EcsContainer& ecsContainer,
                                             ecs::Entity entity,
                                             int playCount) {
            ecsContainer.getComponent<Collider2D>(entity)->clearHitboxTargets();
        });
        sprite.onEnd<animation::ATTACK_1>([](ecs::EcsContainer& ecsContainer,
                                             ecs::Entity entity,
                                             int playCount) {
            ecsContainer.getComponent<Collider2D>(entity)->clearHitboxTargets();
        });
    } else if (atlasName == "numbers2") {
        collider.add<BoxCollider>(width / 2, height / 2, ColliderType::HITBOX,
                                  Vec2(0.5f, 0.5f));
        sprite.getFiniteStateMachine().onState<animation::IDLE>(
            [](ecs::EcsContainer& ecsContainer, ecs::Entity entity, int frame) {
                if (frame == 3) {
                    // std::cout <<"Frame 3!\n";
                    ecsContainer.getComponent<Collider2D>(entity)->setActive(
                        1, true);
                } else {
                    ecsContainer.getComponent<Collider2D>(entity)->setActive(
                        1, false);
                }
            });
    }

    ecsContainer.addComponent<Collider2D>(entity, std::move(collider));
    ecsContainer.addComponent<Transform2D>(entity);
    ecsContainer.addComponent<Physics2D>(entity);
    ecsContainer.addComponent<Controller2D>(entity);
    ecsContainer.addComponent<AnimatedSprite>(entity, std::move(sprite));
    ecsContainer.addComponent<HealthBar>(entity, std::move(healthBar));
    return entity;
}
}  // namespace ecs
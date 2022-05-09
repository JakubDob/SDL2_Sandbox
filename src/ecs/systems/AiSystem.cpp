#include "AiSystem.h"

namespace ecs {
void AiSystem::addBehavior(std::function<void(float)> behavior) {
    behaviors.emplace_back(behavior);
}
void AiSystem::update(float dt) {
    for (auto const& b : behaviors) {
        b(dt);
    }
}
}  // namespace ecs
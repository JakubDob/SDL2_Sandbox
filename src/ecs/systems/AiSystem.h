#pragma once
#include <functional>
#include <vector>

namespace ecs {
class AiSystem {
   public:
    void addBehavior(std::function<void(float)> behavior);
    void update(float dt);

   private:
    std::vector<std::function<void(float)>> behaviors;
};
}  // namespace ecs
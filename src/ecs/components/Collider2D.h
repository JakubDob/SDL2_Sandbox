#pragma once
#include "../../Types.h"
#include "../../CircularBuffer.h"
#include "../../EngineConstants.h"
#include "BaseCollider2D.h"
#include "../../Utils.h"
#include "../EcsContainer.h"
#include "../EcsComponentList.h"
#include <vector>
#include <memory>

struct CollisionHistory {
    CollisionHistory() : entity({}), timeStamp({}) {}
    CollisionHistory(ecs::Entity const& entity)
        : entity(entity), timeStamp({}) {}
    ecs::Entity entity;
    utils::TimeStamp timeStamp;
};

class Collider2D : public ecs::ComponentBase<Collider2D, ecs::Collider2DTag,
                                             ecs::ComponentTags> {
   public:
#if HAS_CONCEPTS
    template <typename T, typename... Args>
    requires std::derived_from<T, BaseCollider2D>
#else
    template <typename T, typename... Args>
#endif
    int add(Args&&... args) {
        colliders.emplace_back(
            std::make_unique<T>(std::forward<Args>(args)...));
        return colliders.size() - 1;
    }
    // scaling needed to update radius in circle collider
    void update(Mat4 const& modelToWorld, Mat2 const& modelNormalsRotation,
                scalar_t scaleFactor = 1) const;
    inline auto const& getColliders() const { return colliders; }
    inline void setActive(int id, bool value) {
        colliders[id]->setActive(value);
    }
    inline bool isActive(int id) const { return colliders[id]->isActive(); }
    inline void setHitboxCallback(int colliderId,
                                  BaseCollider2D::CallbackType fn) {
        colliders[colliderId]->setHitboxCallback(fn);
    }
    bool setCurrentHitboxTarget(
        ecs::Entity const&
            target);  // returns true if successfuly added new target
    void clearHitboxTargets();
    void translateInWorldSpace(Vec2 const& position);
    Vec4 getBoundingBox() const;

   private:
    std::array<ecs::Entity, 32> currentHitboxTargets{};
    std::vector<std::unique_ptr<BaseCollider2D>> colliders;
    int hitboxI{};
};
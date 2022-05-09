#pragma once
#include <vector>
#include <functional>
#include "../../Types.h"
#include "../EcsContainer.h"

struct ColliderData {
    ColliderData(Vec4 const& vertex, Vec2 const& normal)
        : vertex(vertex), normal(normal) {}
    ColliderData() {}
    Vec4 vertex;
    Vec2 normal;
};
class BaseCollider2D {
   public:
    using CallbackType = std::function<void(ecs::EcsContainer&, ecs::Entity)>;
    void update(Mat4 const& modelToWorld, Mat2 const& normalsRotation,
                scalar_t scaleFactor);
    inline auto const& getWorldSpaceData() const {
        return this->worldSpaceData;
    }
    inline auto const& getModelSpaceData() const {
        return this->modelSpaceData;
    }
    inline auto const& getWorldSpacePosition() const {
        return this->worldSpacePosition;
    }
    void translateInWorldSpace(Vec2 const& position);
    inline Shape getShape() const { return shape; }
    inline ColliderType getType() const { return type; }
    inline scalar_t getRadius() const { return radius; }
    inline bool isActive() const { return active; }
    inline void setActive(bool value) { active = value; }
    void notifyHitbox(ecs::EcsContainer& ecsContainer,
                      ecs::Entity const& other) const;
    void setHitboxCallback(CallbackType fn);
    virtual ~BaseCollider2D() {}

   protected:
    BaseCollider2D(Vec2 const& center, ColliderType type,
                   Shape shape = Shape::POLYGON, scalar_t radius = 0)
        : localSpacePosition(Vec4(center, {0, 1})),
          type(type),
          shape(shape),
          initialRadius(radius) {}
    void addModelSpaceData(ColliderData&& data);

   private:
    // position is needed to calc the correct orientation of the minimum
    // translation vector during collision detection
    std::vector<ColliderData> modelSpaceData{};
    std::vector<ColliderData> worldSpaceData{};
    Vec4 localSpacePosition{};
    Vec4 worldSpacePosition{};
    CallbackType hitboxCallback = nullptr;
    Shape shape = Shape::POLYGON;
    ColliderType type = ColliderType::PHYSICS;
    scalar_t radius{};
    scalar_t initialRadius{};
    bool active = true;
};
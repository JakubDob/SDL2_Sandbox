#pragma once
#include "../components/Collider2D.h"
#include "Renderer.h"
#include <set>

class Transform2D;
class Physics2D;
namespace ecs {

struct MinimumTranslation {
    Vec2 normal;
    scalar_t magnitude = 0;
};

struct CollisionData {
    CollisionData(BaseCollider2D const& cA, BaseCollider2D const& cB,
                  Entity const& a, Entity const& b,
                  MinimumTranslation const& mtv)
        : cA(cA), cB(cB), a(a), b(b), mtv(mtv) {}
    BaseCollider2D const& cA;
    BaseCollider2D const& cB;
    Entity a;
    Entity b;
    MinimumTranslation mtv;
};
struct DetectionData {
    Entity entity;
    int gridCellIndex;
};
struct ForceAverage {
    void addForce(Vec2 const& f) {
        force += f;
        ++count;
        maxForce = std::max(f.magnitude(), maxForce);
    }
    void clampForce() {
        if (force.magnitude() > maxForce) {
            force.normalize();
            force *= maxForce;
        }
    }
    void clampTranslation() {
        if (translation.magnitude() > maxTranslation) {
            translation.normalize();
            translation *= maxTranslation;
        }
    }
    void addTranslation(Vec2 const& t) {
        translation += t;
        maxTranslation = std::max(t.magnitude(), maxTranslation);
    }
    Vec2 translation;
    Vec2 force;
    Transform2D* transform = nullptr;
    Physics2D* physics = nullptr;
    float count = 0;
    float maxTranslation = 0;
    float maxForce = 0;
};
class CollisionSystem2D {
   public:
    CollisionSystem2D(Vec2 worldSize);
    void checkCollisions(ecs::EcsContainer& ecsContainer, scalar_t dt);
    bool areColliding(ecs::EcsContainer& ecsContainer, Entity a, Entity b,
                      MinimumTranslation& outMtv) const;
    bool areColliding(ecs::EcsContainer& ecsContainer, Collider2D const& a,
                      Collider2D const& b, MinimumTranslation& outMtv) const;
    void renderBoundingBoxes(ecs::EcsContainer& ecsContainer,
                             Renderer& renderer, Shader const& shader);

   private:
    using pair_t = std::pair<Entity, Entity>;
    struct SetCmp {
        bool operator()(pair_t a, pair_t b) const {
            if (a.first > a.second) a = pair_t{a.second, a.first};
            if (b.first > b.second) b = pair_t{b.second, b.first};
            return a < b;
        }
    };
    void broadPhase(ecs::EcsContainer& ecsContainer);
    void addDetectionData(Entity const& entity, Vec4 const& boundingBox);
    Vec4 findClosestVertexToPoint(
        Vec4 const& point,
        std::vector<ColliderData> const& worldSpaceData) const;
    void findMinMaxProjectionPolygon(
        std::vector<ColliderData> const& worldSpaceData, Vec2 const& normal,
        scalar_t& inOutMin, scalar_t& inOutMax) const;
    void findMinMaxProjectionCircle(Vec4 const& center, scalar_t radius,
                                    Vec2 const& normal, scalar_t& inOutMin,
                                    scalar_t& inOutMax) const;
    void resolveCollision(ecs::EcsContainer& ecsContainer,
                          CollisionData const& collision, scalar_t dt);
    bool polygonPolygon(BaseCollider2D const& c1, BaseCollider2D const& c2,
                        MinimumTranslation& out) const;
    bool polygonCircle(BaseCollider2D const& c1, BaseCollider2D const& c2,
                       MinimumTranslation& out) const;
    bool circleCircle(BaseCollider2D const& c1, BaseCollider2D const& c2,
                      MinimumTranslation& out) const;
    void resolveAllCollisions(ecs::EcsContainer& ecsContainer, scalar_t dt);
    bool linesIntersect(Vec4 const& a, Vec4 const& b, Vec4 const& c,
                        Vec4 const& d, Vec4& outIntersectPoint) const;
    // returns 0 if parallel (0 or 180 deg)
    inline scalar_t cross2DAnalog(Vec4 const& v1, Vec4 const& v2) const {
        return v1[0] * v2[1] - v1[1] * v2[0];
    }
    std::vector<DetectionData> detections;
    std::set<pair_t, SetCmp> potentialCollisions;
    std::vector<ForceAverage> forces;
    Vec2 worldSize;
    Vec2 cellSize;
    scalar_t totalRows;
    scalar_t totalCols;
};
}  // namespace ecs
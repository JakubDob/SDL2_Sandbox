#pragma once
#include "../../Types.h"
#include "../EcsContainer.h"
#include "../EcsComponentList.h"

class Physics2D : public ecs::ComponentBase<Physics2D, ecs::Physics2DTag,
                                            ecs::ComponentTags> {
   public:
    inline void setMass(scalar_t mass) { this->mass = mass; }
    inline auto getMass() const { return mass; }
    inline auto getFriction() const { return friction; }
    inline void setFriction(scalar_t value) {
        if (value > 1) value = 1;
        if (value < 0) value = 0;
        friction = value;
    }
    inline void setGrounded(bool value) {
        atRest = value;
        restVelocity = 0;
    }
    inline bool isGrounded() const { return atRest; }
    inline void setPartiallyStatic(bool value) { partiallyStatic = value; }
    inline bool isPartiallyStatic() const { return partiallyStatic; }
    inline void applyForce(Vec2 force) { this->linearVelocity += force; }
    inline auto getLinearVelocity() const { return this->linearVelocity; }
    inline void setLinearVelocity(Vec2 velocity) {
        this->linearVelocity = velocity;
    }
    inline void setLinearVelocityX(scalar_t velocity) {
        this->linearVelocity[0] = velocity;
    }
    inline void setLinearVelocityY(scalar_t velocity) {
        this->linearVelocity[1] = velocity;
    }
    inline void setAngularVelocity(scalar_t angularVelocity) {
        this->angularVelocity = angularVelocity;
    }
    inline auto getAngularVelocity() const { return this->angularVelocity; }
    inline void scaleLinearVelocity(scalar_t scale) { linearVelocity *= scale; }
    inline void scaleLinearVelocityX(scalar_t scale) {
        linearVelocity[0] *= scale;
    }
    inline void scaleLinearVelocityY(scalar_t scale) {
        linearVelocity[1] *= scale;
    }
    inline bool isStatic() const { return staticObj; }
    inline void setStatic(bool value) { staticObj = value; }
    inline void reflectLinearVelocity(Vec2 const& normal) {
        linearVelocity =
            linearVelocity - 2 * math::dot(linearVelocity, normal) * normal;
    }
    inline Vec2 reflect(Vec2 const& target, Vec2 const& normal) const {
        return target - 2 * math::dot(target, normal) * normal;
    }
    inline void applyEqualOppositeForce(Vec2 const& normal) {
        linearVelocity += -math::dot(linearVelocity, normal) * normal;
    }
    inline Vec2 getEqualOppositeForce(Vec2 const& normal) {
        return -math::dot(linearVelocity, normal) * normal;
    }
    inline scalar_t getSpeed() const { return linearVelocity.magnitude(); }
    inline scalar_t getRestVelocity() { return restVelocity; }
    inline void addToRestVelocity(scalar_t value) { restVelocity += value; }
    void zeroLinearVelocity(scalar_t epsilon);
    void elasticCollision(Physics2D& other);
    static void elasticCollision(Physics2D& a, Physics2D& b);
    static void inelasticCollision(Physics2D& a, Physics2D& b);
    static Vec2 impulse(Physics2D const& a, Physics2D const& b,
                        Vec2 const& normal, scalar_t restitution);
    inline static void swapLinearVelocity(Physics2D& a, Physics2D& b) {
        auto v = b.linearVelocity;
        b.linearVelocity = a.linearVelocity;
        a.linearVelocity = v;
    }

   private:
    Vec2 linearVelocity{};
    scalar_t angularVelocity = 0;
    scalar_t mass = 1;
    scalar_t restVelocity = 0;
    scalar_t friction = 0.01;  // 0 = no friction, 1 = infinite
    bool staticObj = false;
    bool atRest = false;
    bool partiallyStatic =
        false;  // forces applied only when interacting with a static object. In
                // collision with non static object, does not recive an impulse.
};
#include "Physics2D.h"

void Physics2D::elasticCollision(Physics2D& a, Physics2D& b) {
    auto vel1 = a.linearVelocity;
    auto vel2 = b.linearVelocity;
    auto m1 = a.mass;
    auto m2 = b.mass;
    a.linearVelocity =
        (m1 - m2) / (m1 + m2) * vel1 + (2 * m2 / (m2 + m1)) * vel2;
    b.linearVelocity =
        (2 * m1) / (m1 + m2) * vel1 + ((m2 - m1) / (m2 + m1)) * vel2;
}
void Physics2D::inelasticCollision(Physics2D& a, Physics2D& b) {
    auto vel1 = a.linearVelocity;
    auto vel2 = b.linearVelocity;
    auto m1 = a.mass;
    auto m2 = b.mass;
    auto finalVel = ((m1 * vel1) + (m2 * vel2)) / (m1 + m2);
    a.linearVelocity = finalVel;
    b.linearVelocity = finalVel;
}
void Physics2D::zeroLinearVelocity(scalar_t epsilon) {
    if (std::abs(linearVelocity[0]) < epsilon) {
        linearVelocity[0] = 0;
    }
    if (std::abs(linearVelocity[1]) < epsilon) {
        linearVelocity[1] = 0;
    }
}
Vec2 Physics2D::impulse(Physics2D const& a, Physics2D const& b,
                        Vec2 const& normal, scalar_t restitution) {
    Vec2 relativeVel = a.getLinearVelocity() - b.getLinearVelocity();
    scalar_t j = (-(1.0f + restitution) * (math::dot(relativeVel, normal)));
    return j * normal;
}
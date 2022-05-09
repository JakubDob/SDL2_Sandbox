#include "Collider2D.h"
#include <algorithm>

void Collider2D::update(Mat4 const& modelToWorld, Mat2 const& normalsRotation,
                        scalar_t scaleFactor) const {
    for (int i = 0; i < colliders.size(); ++i) {
        colliders[i]->update(modelToWorld, normalsRotation, scaleFactor);
    }
}

bool Collider2D::setCurrentHitboxTarget(ecs::Entity const& target) {
    for (int i = 0; i < hitboxI; ++i) {
        if (currentHitboxTargets[i] == target) {
            return false;
        }
    }
    currentHitboxTargets[hitboxI++] = target;
    if (hitboxI == currentHitboxTargets.size()) {
        hitboxI = 0;
    }
    return true;
}
void Collider2D::translateInWorldSpace(Vec2 const& pos) {
    for (auto& c : colliders) {
        c->translateInWorldSpace(pos);
    }
}

void Collider2D::clearHitboxTargets() {
    currentHitboxTargets.fill({});
    hitboxI = 0;
}

Vec4 Collider2D::getBoundingBox() const {
    scalar_t minX = std::numeric_limits<scalar_t>::max();
    scalar_t maxX = -minX;
    scalar_t minY = minX;
    scalar_t maxY = maxX;

    for (auto const& collider : colliders) {
        if (!collider->isActive()) {
            continue;
        }
        auto const& data = collider->getWorldSpaceData();
        for (auto const& c : data) {
            if (c.vertex[0] < minX) {
                minX = c.vertex[0];
            }
            if (c.vertex[0] > maxX) {
                maxX = c.vertex[0];
            }
            if (c.vertex[1] < minY) {
                minY = c.vertex[1];
            }
            if (c.vertex[1] > maxY) {
                maxY = c.vertex[1];
            }
        }
    }
    // top left corner, width, height
    return Vec4(minX, maxY, maxX - minX, maxY - minY);
}
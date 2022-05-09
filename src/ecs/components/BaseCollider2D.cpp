#include "BaseCollider2D.h"

void BaseCollider2D::update(Mat4 const& modelToWorld,
                            Mat2 const& normalsRotation, scalar_t scaleFactor) {
    for (int i = 0; i < modelSpaceData.size(); ++i) {
        worldSpaceData[i].vertex = modelToWorld * modelSpaceData[i].vertex;
        worldSpaceData[i].normal = normalsRotation * modelSpaceData[i].normal;
    }
    worldSpacePosition = modelToWorld * localSpacePosition;
    radius = initialRadius * scaleFactor;
}

void BaseCollider2D::addModelSpaceData(ColliderData&& data) {
    this->worldSpaceData.push_back({});
    this->modelSpaceData.emplace_back(std::move(data));
}

void BaseCollider2D::setHitboxCallback(CallbackType fn) { hitboxCallback = fn; }

void BaseCollider2D::notifyHitbox(ecs::EcsContainer& ecsContainer,
                                  ecs::Entity const& other) const {
    if (hitboxCallback) hitboxCallback(ecsContainer, other);
}

void BaseCollider2D::translateInWorldSpace(Vec2 const& position) {
    worldSpacePosition[0] += position[0];
    worldSpacePosition[1] += position[1];
}
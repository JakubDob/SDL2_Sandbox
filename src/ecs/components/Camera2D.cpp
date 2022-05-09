#include "Camera2D.h"
#include "../../Utils.h"
#include "../../EngineConstants.h"

Camera2D::Camera2D() { transform.setPosition({0, 0}); }
void Camera2D::zoomOut(scalar_t factor) {
    this->zoomFactor = 1 / factor;
    shouldUpdateView = true;
}
void Camera2D::zoomIn(scalar_t factor) {
    this->zoomFactor = factor;
    shouldUpdateView = true;
}
void Camera2D::resetZoom() {
    this->zoomFactor = 1;
    shouldUpdateView = true;
}
void Camera2D::setZoom(scalar_t value) {
    zoomFactor = value;
    shouldUpdateView = true;
}
void Camera2D::scaleZoom(scalar_t value) {
    zoomFactor *= value;
    shouldUpdateView = true;
}
Mat4 const& Camera2D::worldToView() {
    if (shouldUpdateView) {
        updateView();
    }
    return view;
}
void Camera2D::updateView() {
    auto up = transform.up();
    auto right = utils::clockwiseNormal(up);
    auto pos = transform.getPosition();

    Mat4 inverseCamBase = math::transpose(Mat4(right, up));
    inverseCamBase(2, 2) = 1;  // depth
    inverseCamBase(3, 3) = 1;

    Mat4 inverseTranslate(
        {1, 0, 0, -pos[0], 0, 1, 0, -pos[1], 0, 0, 1, 0, 0, 0, 0, 1});
    Mat4 zoom = math::getScalingMatrix(zoomFactor);
    view = inverseCamBase * zoom * inverseTranslate;
    shouldUpdateView = false;
}

void Camera2D::updatePosition(ecs::EcsContainer& ecsContainer) {
    if (ecsContainer.exists(target)) {
        auto targetPos =
            ecsContainer.getComponent<Transform2D>(target)->getPosition();
        if (transform.getPosition() != targetPos) {
            transform.setPosition(targetPos);
            shouldUpdateView = true;
        }
    }
}
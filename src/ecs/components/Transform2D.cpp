#include "Transform2D.h"

void Transform2D::updateModelMatrix() {
    this->modelMatrix = math::getTranslationMatrix(this->position) *
                        math::getRotationMatrix(this->rotationAngle) *
                        math::getScalingMatrix(this->scaleFactor);
    if (shouldFlipY) {
        math::negateX(modelMatrix);
    }
    this->modelMatrix(2, 2) = depth;
    shouldUpdateModelMatrix = false;
}
void Transform2D::updateNormalsMatrix() {
    this->normalsMatrix = math::get2x2RotationMatrix2D(this->rotationAngle);
}
void Transform2D::translate(Displacement2D const& displacement) {
    this->position += displacement;
    this->shouldUpdateModelMatrix = true;
}
void Transform2D::rotate(DegreeAngle const angle) {
    this->rotationAngle += angle;
    this->shouldUpdateModelMatrix = true;
    this->shouldUpdateNormalsMatrix = true;
}
void Transform2D::scale(scalar_t const scale) {
    this->scaleFactor += scale;
    this->shouldUpdateModelMatrix = true;
}
Vec2 Transform2D::up() const {
    auto m = math::getRotationMatrix(this->rotationAngle);
    Vec2 y(m(0, 1), m(1, 1));
    return y.normalize();
}
Vec2 Transform2D::right() const {
    auto m = math::getRotationMatrix(this->rotationAngle);
    Vec2 x(m(0, 0), m(0, 1));
    return x.normalize();
}

Mat4 Transform2D::getScalingMatrix() const {
    return math::getScalingMatrix(this->scaleFactor);
}
Mat4 Transform2D::getRotationMatrix() const {
    return math::getRotationMatrix(this->rotationAngle);
};
Mat4 Transform2D::getTranslationMatrix() const {
    return math::getTranslationMatrix(this->position);
};
DegreeAngle Transform2D::getRotationAngle() const {
    return this->rotationAngle;
}

Mat4 const& Transform2D::modelToWorld() {
    if (shouldUpdateModelMatrix) {
        updateModelMatrix();
    }
    return modelMatrix;
}

Mat2 const& Transform2D::normalsRotation() {
    if (shouldUpdateNormalsMatrix) {
        updateNormalsMatrix();
    }
    return normalsMatrix;
}

void Transform2D::setNdcDepth(scalar_t depth) {
    if (depth > 0.99f) {
        depth = 0.99f;
    }
    if (depth < -0.99f) {
        depth = -0.99f;
    }
    this->depth = depth;
}
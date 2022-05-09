#pragma once
#include "../../Types.h"
#include "../EcsContainer.h"
#include "../EcsComponentList.h"

class Transform2D : public ecs::ComponentBase<Transform2D, ecs::Transform2DTag,
                                              ecs::ComponentTags> {
   public:
    void translate(Displacement2D const& displacement);
    void rotate(DegreeAngle const angle);
    void scale(scalar_t const scale);
    inline void setFlipY(bool value) { shouldFlipY = value; };
    inline void flipY() { shouldFlipY = !shouldFlipY; }
    Mat4 const& modelToWorld();
    Mat2 const& normalsRotation();
    Mat4 getRotationMatrix() const;
    Mat4 getTranslationMatrix() const;
    Mat4 getScalingMatrix() const;
    DegreeAngle getRotationAngle() const;
    inline scalar_t getScaleFactor() const { return scaleFactor; }
    inline Position2D getPosition() const { return position; }
    inline void setPosition(Position2D const& position) {
        this->position = position;
    }
    inline void setX(scalar_t x) { this->position[0] = x; }
    inline void setY(scalar_t y) { this->position[1] = y; }
    inline scalar_t getX() { return this->position[0]; }
    inline scalar_t getY() { return this->position[1]; }
    void setNdcDepth(scalar_t depth);
    inline scalar_t getNdcDepth() const { return depth; }
    inline void subtractPosition(Position2D const& vec) {
        this->position -= vec;
    }
    /*returns the normalized green(Y) vector in the world space*/
    Vec2 up() const;
    /*returns the normalized red(X) vector in the world space*/
    Vec2 right() const;
    void updateModelMatrix();
    void updateNormalsMatrix();

   private:
    Mat4 modelMatrix;
    Mat2 normalsMatrix;
    Position2D position;
    DegreeAngle rotationAngle = 0;
    scalar_t scaleFactor = 1;
    scalar_t depth = 0;
    bool shouldUpdateModelMatrix = true;
    bool shouldUpdateNormalsMatrix = true;
    bool shouldFlipY = false;
};
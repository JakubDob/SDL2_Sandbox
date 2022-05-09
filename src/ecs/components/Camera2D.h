#pragma once
#include "Transform2D.h"
#include "../../Types.h"
#include "../EcsComponentList.h"

class Camera2D {
   public:
    Camera2D();
    Mat4 const& worldToView();
    Mat4 ortographicProjection(scalar_t left, scalar_t right, scalar_t bottom,
                               scalar_t top);
    inline scalar_t getZoom() const { return zoomFactor; };
    void zoomIn(scalar_t factor);
    void zoomOut(scalar_t factor);
    void setZoom(scalar_t value);
    void scaleZoom(scalar_t value);
    void resetZoom();
    void updateWorldToScreen();
    void updatePosition(ecs::EcsContainer& ecsContainer);
    void updateView();
    inline void attach(ecs::EcsContainer& ecsContainer, ecs::Entity target) {
        this->target = target;
        updatePosition(ecsContainer);
    }
    inline void detach() { this->target = {}; }
    inline ecs::Entity const getTarget() const { return target; }
    inline Transform2D& getTransform() { return transform; }

   private:
    Transform2D transform;
    Mat4 view;
    ecs::Entity target{};
    scalar_t zoomFactor = 1;
    bool shouldUpdateView = true;
};
#pragma once
#include "../../Types.h"
#include "../../opengl/Shader.h"
#include "../EcsContainer.h"
#include "../EcsComponentList.h"
#include "../../QuadMesh.h"

class HealthBar : public ecs::ComponentBase<HealthBar, ecs::HealthBarTag,
                                            ecs::ComponentTags> {
   public:
    HealthBar(scalar_t maxValue, float floatHeight);
    void changeHp(scalar_t amount);
    void reset();
    scalar_t getCurrentValue() const;
    scalar_t getMaxValue() const;
    QuadMesh const& getMesh(bool flipY);

   private:
    QuadMesh mesh;
    scalar_t maxValue;
    scalar_t currentValue;

    inline static float width = 1.0f;
    inline static float height = 0.2f;
};
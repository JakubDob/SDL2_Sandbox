#pragma once
#include "../EcsContainer.h"
#include "../EcsComponentList.h"
#include "../../Types.h"
#include "../../QuadMesh.h"
#include "../../TextureAtlasFrame.h"
#include <array>

class Texture;
namespace ecs {
class StaticSprite
    : public ComponentBase<StaticSprite, StaticSpriteTag, ComponentTags> {
   public:
    StaticSprite(Texture const& singleTexture, scalar_t width, scalar_t height);
    StaticSprite(Texture const& atlasTexture, TextureAtlasFrame const& frame,
                 scalar_t width, scalar_t height,
                 bool preserveAspectRatio = true);
    StaticSprite(Vec4 const& color, scalar_t width, scalar_t height);
    QuadMesh getMesh();  // copy to transform later
    Texture const* getTexture() const;
    bool isOpaque() const { return opaque; }
    inline void setOpaque(bool value) { opaque = value; }
    void setNormalMap(TextureAtlasFrame const& frame);
    void rotate(scalar_t degreeAngle);

   private:
    QuadMesh mesh;
    Texture const* texture = nullptr;
    bool opaque;
};
}  // namespace ecs

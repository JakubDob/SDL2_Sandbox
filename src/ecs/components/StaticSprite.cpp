#include "StaticSprite.h"

namespace ecs {
StaticSprite::StaticSprite(Texture const& atlasTexture,
                           TextureAtlasFrame const& frame, scalar_t width,
                           scalar_t height, bool preserveAspectRatio) {
    texture = &atlasTexture;
    mesh = frame.createMesh(TextureAtlasFrame::center, width, height,
                            preserveAspectRatio);
    mesh.setColor({1.0f, 1.0f, 1.0f, 1.0f});
    opaque = false;
}

StaticSprite::StaticSprite(Texture const& singleTexture, scalar_t width,
                           scalar_t height) {
    texture = &singleTexture;
    mesh = QuadMesh(QuadMesh::center, width, height);
    mesh.setColor({1.0f, 1.0f, 1.0f, 1.0f});
    opaque = false;
}

StaticSprite::StaticSprite(Vec4 const& color, scalar_t width, scalar_t height) {
    mesh = QuadMesh(QuadMesh::center, width, height);
    mesh.setColor(color);
    opaque = true;
}

QuadMesh StaticSprite::getMesh() { return mesh; }

Texture const* StaticSprite::getTexture() const { return texture; }

// Assumes that the frame belongs to the stored texture atlas
void StaticSprite::setNormalMap(TextureAtlasFrame const& frame) {
    mesh.setNormalTexCoords(frame.getTextureRect());
}
void StaticSprite::rotate(scalar_t degreeAngle) {
    mesh.transformPosition(math::getRotationMatrix(degreeAngle));
}
}  // namespace ecs
#include "TextureAtlasFrame.h"
TextureAtlasFrame::TextureAtlasFrame()
    : geoRect({}), texRect({}), trimmedSize({}) {}

TextureAtlasFrame::TextureAtlasFrame(GeometryRect const& geoRect,
                                     TextureRect const& texRect,
                                     Vec2 const& trimmedSize)
    : geoRect(geoRect), texRect(texRect), trimmedSize(trimmedSize) {}

/*
creates a mesh in the coordinate system shown below:
a = top left corner, b = bottom right corner
          ^
          |
      a   |
          |(0,0)
----------|------------->
          |
          |   b
          |
*/
/**
 * \param maxExtent defines maximum width or height. Preserves aspect ratio.
 * Value 0 = no scaling.
 */

QuadMesh TextureAtlasFrame::createMesh(Center, float width, float height,
                                       bool preserveAspectRatio) const {
    float topLeftX = geoRect[0] - 0.5f * geoRect[2];
    float topLeftY = -geoRect[1] + 0.5f * geoRect[3];
    GeometryRect geo{topLeftX, topLeftY, trimmedSize[0], trimmedSize[1]};
    QuadMesh mesh(QuadMesh::center, geo, texRect);

    if (width > 0 && height > 0) {
        auto originalScale = std::max(geoRect[2], geoRect[3]);
        if (preserveAspectRatio) {
            auto maxExtent = std::max(width, height);
            mesh.scale(maxExtent / originalScale, maxExtent / originalScale);
        } else {
            mesh.scale((width) / trimmedSize[0], (height) / trimmedSize[1]);
        }
    }
    return mesh;
}
/*
creates a mesh in the coordinate system shown below; reflected across x axis and
translated (+0.5 * width, +0.5 * height) a = top left corner, b = bottom right
corner (0,0)
|------------->
|a
|
|
|      b
|
\/
*/
/**
 * \param maxExtent defines maximum width or height. Preserves aspect ratio.
 * Value 0 = no scaling.
 */
QuadMesh TextureAtlasFrame::createMesh(TopLeft, float width, float height,
                                       bool preserveAspectRatio) const {
    QuadMesh mesh(QuadMesh::topLeft, geoRect, texRect);
    if (width > 0 && height > 0) {
        auto originalScale = std::max(geoRect[2], geoRect[3]);
        if (preserveAspectRatio) {
            auto maxExtent = std::max(width, height);
            mesh.scale(maxExtent / originalScale, maxExtent / originalScale);
        } else {
            // scale mesh such that the 'untrimmed image' is <width, height>
            float ratioX = trimmedSize[0] / geoRect[2];
            float ratioY = trimmedSize[1] / geoRect[3];
            mesh.scale((width * ratioX) / geoRect[2],
                       (height * ratioY) / geoRect[3]);
        }
    }
    return mesh;
}

Vec2 const& TextureAtlasFrame::getSize() const { return trimmedSize; }

scalar_t TextureAtlasFrame::getWidth() const { return trimmedSize[0]; }
scalar_t TextureAtlasFrame::getHeight() const { return trimmedSize[1]; }

TextureRect const& TextureAtlasFrame::getTextureRect() const { return texRect; }

GeometryRect const& TextureAtlasFrame::getGeometryRect() const {
    return geoRect;
}
#pragma once
#include "QuadMesh.h"
#include "Types.h"

class TextureAtlasFrame {
   public:
    inline static struct Center {
    } center;
    inline static struct TopLeft {
    } topLeft;
    TextureAtlasFrame();
    TextureAtlasFrame(GeometryRect const& geoRect, TextureRect const& texRect,
                      Vec2 const& trimmedSize);
    QuadMesh createMesh(Center, float width = 0, float height = 0,
                        bool preserveAspectRatio = true) const;
    QuadMesh createMesh(TopLeft, float width = 0, float height = 0,
                        bool preserveAspectRatio = true) const;
    Vec2 const& getSize() const;
    scalar_t getWidth() const;
    scalar_t getHeight() const;
    TextureRect const& getTextureRect() const;
    GeometryRect const& getGeometryRect() const;

   private:
    // position in model space; trimmed sprite's top left corner may not match
    // original sprite's top left corner, must save offsets for each frame
    GeometryRect geoRect;
    // normalized rectangular portion of atlas texture
    TextureRect texRect;
    // trimmed sprite's width and height may not match frame's size, so storing
    // this variable is necessary to preserve aspect ratio
    Vec2 trimmedSize;
};
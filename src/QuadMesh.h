#pragma once
#include "Types.h"

class QuadMesh;
class QuadVertex {
    friend class QuadMesh;

   public:
    inline static struct Center {
    } center;
    inline static struct TopLeft {
    } topLeft;
    void transformPosition(Mat4 const& matrix);
    void setDepth(float depth);
    void setPosition(Center, GeometryRect const& rect, VertexName vertexName);
    void setPosition(TopLeft, GeometryRect const& rect, VertexName vertexName);
    void setPosition(Center, GeometryRect const& rect, int vertexIndex);
    void setPosition(TopLeft, GeometryRect const& rect, int vertexIndex);
    void setTexCoord(TextureRect const& rect, VertexName vertexName);
    void setTexCoord(TextureRect const& rect, int vertexIndex);
    void setNormalTexCoord(TextureRect const& rect, VertexName vertexName);
    void setNormalTexCoord(TextureRect const& rect, int vertexIndex);
    void setPosition(float x, float y);
    void setTexCoord(float u, float v);
    void setColor(Vec4 const& color);
    void scale(float width, float height);
    float getRed() const;
    float getGreen() const;
    float getBlue() const;
    float getX() const;
    float getY() const;
    void setRed(float value);
    void setGreen(float value);
    void setBlue(float value);
    void setX(float value);
    void setY(float value);
    void flipY();
    void flipX();

   private:
    Vec2 position;
    Vec2 texCoord;
    Vec2 normalTexCoord{0, 0};  // color and normal can be in the same atlas, so
                                // they have 2 different coordinates
    Vec4 color{{1, 1, 1, 1}};
    float depth;
};

class QuadMesh {
   public:
    inline static struct Center {
    } center;
    inline static struct TopLeft {
    } topLeft;
    QuadMesh() = default;
    explicit QuadMesh(Center, GeometryRect const& rect);
    explicit QuadMesh(TopLeft, GeometryRect const& rect);
    QuadMesh(Center, GeometryRect const& geo, TextureRect const& tex,
             TextureRect const* normalTex = nullptr);
    QuadMesh(TopLeft, GeometryRect const& geo, TextureRect const& tex);
    QuadMesh(Center, float width, float height);
    QuadMesh(TopLeft, float width, float height);
    void transformPosition(Mat4 const& matrix);
    void setDepth(float depth);
    void setColor(Vec4 const& color);
    void scale(float width, float height);
    void translate(Vec2 const& pos);
    void flipY();
    void flipX();
    float getWidth() const;
    float getHeight() const;
    void resetTexCoords();  // discards aspect ratio and sets values to 0..1 ->
                            // texture adapts to the size of a primitive
    void setNormalTexCoords(TextureRect const& rect);
    QuadVertex& operator[](VertexName name);
    QuadVertex const& operator[](VertexName name) const;
    QuadVertex& operator[](size_t i);

   private:
    QuadVertex vertices[4];
};
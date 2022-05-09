#include "QuadMesh.h"

//--------------------------------QuadVertex----------------------------------------------------

void QuadVertex::transformPosition(Mat4 const& matrix) {
    Vec4 newPos(position, 0.f, 1.f);
    newPos = matrix * newPos;
    position[0] = newPos[0];
    position[1] = newPos[1];
}

void QuadVertex::setDepth(scalar_t depth) { this->depth = depth; }

void QuadVertex::setColor(Vec4 const& color) { this->color = color; }

void QuadVertex::setPosition(Center, GeometryRect const& rect,
                             VertexName vertexName) {
    switch (vertexName) {
        case VertexName::TOP_LEFT:
            position[0] = rect[0];
            position[1] = rect[1];
            break;
        case VertexName::TOP_RIGHT:
            position[0] = rect[0] + rect[2];
            position[1] = rect[1];
            break;
        case VertexName::BOTTOM_LEFT:
            position[0] = rect[0];
            position[1] = rect[1] - rect[3];
            break;
        case VertexName::BOTTOM_RIGHT:
            position[0] = rect[0] + rect[2];
            position[1] = rect[1] - rect[3];
    }
}

void QuadVertex::setPosition(TopLeft, GeometryRect const& rect,
                             VertexName vertexName) {
    switch (vertexName) {
        case VertexName::TOP_LEFT:
            position[0] = rect[0];
            position[1] = rect[1];
            break;
        case VertexName::TOP_RIGHT:
            position[0] = rect[0] + rect[2];
            position[1] = rect[1];
            break;
        case VertexName::BOTTOM_LEFT:
            position[0] = rect[0];
            position[1] =
                rect[1] + rect[3];  // width gets added compared to center
            break;
        case VertexName::BOTTOM_RIGHT:
            position[0] = rect[0] + rect[2];
            position[1] =
                rect[1] + rect[3];  // width gets added compared to center
    }
}
void QuadVertex::setTexCoord(TextureRect const& rect, VertexName vertexName) {
    switch (vertexName) {
        case VertexName::TOP_LEFT:
            texCoord[0] = rect[0];
            texCoord[1] = rect[1] + rect[3];
            break;
        case VertexName::TOP_RIGHT:
            texCoord[0] = rect[0] + rect[2];
            texCoord[1] = rect[1] + rect[3];
            break;
        case VertexName::BOTTOM_LEFT:
            texCoord[0] = rect[0];
            texCoord[1] = rect[1];
            break;
        case VertexName::BOTTOM_RIGHT:
            texCoord[0] = rect[0] + rect[2];
            texCoord[1] = rect[1];
    }
}
void QuadVertex::setTexCoord(TextureRect const& rect, int vertexIndex) {
    setTexCoord(rect, static_cast<VertexName>(vertexIndex));
}

void QuadVertex::setNormalTexCoord(TextureRect const& rect,
                                   VertexName vertexName) {
    switch (vertexName) {
        case VertexName::TOP_LEFT:
            normalTexCoord[0] = rect[0];
            normalTexCoord[1] = rect[1] + rect[3];
            break;
        case VertexName::TOP_RIGHT:
            normalTexCoord[0] = rect[0] + rect[2];
            normalTexCoord[1] = rect[1] + rect[3];
            break;
        case VertexName::BOTTOM_LEFT:
            normalTexCoord[0] = rect[0];
            normalTexCoord[1] = rect[1];
            break;
        case VertexName::BOTTOM_RIGHT:
            normalTexCoord[0] = rect[0] + rect[2];
            normalTexCoord[1] = rect[1];
    }
}

void QuadVertex::setNormalTexCoord(TextureRect const& rect, int vertexIndex) {
    setNormalTexCoord(rect, static_cast<VertexName>(vertexIndex));
}

void QuadVertex::setPosition(Center, GeometryRect const& rect,
                             int vertexIndex) {
    setPosition(Center{}, rect, static_cast<VertexName>(vertexIndex));
}
void QuadVertex::setPosition(TopLeft, GeometryRect const& rect,
                             int vertexIndex) {
    setPosition(TopLeft{}, rect, static_cast<VertexName>(vertexIndex));
}
void QuadVertex::setPosition(float x, float y) {
    position[0] = x;
    position[1] = y;
}

void QuadVertex::setTexCoord(float u, float v) {
    texCoord[0] = u;
    texCoord[1] = v;
}

void QuadVertex::scale(float width, float height) {
    position[0] *= width;
    position[1] *= height;
}

void QuadVertex::flipY() { position[0] *= -1.f; }
void QuadVertex::flipX() { position[1] *= -1.f; }
float QuadVertex::getRed() const { return color[0]; }
float QuadVertex::getGreen() const { return color[1]; }
float QuadVertex::getBlue() const { return color[2]; }
float QuadVertex::getX() const { return position[0]; }
float QuadVertex::getY() const { return position[1]; }
void QuadVertex::setRed(float value) { color[0] = value; }
void QuadVertex::setGreen(float value) { color[1] = value; }
void QuadVertex::setBlue(float value) { color[2] = value; }
void QuadVertex::setX(float value) { position[0] = value; }
void QuadVertex::setY(float value) { position[1] = value; }

//--------------------------------QuadMesh----------------------------------------------------
QuadMesh::QuadMesh(Center, GeometryRect const& rect) {
    for (int i = 0; i < 4; ++i) {
        vertices[i].setPosition(QuadVertex::center, rect, i);
    }
    auto aspectRatio = rect[2] / rect[3];
    this->operator[](VertexName::TOP_LEFT).setTexCoord(0, 1);
    this->operator[](VertexName::TOP_RIGHT).setTexCoord(aspectRatio, 1);
    this->operator[](VertexName::BOTTOM_LEFT).setTexCoord(0, 0);
    this->operator[](VertexName::BOTTOM_RIGHT).setTexCoord(aspectRatio, 0);
}

QuadMesh::QuadMesh(TopLeft, GeometryRect const& rect) {
    for (int i = 0; i < 4; ++i) {
        vertices[i].setPosition(QuadVertex::topLeft, rect, i);
    }
    auto aspectRatio = rect[2] / rect[3];
    this->operator[](VertexName::TOP_LEFT).setTexCoord(0, 1);
    this->operator[](VertexName::TOP_RIGHT).setTexCoord(aspectRatio, 1);
    this->operator[](VertexName::BOTTOM_LEFT).setTexCoord(0, 0);
    this->operator[](VertexName::BOTTOM_RIGHT).setTexCoord(aspectRatio, 0);
}

QuadMesh::QuadMesh(Center, float width, float height) {
    this->operator[](VertexName::TOP_LEFT).setPosition(-width / 2, height / 2);
    this->operator[](VertexName::TOP_LEFT).setTexCoord(0, 1);

    this->operator[](VertexName::TOP_RIGHT).setPosition(width / 2, height / 2);
    this->operator[](VertexName::TOP_RIGHT).setTexCoord(width / height, 1);

    this->operator[](VertexName::BOTTOM_LEFT)
        .setPosition(-width / 2, -height / 2);
    this->operator[](VertexName::BOTTOM_LEFT).setTexCoord(0, 0);

    this->operator[](VertexName::BOTTOM_RIGHT)
        .setPosition(width / 2, -height / 2);
    this->operator[](VertexName::BOTTOM_RIGHT).setTexCoord(width / height, 0);
}

QuadMesh::QuadMesh(TopLeft, float width, float height) {
    this->operator[](VertexName::TOP_LEFT).setPosition(0, 0);
    this->operator[](VertexName::TOP_LEFT).setTexCoord(0, 1);

    this->operator[](VertexName::TOP_RIGHT).setPosition(0, height);
    this->operator[](VertexName::TOP_RIGHT).setTexCoord(width / height, 1);

    this->operator[](VertexName::BOTTOM_LEFT).setPosition(0, height);
    this->operator[](VertexName::BOTTOM_LEFT).setTexCoord(0, 0);

    this->operator[](VertexName::BOTTOM_RIGHT).setPosition(width, height);
    this->operator[](VertexName::BOTTOM_RIGHT).setTexCoord(width / height, 0);
}

QuadMesh::QuadMesh(Center, GeometryRect const& geo, TextureRect const& tex,
                   TextureRect const* normalTex) {
    if (normalTex) {
        for (int i = 0; i < 4; ++i) {
            vertices[i].setPosition(QuadVertex::center, geo, i);
            vertices[i].setTexCoord(tex, i);
            vertices[i].setNormalTexCoord(*normalTex, i);
        }
    } else {
        for (int i = 0; i < 4; ++i) {
            vertices[i].setPosition(QuadVertex::center, geo, i);
            vertices[i].setTexCoord(tex, i);
        }
    }
}

QuadMesh::QuadMesh(TopLeft, GeometryRect const& geo, TextureRect const& tex) {
    for (int i = 0; i < 4; ++i) {
        vertices[i].setPosition(QuadVertex::topLeft, geo, i);
        vertices[i].setTexCoord(tex, i);
    }
}

void QuadMesh::transformPosition(Mat4 const& matrix) {
    for (auto& v : vertices) {
        v.transformPosition(matrix);
    }
}

void QuadMesh::setDepth(scalar_t depth) {
    for (auto& v : vertices) {
        v.setDepth(depth);
    }
}

void QuadMesh::setColor(Vec4 const& color) {
    for (auto& v : vertices) {
        v.setColor(color);
    }
}

void QuadMesh::scale(float width, float height) {
    for (auto& v : vertices) {
        v.scale(width, height);
    }
}
QuadVertex& QuadMesh::operator[](VertexName name) {
    return vertices[static_cast<std::underlying_type_t<VertexName>>(name)];
}

QuadVertex const& QuadMesh::operator[](VertexName name) const {
    return vertices[static_cast<std::underlying_type_t<VertexName>>(name)];
}

QuadVertex& QuadMesh::operator[](size_t i) { return vertices[i]; }

void QuadMesh::flipY() {
    for (auto& v : vertices) {
        v.flipY();
    }
}
void QuadMesh::flipX() {
    for (auto& v : vertices) {
        v.flipX();
    }
}
void QuadMesh::translate(Vec2 const& pos) {
    for (auto& v : vertices) {
        v.position[0] += pos[0];
        v.position[1] += pos[1];
    }
}

float QuadMesh::getWidth() const {
    return std::abs(this->operator[](VertexName::TOP_LEFT).getX() -
                    this->operator[](VertexName::TOP_RIGHT).getX());
}
float QuadMesh::getHeight() const {
    return std::abs(this->operator[](VertexName::TOP_LEFT).getY() -
                    this->operator[](VertexName::BOTTOM_LEFT).getY());
}

void QuadMesh::resetTexCoords() {
    this->operator[](VertexName::TOP_LEFT).setTexCoord(0, 1);
    this->operator[](VertexName::TOP_RIGHT).setTexCoord(1, 1);
    this->operator[](VertexName::BOTTOM_LEFT).setTexCoord(0, 0);
    this->operator[](VertexName::BOTTOM_RIGHT).setTexCoord(1, 0);
}

void QuadMesh::setNormalTexCoords(TextureRect const& rect) {
    for (int i = 0; i < 4; ++i) {
        vertices[i].setNormalTexCoord(rect, i);
    }
}
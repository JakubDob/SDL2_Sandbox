#include "VertexBufferLayout.h"
#include <glad/glad.h>

template <>
void VertexBufferLayout::add<Mat4>(unsigned count, VertexAttributeInfo info,
                                   unsigned char normalized) {
    for (unsigned i = 0; i < 4 * count; ++i) {
        data.push_back({GL_FLOAT, 4, 4 * static_cast<unsigned>(sizeof(float)),
                        normalized});
    }
    stride += sizeof(Mat4) * count;
}
template <>
void VertexBufferLayout::add<float>(unsigned count, VertexAttributeInfo info,
                                    unsigned char normalized) {
    data.push_back({GL_FLOAT, count,
                    count * static_cast<unsigned>(sizeof(float)), normalized});
    stride += sizeof(float) * count;
}

template <>
void VertexBufferLayout::add<unsigned>(unsigned count, VertexAttributeInfo info,
                                       unsigned char normalized) {
    data.push_back({GL_UNSIGNED_INT, count,
                    count * static_cast<unsigned>(sizeof(unsigned)),
                    normalized});
    stride += sizeof(unsigned) * count;
}

template <>
void VertexBufferLayout::add<unsigned char>(unsigned count,
                                            VertexAttributeInfo info,
                                            unsigned char normalized) {
    data.push_back({GL_BYTE, count,
                    count * static_cast<unsigned>(sizeof(unsigned char)),
                    normalized});
    stride += sizeof(unsigned char) * count;
}

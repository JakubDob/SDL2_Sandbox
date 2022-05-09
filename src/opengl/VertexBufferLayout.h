#pragma once
#include <vector>
#include "../Types.h"
enum class VertexAttributeInfo {
    POSITION,
    NORMAL,
    TEX_COORD,
    NORMAL_TEX_COORD,
    COLOR,
    POSITION_AND_TEX_COORD,
    TRANSLATION_ROTATION_SCALE,
    TEX_INDEX,
    DEPTH,
    MODEL_MATRIX,
    DEPTH_3EXTRA
};
struct VertexBufferLayoutData {
    VertexBufferLayoutData(unsigned type, unsigned count, unsigned size,
                           unsigned char normalized)
        : type(type), count(count), size(size), normalized(normalized) {}
    unsigned type;   // opengl data type, GL_FLOAT, GL_BYTE etc.
    unsigned count;  // number of components eg. position x,y,z -> 3; tex coords
                     // u,v -> 2
    unsigned size;  // number of bytes eg. 3 * sizeof(float)
    unsigned char normalized;
};

class VertexBufferLayout {
   public:
    template <typename T>
    void add(unsigned count, VertexAttributeInfo info,
             unsigned char normalized = false) {
        static_assert(!std::is_fundamental_v<T>);
    }
    template <>
    void add<Mat4>(unsigned count, VertexAttributeInfo info,
                   unsigned char normalized);
    template <>
    void add<float>(unsigned count, VertexAttributeInfo info,
                    unsigned char normalized);
    template <>
    void add<unsigned int>(unsigned count, VertexAttributeInfo info,
                           unsigned char normalized);
    template <>
    void add<unsigned char>(unsigned count, VertexAttributeInfo info,
                            unsigned char normalized);

    inline size_t size() const { return data.size(); }
    inline VertexBufferLayoutData const& operator[](int i) const {
        return data[i];
    }
    inline unsigned getStride() const { return stride; }

   private:
    std::vector<VertexBufferLayoutData> data;
    unsigned stride = 0;
};

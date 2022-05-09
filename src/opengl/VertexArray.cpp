#include "VertexArray.h"
#include <glad/glad.h>

VertexArray::VertexArray(VertexBuffer const& vbo, ElementBuffer const& ebo,
                         VertexBufferLayout const& layout) {
    init(vbo, ebo, layout);
}

void VertexArray::init(VertexBuffer const& vbo, ElementBuffer const& ebo,
                       VertexBufferLayout const& layout) {
    if (!initialized) {
        vertexCount = ebo.getVertexCount();
        glGenVertexArrays(1, &id);
        glBindVertexArray(id);
        vbo.bind();
        ebo.bind();
        long long offset = 0;
        for (int i = 0; i < layout.size(); ++i) {
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, layout[i].count, layout[i].type,
                                  layout[i].normalized, layout.getStride(),
                                  reinterpret_cast<void*>(offset));
            offset += layout[i].size;
        }
        unbind();
        ebo.unbind();
        vbo.unbind();
        initialized = true;
    }
}
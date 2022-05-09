#include "ElementBuffer.h"

ElementBuffer::ElementBuffer(std::vector<unsigned> const& indices, int usage) {
    init(indices, usage);
}

void ElementBuffer::init(std::vector<unsigned> const& indices, int usage) {
    if (!initialized) {
        glGenBuffers(1, &id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned),
                     indices.data(), usage);
        vertexCount = indices.size();
        initialized = true;
    }
}

void ElementBuffer::update(unsigned long long offset,
                           std::vector<unsigned> const& indices) {
    bind();
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset,
                    indices.size() * sizeof(unsigned), indices.data());
    vertexCount = indices.size();
}
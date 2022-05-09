#pragma once
#include "VertexBuffer.h"
#include "ElementBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray {
   public:
    VertexArray() {}
    VertexArray(VertexBuffer const& vbo, ElementBuffer const& ebo,
                VertexBufferLayout const& layout);
    void init(VertexBuffer const& vbo, ElementBuffer const& ebo,
              VertexBufferLayout const& layout);
    ~VertexArray() {
        if (initialized) {
            glDeleteVertexArrays(1, &id);
            glFlush();
        }
    };
    inline void bind() const { glBindVertexArray(id); }
    inline void unbind() const { glBindVertexArray(0); }
    inline int getVertexCount() const { return vertexCount; }
    inline void setVertexCount(int count) { vertexCount = count; }
    inline unsigned getId() const { return id; }
    VertexArray& operator=(VertexArray const& o) = delete;
    VertexArray(VertexArray const& o) = delete;
    VertexArray& operator=(VertexArray&& o) noexcept {
        if (this != &o) {
            vertexCount = o.vertexCount;
            id = o.id;
            initialized = o.initialized;
            o.id = 0;
        }
        return *this;
    }
    VertexArray(VertexArray&& o) noexcept {
        vertexCount = o.vertexCount;
        id = o.id;
        initialized = o.initialized;
        o.id = 0;
    };

   private:
    int vertexCount = 0;
    unsigned id = 0;
    bool initialized = false;
};
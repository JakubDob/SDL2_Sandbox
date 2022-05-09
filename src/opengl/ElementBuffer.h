#pragma once
#include <glad/glad.h>
#include <vector>

class ElementBuffer {
   public:
    ElementBuffer(){};
    ElementBuffer(std::vector<unsigned> const& indices,
                  int usage = GL_STATIC_DRAW);
    ~ElementBuffer() {
        if (initialized) {
            glDeleteBuffers(1, &id);
            glFlush();
        }
    };
    void init(std::vector<unsigned> const& indices, int usage = GL_STATIC_DRAW);
    void update(unsigned long long offset,
                std::vector<unsigned> const& indices);
    inline void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); };
    inline void unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); };
    inline unsigned getId() const { return id; }
    inline int getVertexCount() const { return vertexCount; }
    ElementBuffer& operator=(ElementBuffer const&) = delete;
    ElementBuffer(ElementBuffer const&) = delete;
    ElementBuffer& operator=(ElementBuffer&& o) noexcept {
        if (this != &o) {
            this->vertexCount = o.vertexCount;
            this->id = o.id;
            this->initialized = o.initialized;
            o.id = 0;
        }
        return *this;
    }
    ElementBuffer(ElementBuffer&& o) noexcept {
        this->vertexCount = o.vertexCount;
        this->id = o.id;
        this->initialized = o.initialized;
        o.id = 0;
    }

   private:
    int vertexCount = 0;
    unsigned id = 0;
    bool initialized = false;
};
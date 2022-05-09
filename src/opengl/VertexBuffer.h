#pragma once
#include <glad/glad.h>
#include <vector>

class VertexBuffer {
   public:
    VertexBuffer() {}
    template <typename T>
    VertexBuffer(std::vector<T> const& data, int usage = GL_STATIC_DRAW) {
        init(data, usage);
    }
    template <typename T>
    VertexBuffer(T const&, int vertexCount, int usage) {
        init<T>(vertexCount, usage);
    }
    ~VertexBuffer() {
        if (initialized) {
            glDeleteBuffers(1, &id);
            glFlush();
        }
    }
    template <typename T>
    void init(std::vector<T> const& data, int usage = GL_STATIC_DRAW) {
        if (!initialized) {
            glGenBuffers(1, &id);
            glBindBuffer(GL_ARRAY_BUFFER, id);
            glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(),
                         usage);
            initialized = true;
        }
    }

    // initialize empty buffer
    template <typename T>
    void init(int itemCount, int usage) {
        if (!initialized) {
            glGenBuffers(1, &id);
            glBindBuffer(GL_ARRAY_BUFFER, id);
            glBufferData(GL_ARRAY_BUFFER, itemCount * sizeof(T), nullptr,
                         usage);
            initialized = true;
        }
    }
    inline void bind() const { glBindBuffer(GL_ARRAY_BUFFER, id); };
    inline void unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); };
    inline unsigned getId() const { return id; }

    template <typename T>
    inline void update(unsigned long long offset, std::vector<T> const& data) {
        bind();
        glBufferSubData(GL_ARRAY_BUFFER, offset, data.size() * sizeof(T),
                        data.data());
    }

    VertexBuffer(VertexBuffer const&) = delete;
    VertexBuffer& operator=(VertexBuffer const&) = delete;

    VertexBuffer(VertexBuffer&& o) noexcept {
        id = o.id;
        initialized = o.initialized;
        o.id = 0;
    }
    VertexBuffer& operator=(VertexBuffer&& o) noexcept {
        if (this != &o) {
            id = o.id;
            initialized = o.initialized;
            o.id = 0;
        }
        return *this;
    };

   private:
    unsigned id = 0;
    bool initialized = false;
};
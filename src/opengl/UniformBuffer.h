#pragma once
#include <glad/glad.h>
#include <vector>
#include <array>
#include "../Types.h"

constexpr auto alignTo(size_t alignment, size_t value) {
    if ((value & (alignment - 1)) != 0) {
        return (value & ~(alignment - 1)) + alignment;
    } else {
        return value;
    }
};

template <typename T>
constexpr auto getAlignmentForType() {
    constexpr size_t N = 4;
    if constexpr (std::is_same_v<T, Vec2>) {
        return N * 2;
    }
    if constexpr (std::is_same_v<T, Mat4>) {
        return N * 4;
    }
    if constexpr (std::is_array_v<T>) {
        return alignTo(N * 4,
                       getAlignmentForType<std::remove_all_extents_t<T>>());
    }
    if constexpr (std::is_arithmetic_v<T>) {
        return N;
    }
    return N * 4;
}

class UniformBuffer {
   public:
    template <typename T>
    UniformBuffer(T const&, int count, int usage = GL_DYNAMIC_DRAW) {
        init<T>(count, usage);
    }
    template <typename T>
    void init(int count, int usage = GL_DYNAMIC_DRAW) {
        if (!initialized) {
            glGenBuffers(1, &id);
            glBindBuffer(GL_UNIFORM_BUFFER, id);
            glBufferData(GL_UNIFORM_BUFFER, count * sizeof(T), nullptr, usage);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            initialized = true;
        }
    }

    void bind(int bindingPoint) const {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, id);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    template <size_t offset = 0, typename... Args>
    void updateStd140(Args const&... args) {
        glBindBuffer(GL_UNIFORM_BUFFER, id);

        constexpr auto ArgSize = sizeof...(Args);

        constexpr std::array<size_t, ArgSize> baseAlignment{
            (getAlignmentForType<Args>())...};
        constexpr auto elementCount = [ArgSize, baseAlignment]() {
            constexpr std::array<bool, ArgSize> isArray{
                std::is_array_v<Args>...};
            constexpr std::array<size_t, ArgSize> byteSize{sizeof(Args)...};
            constexpr std::array<size_t, ArgSize> flatSize{
                sizeof(std::remove_all_extents_t<Args>)...};
            std::array<size_t, ArgSize> elementCount{};
            for (size_t i = 0; i < elementCount.size(); ++i) {
                if (isArray[i]) {
                    elementCount[i] = byteSize[i] / flatSize[i];
                } else {
                    elementCount[i] = byteSize[i] / baseAlignment[i];
                    if (elementCount[i] == 0) {
                        elementCount[i] = 1;
                    }
                }
            }
            return elementCount;
        }();

        constexpr auto alignedOffsets = [&]() {
            std::array<size_t, ArgSize> alignedOffsets{};
            alignedOffsets[0] = alignTo(baseAlignment[0], offset);
            for (size_t i = 1; i < alignedOffsets.size(); ++i) {
                alignedOffsets[i] =
                    alignTo(baseAlignment[i],
                            alignedOffsets[i - 1] +
                                (baseAlignment[i - 1] * elementCount[i - 1]));
            }
            return alignedOffsets;
        }();

        constexpr auto sizeNeeded =
            alignedOffsets.back() + baseAlignment.back() * elementCount.back();
        if (buffer.size() < sizeNeeded) {
            buffer.resize(sizeNeeded);
        }

        size_t i = 0;
        (memcpy(&buffer[alignedOffsets[i++]], &args, sizeof(args)), ...);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, buffer.size(),
                        buffer.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    inline unsigned int getId() const { return id; }

    ~UniformBuffer() {
        if (initialized) {
            glDeleteBuffers(1, &id);
            glFlush();
        }
    }

    UniformBuffer(UniformBuffer const&) = delete;
    UniformBuffer& operator=(UniformBuffer const&) = delete;
    UniformBuffer& operator=(UniformBuffer&& o) noexcept {
        if (this != &o) {
            id = o.id;
            initialized = o.initialized;
            o.id = 0;
        }
        return *this;
    };
    UniformBuffer(UniformBuffer&& o) noexcept {
        id = o.id;
        initialized = o.initialized;
        o.id = 0;
    }

   private:
    std::vector<byte> buffer;
    unsigned int id;
    bool initialized = false;
};
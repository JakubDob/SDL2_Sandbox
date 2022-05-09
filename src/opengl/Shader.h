#pragma once
#include <string>
#include <glad/glad.h>
#include <type_traits>
#include "../Types.h"

class Shader {
   public:
    Shader(std::string const& vertexShaderSource,
           std::string const& fragmentShaderSource);
    ~Shader() { glDeleteProgram(programId); }
    inline void use() const { glUseProgram(programId); }
    inline unsigned int getId() const { return programId; }
    inline void setBool(std::string const& name, bool value) const {
        glUniform1i(glGetUniformLocation(programId, name.c_str()),
                    static_cast<int>(value));
    }
    inline void setInt(std::string const& name, int value) const {
        glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
    }
    inline void setIntArray(std::string const& name, int count,
                            int* value) const {
        glUniform1iv(glGetUniformLocation(programId, name.c_str()), count,
                     value);
    }
    inline void setFloat(std::string const& name, float value) const {
        glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
    }
    template <typename T,
              std::enable_if_t<std::is_same_v<T, Vec2>, bool> = false>
    inline void setVec2(std::string const& name, T const& value) const {
        glUniform2fv(glGetUniformLocation(programId, name.c_str()), 1,
                     value.getData());
    }
    template <typename T,
              std::enable_if_t<std::is_same_v<T, Vec3>, bool> = false>
    inline void setVec3(std::string const& name, T const& value) const {
        glUniform3fv(glGetUniformLocation(programId, name.c_str()), 1,
                     value.getData());
    }
    template <typename T,
              std::enable_if_t<std::is_same_v<T, Vec4>, bool> = false>
    inline void setVec4(std::string const& name, T const& value) const {
        glUniform4fv(glGetUniformLocation(programId, name.c_str()), 1,
                     value.getData());
    }
    // matrix class has row major memory layout, so transpose the mat4 in order
    // to left multiply in shaders
    template <typename T,
              std::enable_if_t<std::is_same_v<T, Mat4>, bool> = false>
    inline void setMat4(std::string const& name, T const& value) const {
        glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), 1,
                           GL_TRUE, value.getData());
    }

    inline void bind(std::string const& name, int bindingPoint) const {
        auto blockIndex = glGetUniformBlockIndex(programId, name.c_str());
        glUniformBlockBinding(programId, blockIndex, bindingPoint);
    }

    Shader& operator=(Shader&) = delete;
    Shader& operator=(Shader&&) = delete;
    Shader(Shader&) = delete;
    Shader(Shader&&) = delete;

   private:
    unsigned programId = 0;
};
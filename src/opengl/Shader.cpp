#include "Shader.h"

Shader::Shader(std::string const& vertexShaderSource,
               std::string const& fragmentShaderSource) {
    unsigned int vertShaderId, fragShaderId;
    char const* vs = vertexShaderSource.c_str();
    char const* fs = fragmentShaderSource.c_str();

    vertShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShaderId, 1, &vs, NULL);
    glCompileShader(vertShaderId);
    int success;
    char infoLog[512];
    glGetShaderiv(vertShaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertShaderId, 512, NULL, infoLog);
        throw std::runtime_error("Vertex shader compilation failed: " +
                                 std::string(infoLog));
    }

    fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShaderId, 1, &fs, NULL);
    glCompileShader(fragShaderId);
    glGetShaderiv(fragShaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragShaderId, 512, NULL, infoLog);
        throw std::runtime_error("Fragment shader compilation failed: " +
                                 std::string(infoLog));
    }

    programId = glCreateProgram();
    glAttachShader(programId, vertShaderId);
    glAttachShader(programId, fragShaderId);
    glLinkProgram(programId);
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        throw std::runtime_error("Shader program linking failed: " +
                                 std::string(infoLog));
    }
    glDeleteShader(vertShaderId);
    glDeleteShader(fragShaderId);
}
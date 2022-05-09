cmake_minimum_required(VERSION 3.22.1)

target_sources(
    SDL2_Sandbox PRIVATE
    Shader.cpp
    VertexArray.cpp
    VertexBufferLayout.cpp
    ElementBuffer.cpp
    Texture.cpp

    Shader.h
    VertexArray.h
    VertexBuffer.h
    VertexBufferLayout.h
    ElementBuffer.h
    Texture.h
    UniformBuffer.h
)

install(
    FILES
    Shader.h
    VertexArray.h
    VertexBuffer.h
    VertexBufferLayout.h
    ElementBuffer.h
    Texture.h
    UniformBuffer.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SDL2_Sandbox/opengl
)
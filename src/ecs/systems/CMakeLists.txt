cmake_minimum_required(VERSION 3.22.1)

target_sources(
    SDL2_Sandbox PRIVATE
    Renderer.cpp
    PhysicsSystem.cpp
    CollisionSystem2D.cpp
    SpriteSystem.cpp
    HealthBarSystem.cpp
    GuiSystem.cpp
    AiSystem.cpp

    CollisionSystem2D.h
    PhysicsSystem.h
    Renderer.h
    SpriteSystem.h
    HealthBarSystem.h
    GuiSystem.h
    AiSystem.h
)

install(
    FILES
    CollisionSystem2D.h
    PhysicsSystem.h
    Renderer.h
    SpriteSystem.h
    HealthBarSystem.h
    GuiSystem.h
    AiSystem.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SDL2_Sandbox/ecs/systems
)
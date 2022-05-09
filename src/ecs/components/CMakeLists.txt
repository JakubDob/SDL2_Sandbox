cmake_minimum_required(VERSION 3.22.1)

target_sources(
    SDL2_Sandbox PRIVATE
    Transform2D.cpp
    Collider2D.cpp
    Physics2D.cpp
    Camera2D.cpp
    PolygonCollider.cpp
    BoxCollider.cpp
    BaseCollider2D.cpp
    Controller2D.cpp
    HealthBar.cpp
    StaticSprite.cpp
    AnimatedSprite.cpp
    CircleCollider.cpp

    BaseCollider2D.h
    BoxCollider.h
    Camera2D.h
    CircleCollider.h
    Collider2D.h
    Controller2D.h
    Physics2D.h
    PolygonCollider.h
    Transform2D.h
    HealthBar.h
    StaticSprite.h
    AnimatedSprite.h
    CircleCollider.h
)

install(
    FILES
    BaseCollider2D.h
    BoxCollider.h
    Camera2D.h
    CircleCollider.h
    Collider2D.h
    Controller2D.h
    Physics2D.h
    PolygonCollider.h
    Transform2D.h
    HealthBar.h
    StaticSprite.h
    AnimatedSprite.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SDL2_Sandbox/ecs/components
)
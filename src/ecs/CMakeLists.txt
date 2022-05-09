cmake_minimum_required(VERSION 3.22.1)

add_subdirectory(components)
add_subdirectory(systems)

target_sources(
    SDL2_Sandbox PRIVATE
    PrefabFactory.cpp
    AnimationFactory.cpp

    EcsContainer.h
    EcsContainerInl.hpp
    EcsComponentList.h
    PrefabFactory.h
    AnimationFactory.h
)

install(
    FILES
    EcsContainer.h
    EcsContainerInl.hpp
    EcsComponentList.h
    PrefabFactory.h
    AnimationFactory.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SDL2_Sandbox/ecs
)
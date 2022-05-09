cmake_minimum_required(VERSION 3.22.1)

add_subdirectory(glad)
add_subdirectory(vendor)
add_subdirectory(opengl)
add_subdirectory(gui)
add_subdirectory(ecs)

#add header files to target_sources as well to show them in the folders in the IDE (Visual Studio solution)
#root cmake calls get_target_property(SOURCES) and source_group to make it happen

target_sources(
    SDL2_Sandbox PRIVATE
    Window.cpp
    KeyToggle.cpp
    InputHandler.cpp
    TimeUtils.cpp
    AssetsManager.cpp
    Engine2D.cpp
    TextureAtlas.cpp
    Utils.cpp
    FontAtlas.cpp
    QuadMesh.cpp
    TextureAtlasFrame.cpp
    DrawableText.cpp

    Engine2D.h
    TimeUtils.h
    InputHandler.h
    KeyToggle.h
    Matrix.h
    Types.h
    Utils.h
    Window.h
    Lang.h
    InputTarget.h
    AssetsManager.h
    EngineConstants.h
    CleanupHelpers.h
    TemplateUtils.h
    FiniteStateMachine.h
    TextureAtlas.h
    CircularBuffer.h
    FontAtlas.h
    Debug.h
    QuadMesh.h
    TextureAtlasFrame.h
    DrawableText.h
)

install(
    FILES
    Engine2D.h
    TimeUtils.h
    InputHandler.h
    KeyToggle.h
    Matrix.h
    Types.h
    Utils.h
    Window.h
    Lang.h
    InputTarget.h
    AssetsManager.h
    EngineConstants.h
    CleanupHelpers.h
    TemplateUtils.h
    FiniteStateMachine.h
    TextureAtlas.h
    CircularBuffer.h
    FontAtlas.h
    QuadMesh.h
    TextureAtlasFrame.h
    DrawableText.h
    Debug.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SDL2_Sandbox
)
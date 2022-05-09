cmake_minimum_required(VERSION 3.22.1)

target_sources(
    SDL2_Sandbox PRIVATE
    GuiButton.cpp
    GuiElement.cpp
    GuiContainer.cpp
    GuiTitlebar.cpp
    GuiWindow.cpp
    GuiScrollbar.cpp
    GuiWindowBorder.cpp
    SandboxOptionsWindow.cpp

    GuiButton.h
    GuiElement.h
    GuiContainer.h
    GuiTitlebar.h
    GuiWindow.h
    BoxResizer.h
    GuiScrollbar.h
    GuiWindowBorder.h
    GuiConstants.h
    SandboxOptionsWindow.h
)

install(
    FILES
    GuiButton.h
    GuiElement.h
    GuiContainer.h
    GuiTitlebar.h
    GuiWindow.h
    BoxResizer.h
    GuiScrollbar.h
    GuiWindowBorder.h
    GuiConstants.h
    SandboxOptionsWindow.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SDL2_Sandbox/gui
)
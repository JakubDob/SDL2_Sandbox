cmake_minimum_required(VERSION 3.22.1)
project(SDL2_Sandbox)

message(STATUS "Building static library SDL2_Sandbox")
add_library(SDL2_Sandbox STATIC)
add_subdirectory(src)
add_subdirectory(tests)
#append 'd' to debug version of the library to keep both versions in the same folder in the install directory. Which lib is being used is specified in ...Config.cmake files
set_target_properties(SDL2_Sandbox PROPERTIES DEBUG_POSTFIX "d")


target_compile_features(SDL2_Sandbox PRIVATE cxx_std_23)

#if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
#    target_compile_features(SDL2_Sandbox PRIVATE cxx_std_23)
#else()
#    set(CMAKE_CXX_FLAGS "-std=c++2b")
#endif()
#add path to search for Find.cmake modules
option(USE_CONFIG "Use config file to link (built from sources)" FALSE)

if(NOT ${USE_CONFIG})
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
endif()

#if the library was built and installed from sources, try to find config first
#set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)

#namespaces set by Find.cmake modules differ from the ones set in the config files when built from sources, for example:
#SDL2 library config file sets SDL2::SDL2main namespace but FindSDL2.cmake sets SDL2::Main,
#similarly, config: SDL2_ttf::SDL2_ttf, module: SDL2::TTF
#If all libraries have config file (can be located with CMAKE_PREFIX_PATH), set USE_CONFIG

if(${USE_CONFIG})
target_link_libraries(
    SDL2_Sandbox
    PRIVATE
    SDL2::SDL2main
    SDL2::Mixer
    SDL2_ttf::SDL2_ttf
    SDL2::Image
    glad
)
else()
target_link_libraries(
    SDL2_Sandbox
    PRIVATE
    SDL2::Main
    SDL2::Mixer
    SDL2::TTF
    SDL2::Image
    glad
)
endif()

#copy dlls
get_target_property(target_type SDL2_Sandbox TYPE)
if(target_type STREQUAL "EXECUTABLE")
    add_custom_command(TARGET SDL2_Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:SDL2_Sandbox> $<TARGET_FILE_DIR:SDL2_Sandbox>
    COMMAND_EXPAND_LISTS
    )
endif()


include(GNUInstallDirs)
target_include_directories(
    SDL2_Sandbox
    PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

install(
    TARGETS SDL2_Sandbox glad #name of exported targets to be included in the export set
    EXPORT SDL2_SandboxTargets #name of the export set
    LIBRARY DESTINATION ${CMAKE_INSTALL_DIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT SDL2_SandboxTargets #name of the export set
    FILE SDL2_SandboxTargets.cmake #file containing code for creating imported targets (existing libs) by an external project
    NAMESPACE SDL2_Sandbox::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SDL2_Sandbox
)

include(CMakePackageConfigHelpers)
# this creates Config.cmake
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/SDL2_SandboxConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SDL2_Sandbox
)

install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/SDL2_SandboxConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SDL2_Sandbox
)

# create project structure for Visual Studio
get_target_property(project_sources SDL2_Sandbox SOURCES)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${project_sources})

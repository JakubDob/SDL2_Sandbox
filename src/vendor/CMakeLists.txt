cmake_minimum_required(VERSION 3.22.1)
# CMakeLists for all external deps downloaded with fetch content

include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz
)
FetchContent_MakeAvailable(json)

target_link_libraries(SDL2_Sandbox PRIVATE nlohmann_json::nlohmann_json)
install(TARGETS nlohmann_json EXPORT SDL2_SandboxTargets)
install(
    FILES
    ${json_SOURCE_DIR}/single_include/nlohmann/json.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SDL2_Sandbox/nlohmann
)
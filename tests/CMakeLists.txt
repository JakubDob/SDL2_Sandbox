#run tests with ctest --test-dir out/build/tests -C Release
cmake_minimum_required(VERSION 3.22.1)

project(MathTests)

enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(MathTests)

target_compile_features(MathTests PRIVATE cxx_std_23)

add_dependencies(
    MathTests
    SDL2_Sandbox
)

target_sources(
    MathTests
    PRIVATE
    MathTests.cpp
)

target_link_libraries(
    MathTests
    PRIVATE
    gtest_main
    SDL2_Sandbox
)

target_include_directories(
    MathTests
    PRIVATE
    $<TARGET_PROPERTY:SDL2_Sandbox,INCLUDE_DIRECTORIES>
)

include(GoogleTest)
gtest_discover_tests(
    MathTests
    DISCOVERY_MODE PRE_TEST #delays test discovery to specify config, eg. 
                            #if debug is not built, then ctest -C Debug will fail with this option; 
                            #without it ctest -C Debug succeeds even when only Release is built
)
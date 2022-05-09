cmake_minimum_required (VERSION 3.22.1)

project(Glad)

add_library(glad)

target_sources(
    glad 
    PRIVATE
    src/glad.c
    include/glad/glad.h
    include/KHR/khrplatform.h
)

target_include_directories(
    glad 
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

install(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/glad/glad.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/glad/glad
)
install(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/KHR/khrplatform.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/KHR
)
install(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/glad/glad.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/glad/
)
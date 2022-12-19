cmake_minimum_required(VERSION 3.20)

project(canshark)

option(MXE_BUILD "Build with MXE toolchain" OFF)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin)

if(WIN32 AND NOT MXE_BUILD)
    set(CMAKE_PREFIX_PATH "C:\\Qt6\\6.4.1\\msvc2019_64")
endif()

include_directories(include)

add_subdirectory(libcanshark)
add_subdirectory(QHexView)
add_subdirectory(cansharkgui)

cmake_minimum_required(VERSION 3.20)

project(canshark)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin)

include_directories(include)

add_subdirectory(libcanshark)
add_subdirectory(QHexView)
add_subdirectory(cansharkgui)

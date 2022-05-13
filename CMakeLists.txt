cmake_minimum_required(VERSION 3.20)
project(canshark)

set(CMAKE_PREFIX_PATH "C:/Qt5.13/5.15.2/msvc2019")

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin)

include_directories(include)

add_subdirectory(libcanshark)
add_subdirectory(cansharkgui)

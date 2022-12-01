cmake_minimum_required(VERSION 3.20)

project(canshark)

if(UNIX AND NOT APPLE)
    set(CMAKE_PREFIX_PATH "")
elseif(WIN32)
    set(CMAKE_PREFIX_PATH "C:/Qt5.13/5.15.2/msvc2019")
else()
    set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qt@5/5.15.5_2")
endif()

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin)

include_directories(include)

add_subdirectory(libcanshark)
add_subdirectory(QHexView)
add_subdirectory(cansharkgui)

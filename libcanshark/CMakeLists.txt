cmake_minimum_required(VERSION 3.20)

project(libcanshark)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/Qt5.13/5.15.2/msvc2019_64")

find_package(Qt5 COMPONENTS
        Core
        SerialPort
        REQUIRED)

set(SRCS
        LibCanShark.cpp
)

add_library(libcanshark STATIC ${SRCS})

target_link_libraries(libcanshark Qt5::Core Qt5::SerialPort)

install(TARGETS libcanshark
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
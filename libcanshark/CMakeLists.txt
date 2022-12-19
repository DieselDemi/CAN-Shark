cmake_minimum_required(VERSION 3.20)

project(libcanshark)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        SerialPort
        REQUIRED)

include_directories(threads)
include_directories(helpers)
include_directories(drivers)

set(SRCS
        RecordItem.h
        threads/RecordingThread.cpp
        threads/RecordingThread.h
        threads/FirmwareUpdateThread.cpp
        threads/FirmwareUpdateThread.h
        drivers/CanSharkMini.cpp
        drivers/CanSharkMini.h
        helpers/Helpers.cpp
        helpers/Helpers.h
        drivers/CanSharkPro.cpp
        drivers/CanSharkPro.h
        drivers/CanShark.cpp
        drivers/CanShark.h
        drivers/CanSharkDrivers.h)

add_library(libcanshark STATIC ${SRCS})

if(WIN32)
    target_link_libraries(libcanshark Qt6::Core Qt6::SerialPort ws2_32)
else()
    target_link_libraries(libcanshark Qt6::Core Qt6::SerialPort)
endif()

install(TARGETS libcanshark
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

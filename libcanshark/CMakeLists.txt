cmake_minimum_required(VERSION 3.20)

project(libcanshark)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#set(CMAKE_PREFIX_PATH "C:/Qt5.13/5.15.2/msvc2019_64")
#set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qt/6.3.1_4")

find_package(Qt6 COMPONENTS
        Core
        SerialPort
        REQUIRED)

include_directories(drivers)
include_directories(helpers)

set(SRCS
        DataParserThread.cpp
        DataParserThread.h
        RecordItem.h
        drivers/CanSharkMini.cpp
        drivers/CanSharkMini.h
        helpers/Helpers.cpp
        helpers/Helpers.h
        drivers/CanSharkPro.cpp
        drivers/CanSharkPro.h
        drivers/CanShark.cpp
        drivers/CanShark.h
        drivers/CanSharkDrivers.h
)

add_library(libcanshark STATIC ${SRCS})

if(WIN32)
    target_link_libraries(libcanshark Qt6::Core Qt6::SerialPort ws2_32)
elseif(APPLE)
    target_link_libraries(libcanshark Qt6::Core Qt6::SerialPort)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(libcanshark Qt6::Core Qt6::SerialPort)
endif()

install(TARGETS libcanshark
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

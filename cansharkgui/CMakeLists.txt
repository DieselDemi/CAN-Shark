cmake_minimum_required(VERSION 3.22)
project(CanShark)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")


find_package(Qt6 COMPONENTS Core Gui Widgets SerialPort REQUIRED)

include_directories(../libcanshark)
include_directories(../QHexView)

set(SRCS BaseTheme.h main.cpp FormMainWindow.cpp FormMainWindow.h FormMainWindow.ui RecordTableModel.cpp RecordTableModel.h FormInspect.cpp FormInspect.h FormInspect.ui FormSettings.cpp FormSettings.h FormSettings.ui)

# Add exe here
if (WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "resources/appicon.rc")

    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        add_executable(CanShark ${SRCS} ${APP_ICON_RESOURCE_WINDOWS})
    else()
        add_executable(CanShark WIN32 ${SRCS} ${APP_ICON_RESOURCE_WINDOWS})
    endif()
elseif (APPLE)
    # NOTE: Don't include the path in MACOSX_BUNDLE_ICON_FILE -- this is
    # the property added to Info.plist
    set(MACOSX_BUNDLE_ICON_FILE can-shark.icns)

    # And this part tells CMake where to find and install the file itself
    set(app_ICONS ${CMAKE_CURRENT_SOURCE_DIR}/resources/can-shark.icns)
    set_source_files_properties(${app_ICONS} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")

    add_executable(CanShark MACOSX_BUNDLE ${app_ICONS} ${SRCS})
else (UNIX AND NOT APPLE)
    add_executable(CanShark ${SRCS})
endif ()

target_link_libraries(CanShark Qt6::Core Qt6::Gui Qt6::Widgets Qt6::SerialPort libcanshark qhexview-lib)

if(WIN32 AND CMAKE_BUILD_TYPE MATCHES "Release" AND NOT MXE_BUILD)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${QT_INSTALL_PATH}/bin/windeployqt.exe --dir $<TARGET_FILE_DIR:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.exe)
elseif (WIN32 AND CMAKE_BUILD_TYPE MATCHES "Debug" AND NOT MXE_BUILD)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${QT_INSTALL_PATH}/bin/windeployqt.exe --debug --dir $<TARGET_FILE_DIR:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.exe)
elseif (APPLE)
    install(TARGETS CanShark RUNTIME DESTINATION bin BUNDLE DESTINATION bin)
else ()
    install(TARGETS CanShark RUNTIME DESTINATION bin)
endif ()

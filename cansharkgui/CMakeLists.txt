cmake_minimum_required(VERSION 3.22)
project(CanShark)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        SerialPort
        REQUIRED)

include_directories(../libcanshark)
include_directories(../QHexView)

if (WIN32)
    add_executable(CanShark WIN_32 main.cpp FormMainWindow.cpp FormMainWindow.h FormMainWindow.ui RecordTableModel.cpp RecordTableModel.h FormInspect.cpp FormInspect.h FormInspect.ui)
elseif (APPLE)
    # NOTE: Don't include the path in MACOSX_BUNDLE_ICON_FILE -- this is
    # the property added to Info.plist
    set(MACOSX_BUNDLE_ICON_FILE can-shark.icns)

    # And this part tells CMake where to find and install the file itself
    set(app_ICONS ${CMAKE_CURRENT_SOURCE_DIR}/images/can-shark.icns)
    set_source_files_properties(${app_ICONS} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")

    add_executable(CanShark MACOSX_BUNDLE ${app_ICONS} main.cpp FormMainWindow.cpp FormMainWindow.h FormMainWindow.ui RecordTableModel.cpp RecordTableModel.h FormInspect.cpp FormInspect.h FormInspect.ui)
else (UNIX AND NOT APPLE)
    add_executable(CanShark main.cpp FormMainWindow.cpp FormMainWindow.h FormMainWindow.ui RecordTableModel.cpp RecordTableModel.h FormInspect.cpp FormInspect.h FormInspect.ui)
endif ()

target_link_libraries(CanShark
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::SerialPort
        libcanshark
        qhexview-lib
        )

if (WIN32)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets SerialPort)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
    install(TARGETS CanShark RUNTIME DESTINATION bin)
elseif (APPLE)
    install(TARGETS CanShark RUNTIME DESTINATION bin BUNDLE DESTINATION bin)
elseif (UNIX AND NOT APPLE)
    install(TARGETS CanShark RUNTIME DESTINATION bin)
endif ()

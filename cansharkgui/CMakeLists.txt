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
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
    #    set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CXX_STANDARD_LIBRARIES}")
    #    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lpthread -Wl,-Bdynamic,--no-whole-archive")

    add_executable(CanShark WIN32 main.cpp FormMainWindow.cpp FormMainWindow.h FormMainWindow.ui RecordTableModel.cpp RecordTableModel.h FormInspect.cpp FormInspect.h FormInspect.ui)
elseif (APPLE)
    # NOTE: Don't include the path in MACOSX_BUNDLE_ICON_FILE -- this is
    # the property added to Info.plist
    set(MACOSX_BUNDLE_ICON_FILE can-shark.icns)

    # And this part tells CMake where to find and install the file itself
    set(app_ICONS ${CMAKE_CURRENT_SOURCE_DIR}/images/can-shark.icns BaseTheme.h)
    set_source_files_properties(${app_ICONS} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")

    add_executable(CanShark MACOSX_BUNDLE ${app_ICONS} main.cpp FormMainWindow.cpp FormMainWindow.h FormMainWindow.ui RecordTableModel.cpp RecordTableModel.h FormInspect.cpp FormInspect.h FormInspect.ui)
else (UNIX AND NOT APPLE)
    add_executable(CanShark main.cpp FormMainWindow.cpp FormMainWindow.h FormMainWindow.ui RecordTableModel.cpp RecordTableModel.h FormInspect.cpp FormInspect.h FormInspect.ui)
endif ()

if (WIN32)
    target_link_libraries(CanShark Qt6::Core Qt6::Gui Qt6::Widgets Qt6::SerialPort libcanshark qhexview-lib -static gcc stdc++ winpthread -dynamic)
    #This is bad but I don't know what else to do need to RTFM on CMAKE static libraries
else ()
    target_link_libraries(CanShark Qt6::Core Qt6::Gui Qt6::Widgets Qt6::SerialPort libcanshark qhexview-lib)
endif ()

if (WIN32 AND NOT DEBUG)
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
    #    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    #            COMMAND ${CMAKE_COMMAND} -E copy
    #            "${QT_INSTALL_PATH}/bin/libstdc++-6.dll"
    #            "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
elseif (APPLE)
    install(TARGETS CanShark RUNTIME DESTINATION bin BUNDLE DESTINATION bin)
else ()
    install(TARGETS CanShark RUNTIME DESTINATION bin)
endif ()

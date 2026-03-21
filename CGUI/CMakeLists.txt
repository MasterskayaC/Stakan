cmake_minimum_required(VERSION 3.10)
project(gui_library)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(SOURCES
    graphicalui.cpp
)

set(HEADERS
    graphicalui.h
)

set(UI_FILES
    mainwindow.ui
)

add_library(gui STATIC ${SOURCES} ${HEADERS} ${UI_FILES})

target_link_libraries(gui Qt5::Widgets)

target_include_directories(gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

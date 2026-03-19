cmake_minimum_required(VERSION 3.10)
project(Stakan)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(SOURCES
   main.cpp
   graphicalui.cpp
)

add_executable(stakan ${SOURCES})

target_link_libraries(stakan Qt5::Widgets)
cmake_minimum_required(VERSION 3.14)

project(Stakan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(snapshots_broadcaster)

add_subdirectory(src/database)

add_executable(exc_glass main.cpp)

target_link_libraries(exc_glass database_lib)

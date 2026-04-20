cmake_minimum_required(VERSION 3.11)
project(database_lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(database_lib INTERFACE)

target_include_directories(database_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

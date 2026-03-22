cmake_minimum_required(VERSION 3.11)

project(stakan)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(console)

add_executable(exc_glass main.cpp)

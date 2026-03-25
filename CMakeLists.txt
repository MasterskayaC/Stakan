cmake_minimum_required(VERSION 3.11)

project(stakan)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(exc_glass main.cpp)

add_subdirectory(tcp_client)

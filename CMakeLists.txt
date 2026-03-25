cmake_minimum_required(VERSION 3.11)

project(stakan)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src/client_list)

add_executable(exc_glass main.cpp)
target_link_libraries(exc_glass client_list)

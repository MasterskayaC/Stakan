cmake_minimum_required(VERSION 3.11)

project(Stakan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(snapshots_broadcaster)
add_subdirectory(bid_ask_snaphot_interface)
add_subdirectory(src/client_list)
add_subdirectory(src/TCP_server)

add_executable(exc_glass main.cpp)

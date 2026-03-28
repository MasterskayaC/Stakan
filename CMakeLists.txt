cmake_minimum_required(VERSION 3.14)
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
project(Stakan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED COMPONENTS filesystem system)
find_package(Threads REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_subdirectory(snapshots_broadcaster)
add_subdirectory(bid_ask_snaphot_interface)
add_subdirectory(src/client_list)
add_subdirectory(src/TCP_server)
add_subdirectory(client_lib)
add_subdirectory(tcp_client)
add_subdirectory(bid_ask_book)
add_subdirectory(console)
add_subdirectory(CGUI)

add_executable(exc_glass main.cpp)


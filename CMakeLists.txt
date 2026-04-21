cmake_minimum_required(VERSION 3.15)
project(Stakan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED COMPONENTS filesystem system serialization)
find_package(Threads REQUIRED)
find_package(Catch2 REQUIRED)
#find_package(Qt6 REQUIRED COMPONENTS Widgets) неуверен что стоит использовать Qt6
#заменить как примем решение
#find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_subdirectory(src/snapshots_broadcaster)
add_subdirectory(bid_ask_snaphot_interface)
add_subdirectory(src/client_list)
add_subdirectory(src/TCP_server)
add_subdirectory(client_lib)
add_subdirectory(include)
add_subdirectory(bid_ask_book)
add_subdirectory(src/console)

add_executable(exc_glass main.cpp)

target_link_libraries(exc_glass database_lib)

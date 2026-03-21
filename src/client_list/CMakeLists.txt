cmake_minimum_required(VERSION 3.14)
project(client_list LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(client_list STATIC
    client_order_manager.cpp
    client_session_manager.cpp
    client_list.cpp
    client_order_manager.h
    client_session_manager.h
    client_list.h
)

target_include_directories(client_list PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
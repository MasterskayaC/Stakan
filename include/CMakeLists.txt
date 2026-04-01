cmake_minimum_required(VERSION 4.2)
project(TCP_client)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src/TCP_client)

add_executable(TCP_client
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/client.cpp
        ${SRC_DIR}/protocol.cpp
        ${INCLUDE_DIR}/tcp_client/types.hpp
        ${INCLUDE_DIR}/tcp_client/client.hpp
        ${INCLUDE_DIR}/tcp_client/protocol.hpp
)

target_include_directories(TCP_client PRIVATE ${INCLUDE_DIR})
cmake_minimum_required(VERSION 3.20)  # версия 4.2 не существует, ставим актуальную 3.20
project(TCP_client)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src/TCP_client)

# Группировка исходников
set(CLIENT_SOURCES
        ${SRC_DIR}/client.cpp
)

set(PROTOCOL_SOURCES
        ${SRC_DIR}/protocol.cpp
)

set(MAIN_SOURCES
        ${SRC_DIR}/main.cpp
)

# Группировка заголовков
set(CLIENT_HEADERS
        ${INCLUDE_DIR}/tcp_client/client.hpp
)

set(PROTOCOL_HEADERS
        ${INCLUDE_DIR}/tcp_client/protocol.hpp
)

set(COMMON_HEADERS
        ${INCLUDE_DIR}/tcp_client/types.hpp
)

add_executable(TCP_client
        ${MAIN_SOURCES}
        ${CLIENT_SOURCES}
        ${PROTOCOL_SOURCES}
        ${CLIENT_HEADERS}
        ${PROTOCOL_HEADERS}
        ${COMMON_HEADERS}
)

target_include_directories(TCP_client PRIVATE ${INCLUDE_DIR})
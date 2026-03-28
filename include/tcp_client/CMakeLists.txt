cmake_minimum_required(VERSION 3.20)

project(tcp_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
endif()



find_package(Boost REQUIRED)

add_library(tcp_client_lib
    ../../src/client.cpp
    ../../src/protocol.cpp
)

target_include_directories(tcp_client_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(tcp_client_lib
    PUBLIC
        Boost::headers
)

add_executable(tcp_client_app
    ../../src/main.cpp
)

target_link_libraries(tcp_client_app
    PRIVATE
        tcp_client_lib
)

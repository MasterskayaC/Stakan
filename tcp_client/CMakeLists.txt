cmake_minimum_required(VERSION 3.14)

find_package(Boost CONFIG REQUIRED COMPONENTS system)

add_library(tcp_client
    ../src/client.cpp
    ../src/protocol.cpp
)

target_include_directories(tcp_client
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(tcp_client
    PUBLIC
        Boost::headers
        Boost::system
)

if(WIN32)
    target_link_libraries(tcp_client PUBLIC ws2_32 wsock32)
endif()

add_executable(tcp_client_demo
    ../src/main.cpp
)

target_link_libraries(tcp_client_demo
    PRIVATE
        tcp_client
)

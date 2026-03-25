add_compile_definitions(_WIN32_WINNT=0x0A00)

set(TCP_CLIENT_BOOST_ROOT "D:/Boost/boost_1_90_0" CACHE PATH "Path to Boost root")

add_library(tcp_client_lib
    src/client.cpp
    src/protocol.cpp
)

target_include_directories(tcp_client_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${TCP_CLIENT_BOOST_ROOT}
)

add_executable(tcp_client_app
    src/main.cpp
)

target_link_libraries(tcp_client_app
    PRIVATE
        tcp_client_lib
)

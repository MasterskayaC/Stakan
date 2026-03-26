cmake_minimum_required(VERSION 3.11)

add_library(client_lib
    order_book_client.cpp
)

target_include_directories(client_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(client_lib
    PUBLIC cxx_std_20
)

add_library(client_lib
    order_book_client.cpp
)

target_include_directories(client_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)


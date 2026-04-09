add_library(client_lib
    order_book_client.cpp
)

target_compile_features(client_lib
    PUBLIC cxx_std_20
)

target_include_directories(client_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(client_lib
    PRIVATE
        tcp_client
)

# TODO: Линк tcp_client. 

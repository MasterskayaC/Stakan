add_library(client_lib
    order_book_client.cpp
)

target_include_directories(client_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

<<<<<<< HEAD
=======
target_compile_features(client_lib
    PUBLIC cxx_std_20
)

# TODO: Линк tcp_client. 
>>>>>>> caaa8df (fix: Fix issues found during review)

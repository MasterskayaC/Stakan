
add_library(bid_ask_book STATIC
       src/bid_ask_book.cpp
       src/bid_ask_book.h
)

target_include_directories(bid_ask_book PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(bid_ask_book PRIVATE
        interface_bid_ask
)

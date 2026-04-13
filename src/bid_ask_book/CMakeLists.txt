enable_testing()

add_library(bid_ask_book STATIC
    bid_ask_book.cpp
    bid_ask_book.h
    logger.cpp
    logger.h
)

target_include_directories(bid_ask_book PUBLIC
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(bid_ask_book PUBLIC
    interface_bid_ask
)

add_executable(bid_ask_book_tests
    tests.cpp
)

target_link_libraries(bid_ask_book_tests PRIVATE
    bid_ask_book
    Catch2::Catch2WithMain
)

add_test(NAME mytest COMMAND bid_ask_book_tests)

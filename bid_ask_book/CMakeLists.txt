cmake_minimum_required(VERSION 3.11)
project(bid_ask_book)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 3 REQUIRED)

set(MAIN_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(bid_ask_book_lib STATIC
        ${MAIN_SOURCE_DIR}/src/bid_ask_book.cpp
        ${MAIN_SOURCE_DIR}/src/bid_ask_book.h
)

target_link_libraries(bid_ask_book_lib PUBLIC
        interface_bid_ask
)

add_executable(bid_ask_book
        ${MAIN_SOURCE_DIR}/src/tests.cpp
)

target_link_libraries(bid_ask_book PRIVATE
        Catch2::Catch2WithMain
        bid_ask_book_lib
)
cmake_minimum_required(VERSION 3.11)
project(bid_ask_book)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

find_package(Catch2 3 REQUIRED)

add_executable(bid_ask_book
        src/bid_ask_book.cpp
        src/bid_ask_book.h
        src/tests.cpp
)

target_link_libraries(bid_ask_book PRIVATE Catch2::Catch2WithMain)

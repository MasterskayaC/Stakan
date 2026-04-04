<<<<<<< HEAD

add_library(bid_ask_book STATIC
       src/bid_ask_book.cpp
       src/bid_ask_book.h
)

target_include_directories(bid_ask_book PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
=======
﻿cmake_minimum_required(VERSION 3.11)
project(bid_ask_book)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 3 CONFIG REQUIRED)

set(MAIN_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(bid_ask_book
        ${MAIN_SOURCE_DIR}/src/bid_ask_book.cpp
        ${MAIN_SOURCE_DIR}/src/bid_ask_book.h
        ${MAIN_SOURCE_DIR}/src/logger.cpp
        ${MAIN_SOURCE_DIR}/src/logger.h
        ${MAIN_SOURCE_DIR}/src/tests.cpp
>>>>>>> 1df5525 (Вынес логгер в отдельный файл)
)

target_link_libraries(bid_ask_book PRIVATE
        interface_bid_ask
)

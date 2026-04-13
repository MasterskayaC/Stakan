cmake_minimum_required(VERSION 3.14)

set(MENU menu.hpp menu.cpp)
set(APP app.hpp app.cpp)
set(MENU_ACTION menu_action.hpp menu_action.cpp)
add_library(cui STATIC
    ${MENU}
    ${APP}
    ${MENU_ACTION}
)

set(CMAKE_CXX_STANDARD 20)

add_executable(cui
    main.cpp
    app.cpp
    snapshot_client.cpp
    menu.cpp
    menu_action.cpp
    ../client_lib/order_book_client.cpp
)

target_include_directories(cui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../client_lib
        ${CMAKE_CURRENT_SOURCE_DIR}/../bid_ask_snaphot_interface
        ${CMAKE_CURRENT_SOURCE_DIR}/../tcp_client
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

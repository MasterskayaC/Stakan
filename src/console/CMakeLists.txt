cmake_minimum_required(VERSION 3.14)

project(ClientConsole)

set(CMAKE_CXX_STANDARD 20)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../)

add_executable(cui
    main.cpp
    app.cpp
    snapshot_client.cpp
    menu.cpp
    menu_action.cpp
    ${PROJECT_ROOT}/client_lib/order_book_client.cpp
)

target_include_directories(cui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_ROOT}/client_lib
        ${PROJECT_ROOT}/bid_ask_snaphot_interface
        ${PROJECT_ROOT}/include
)

cmake_minimum_required(VERSION 3.14)

project(ClientConsole)

set(CMAKE_CXX_STANDARD 20)

add_executable(cui
    main.cpp
    app.cpp
    snapshot_client.cpp
)

target_include_directories(cui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../client_lib
)

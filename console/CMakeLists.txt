cmake_minimum_required(VERSION 3.28)

project(ClientConsole)

set(CMAKE_CXX_STANDARD 20)

add_executable(cui 
    main.cpp
    app.hpp
    app.cpp
    menu.hpp
    menu.cpp
    menu_action.hpp
    menu_action.cpp
)
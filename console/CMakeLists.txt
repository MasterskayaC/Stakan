
add_library(cui STATIC
    app.hpp
    app.cpp
    menu.hpp
    menu.cpp
    menu_action.hpp
    menu_action.cpp
)

target_include_directories( cui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

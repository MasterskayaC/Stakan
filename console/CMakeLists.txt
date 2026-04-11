
set(MENU menu.hpp menu.cpp)
set(APP app.hpp app.cpp)
set(MENU_ACTION menu_action.hpp menu_action.cpp)
add_library(cui STATIC
    ${MENU}
    ${APP}
    ${MENU_ACTION}
)

target_include_directories( cui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

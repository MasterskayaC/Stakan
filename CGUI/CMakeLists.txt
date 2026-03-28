
set(SOURCES
    graphicalui.cpp
)

set(HEADERS
    graphicalui.h
)

set(UI_FILES
    mainwindow.ui
)

add_library(gui STATIC ${SOURCES} ${HEADERS} ${UI_FILES})

target_link_libraries(gui Qt6::Widgets)

target_include_directories(gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


set(SOURCES
    graphicalui.cpp
)

set(HEADERS
    graphicalui.h
)

set(UI_FILES
    mainwindow.ui
)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_library(gui STATIC ${SOURCES} ${HEADERS} ${UI_FILES})

target_link_libraries(gui Qt6::Widgets)

target_include_directories(gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

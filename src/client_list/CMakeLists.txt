add_library(client_list STATIC
    client_list.cpp
    client_list.h
)

target_include_directories(client_list PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR})

add_library(snapshots_broadcaster INTERFACE)

target_include_directories(snapshots_broadcaster
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

add_library(dom_manager STATIC)
target_sources(dom_manager PRIVATE
    dom_manager.cpp
)

target_include_directories(dom_manager PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(dom_manager
    PUBLIC
        client_list
    PRIVATE
        Boost::headers
        Threads::Threads
)

add_executable(dom_manager_exe
    main.cpp
)

target_link_libraries(dom_manager_exe
    PRIVATE
        dom_manager
        client_list
        interface_bid_ask
        Boost::headers
        Threads::Threads
)

if(WIN32)
    target_link_libraries(dom_manager_exe PRIVATE ws2_32)
endif()

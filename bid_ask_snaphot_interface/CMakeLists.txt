add_library(interface_bid_ask STATIC
bid_ask_interface.cpp)

target_include_directories(interface_bid_ask PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS} )

target_link_libraries(interface_bid_ask
    PUBLIC
       ${Boost_LIBRARIES}
       Threads::Threads
)

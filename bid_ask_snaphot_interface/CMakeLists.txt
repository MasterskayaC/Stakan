add_library(interface_bid_ask INTERFACE
bid_ask_interface.h)

target_include_directories(interface_bid_ask INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(interface_bid_ask
    INTERFACE
       Boost::serialization
       Boost::disable_autolinking
       Threads::Threads
)

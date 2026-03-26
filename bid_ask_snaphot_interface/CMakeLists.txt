find_package(Boost REQUIRED COMPONENTS serialization)
find_package(Threads REQUIRED)

add_library(interface_bid_ask INTERFACE)

target_include_directories(interface_bid_ask
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(interface_bid_ask
    INTERFACE
        Boost::headers
        Boost::serialization
        Threads::Threads
)

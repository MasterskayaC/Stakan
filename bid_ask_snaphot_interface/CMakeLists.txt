enable_testing()

add_library(interface_bid_ask INTERFACE
    bid_ask_interface.h
)

target_include_directories(interface_bid_ask INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(interface_bid_ask_tests
    tests.cpp
)

target_link_libraries(interface_bid_ask_tests PRIVATE
    interface_bid_ask
    Catch2::Catch2WithMain
)

add_test(NAME test COMMAND interface_bid_ask_tests)

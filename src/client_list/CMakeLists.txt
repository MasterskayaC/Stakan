set(CLIENT_LIST client_list.cpp client_list.h)
add_library(client_list STATIC
    ${CLIENT_LIST}
)
target_link_libraries(client_list PUBLIC
snapshots_broadcaster
interface_bid_ask
TCPServer
Boost::headers
Boost::boost)

target_include_directories(client_list PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR})
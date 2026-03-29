add_library(tcp_client INTERFACE
tcp_client/client.hpp
tcp_client/protocol.hpp
tcp_client/types.hpp)

target_include_directories(tcp_client INTERFACE
${CMAKE_CURRENT_SOURCE_DIR}/tcp_client
)
add_library(TCPServer STATIC
    include/session.h
    include/tcpserver.h
    src/session.cpp
    src/tcpserver.cpp)


target_include_directories(TCPServer PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/include/
${Boost_INCLUDE_DIRS})

target_link_libraries(TCPServer PUBLIC ${Boost_LIBRARIES})

if(WIN32)
    target_link_libraries(TCPServer ws2_32 wsock32)
endif()



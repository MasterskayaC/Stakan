set(SESSION_FILES
    include/session.h
    src/session.cpp
)

set(TCPSERVER_FILES
    include/tcpserver.h
    src/tcpserver.cpp
)

add_library(TCPServer STATIC ${TCPSERVER_FILES} ${SESSION_FILES}) 

target_include_directories(TCPServer PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/include/
${Boost_INCLUDE_DIRS})

target_link_libraries(TCPServer PUBLIC ${Boost_LIBRARIES})

if(WIN32)
    target_link_libraries(TCPServer ws2_32 wsock32)
endif()



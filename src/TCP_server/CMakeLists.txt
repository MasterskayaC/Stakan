cmake_minimum_required(VERSION 3.11)

set(SESSION_FILES
        session.h
        session.cpp
)

set(TCPSERVER_FILES
        tcpserver.h
        tcpserver.cpp
)

add_library(TCPServer STATIC ${TCPSERVER_FILES} ${SESSION_FILES})

target_include_directories(TCPServer PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src/client_list
        ${CMAKE_SOURCE_DIR}/snapshots_broadcaster
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(TCPServer PUBLIC
        ${Boost_LIBRARIES}
        client_list
        dom_manager
)

if(WIN32)
    target_link_libraries(TCPServer PUBLIC ws2_32 wsock32)
endif()

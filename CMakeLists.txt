cmake_minimum_required(VERSION 3.16)

project(TCPServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("C:/include/boost_1_90_0")
link_directories("C:/include/boost_1_90_0/stage/lib")

add_executable(TCPServer
    main.cpp
    include/session.h
    include/tcpserver.h
    src/session.cpp
    src/tcpserver.cpp)

include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(TCPServer ${Boost_LIBRARIES})

if(WIN32)
    target_link_libraries(TCPServer ws2_32 wsock32)
endif()

include(GNUInstallDirs)
install(TARGETS TCPServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

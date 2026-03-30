cmake_minimum_required(VERSION 3.11)

project(TCPServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.90.0 REQUIRED COMPONENTS)

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

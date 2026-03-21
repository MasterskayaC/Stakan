cmake_minimum_required(VERSION 3.16)

project(TCPServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BOOST_ROOT "C:/include/boost_1_90_0")
find_package(Boost REQUIRED)

add_executable(TCPServer
    main.cpp
    include/session.h
    include/tcpserver.h
    src/session.cpp
    src/tcpserver.cpp)

target_include_directories(TCPServer PUBLIC ${BOOST_ROOT})

if(WIN32)
    target_link_libraries(TCPServer ws2_32 wsock32)
endif()

include(GNUInstallDirs)
install(TARGETS TCPServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

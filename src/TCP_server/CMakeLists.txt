cmake_minimum_required(VERSION 3.16)

project(TCPServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost CONFIG REQUIRED COMPONENTS serialization system)

add_executable(TCPServer
    main.cpp
    include/session.h
    include/tcpserver.h
    src/session.cpp
    src/tcpserver.cpp)

target_include_directories(TCPServer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(TCPServer
    PRIVATE
        Boost::headers
        Boost::serialization
        Boost::system
)

if(WIN32)
    target_link_libraries(TCPServer PRIVATE ws2_32 wsock32)
endif()

include(GNUInstallDirs)
install(TARGETS TCPServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

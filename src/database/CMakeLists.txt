cmake_minimum_required(VERSION 3.11)
project(database_lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(libpqxx REQUIRED)

add_library(database_lib STATIC
        IDBConnector.h
        PostgresConnector.h
        PostgresConnector.cpp
)

target_link_libraries(database_lib libpqxx::pqxx)
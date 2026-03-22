cmake_minimum_required(VERSION 3.11)
project(database_lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(database_lib STATIC
        IDBConnector.h
)

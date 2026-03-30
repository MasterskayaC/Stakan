cmake_minimum_required(VERSION 3.20)
project(orderbook_mongodb_connector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Исходники в UTF-8; без этого MSVC кладёт строки в exe неверно, в консоли «кракозябры».
if(MSVC)
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/utf-8>")
endif()

# Проверка, что компилятор и CMake вообще собирают проект (без MongoDB / vcpkg).
add_executable(compile_test src/compile_test.cpp)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows")
endif()

find_package(mongocxx CONFIG QUIET)
if(mongocxx_FOUND)
    add_library(ob_mongodb STATIC
        src/mongodb_connector.cpp
    )
    target_include_directories(ob_mongodb PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(ob_mongodb PUBLIC mongo::mongocxx_shared)

    add_executable(mongodb_smoke src/smoke_main.cpp)
    target_link_libraries(mongodb_smoke PRIVATE ob_mongodb)
else()
    message(STATUS "mongocxx не найден — цели ob_mongodb и mongodb_smoke пропущены (есть compile_test).")
endif()

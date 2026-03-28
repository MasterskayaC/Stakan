add_library(serialization STATIC
    serialization.cpp
    serialization.h
)

target_link_libraries(serialization PUBLIC Boost::serialization)

target_include_directories(serialization PUBLIC .)

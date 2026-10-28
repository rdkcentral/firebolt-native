# Core client library
add_library(firebolt-core-client SHARED
    src/CoreClient.cpp
)

target_include_directories(firebolt-core-client PUBLIC
    include
    ${CMAKE_SOURCE_DIR}/include
)

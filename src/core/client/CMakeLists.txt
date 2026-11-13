# Core client library
add_library(firebolt-core-client SHARED
    src/FireboltClient.cpp
    src/JsonRpcBuilder.cpp
)

target_include_directories(firebolt-core-client PUBLIC
    include
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(firebolt-core-client PUBLIC
    firebolt-common
    firebolt-proto
)

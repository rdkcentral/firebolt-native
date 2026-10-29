# Example transport library
add_library(firebolt-example-transport SHARED
    src/ExampleTransport.cpp
)

target_include_directories(firebolt-example-transport PUBLIC
    include
    ${CMAKE_SOURCE_DIR}/include
)

cmake_minimum_required(VERSION 3.15)

# Example transport library
add_library(firebolt-example-transport SHARED
    src/ExampleTransport.cpp
)

target_include_directories(firebolt-example-transport PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Set C++ standard
target_compile_features(firebolt-example-transport PUBLIC cxx_std_17)

cmake_minimum_required(VERSION 3.15)

# Secure Transport Library
add_library(firebolt-secure-transport
    src/SecureTransport.cpp
)

target_include_directories(firebolt-secure-transport PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(firebolt-secure-transport PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set C++ standard
target_compile_features(firebolt-secure-transport PUBLIC cxx_std_17)

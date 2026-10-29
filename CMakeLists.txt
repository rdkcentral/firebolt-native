cmake_minimum_required(VERSION 3.15)
project(firebolt-native VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include compiler flags
include(cmake/CompilerFlags.cmake)

# Common utilities
add_subdirectory(src/common)

# Surface clients
add_subdirectory(src/core)
add_subdirectory(src/crypto)
add_subdirectory(src/media)

# Transports
add_subdirectory(transports/example)

# Integration tests
add_subdirectory(tests/integration)

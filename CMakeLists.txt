cmake_minimum_required(VERSION 3.15)
project(firebolt-native VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Protobuf
find_package(Protobuf REQUIRED)

# Find abseil (required for Protobuf 7.x, may be provided by build system)
find_package(absl QUIET)
if(NOT absl_FOUND)
    message(STATUS "absl not found - assuming Protobuf provides required symbols or build system provides them")
endif()

# Find nlohmann/json
find_package(nlohmann_json 3.2.0 QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found - trying to find manually")
    # Try to find it in Homebrew or system paths
    find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp
        PATHS
            /opt/homebrew/Cellar/nlohmann-json/3.12.0/include
            /opt/homebrew/include
            /usr/local/include
            /usr/include
    )
    if(NLOHMANN_JSON_INCLUDE_DIR)
        message(STATUS "Found nlohmann/json at ${NLOHMANN_JSON_INCLUDE_DIR}")
        add_library(nlohmann_json::nlohmann_json INTERFACE IMPORTED)
        target_include_directories(nlohmann_json::nlohmann_json INTERFACE ${NLOHMANN_JSON_INCLUDE_DIR})
    else()
        message(WARNING "nlohmann/json not found - using FetchContent")
        include(FetchContent)
        FetchContent_Declare(json
            URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
        )
        FetchContent_MakeAvailable(json)
    endif()
endif()

# Include compiler flags
include(cmake/CompilerFlags.cmake)

# Enable tests before adding subdirectories so each one's add_test() is registered
enable_testing()

# Protobuf definitions
add_subdirectory(src/proto)

# Common utilities
add_subdirectory(src/common)

# Surface clients
add_subdirectory(src/core)
add_subdirectory(src/secure/core/client)
add_subdirectory(src/crypto)
add_subdirectory(src/media)

# Transports
add_subdirectory(transports/example)
add_subdirectory(transports/secure)

# Integration tests
add_subdirectory(tests/integration)

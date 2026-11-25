cmake_minimum_required(VERSION 3.15)
project(firebolt-native VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add cmake directory to CMake module path for find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Add /usr/local to CMAKE prefix path for locally installed libraries
# This must come before find_package so it finds our installed Protobuf
list(APPEND CMAKE_PREFIX_PATH "/usr/local")
set(CMAKE_LIBRARY_PATH "/usr/local/lib" ${CMAKE_LIBRARY_PATH})
set(CMAKE_INCLUDE_PATH "/usr/local/include" ${CMAKE_INCLUDE_PATH})

# Find Protobuf
find_package(Protobuf REQUIRED HINTS /usr/local)
if(NOT Protobuf_FOUND)
    set(_firebolt_saved_module_path "${CMAKE_MODULE_PATH}")
    list(REMOVE_ITEM CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
    find_package(Protobuf REQUIRED MODULE)
    set(CMAKE_MODULE_PATH "${_firebolt_saved_module_path}")
    unset(_firebolt_saved_module_path)
endif()

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

# Find EyilProtocol and EyilClient from eyil-secure-gateway (optional)
find_package(EyilProtocol QUIET)
find_package(EyilClient QUIET)

# Set a flag to indicate whether eyil is available
if(EyilProtocol_FOUND AND EyilClient_FOUND)
    set(BUILD_WITH_EYIL TRUE)
    message(STATUS "EyilProtocol and EyilClient found - building with eyil support")
else()
    set(BUILD_WITH_EYIL FALSE)
    message(STATUS "EyilProtocol or EyilClient not found - skipping eyil-dependent components")
endif()

# Include compiler flags
include(cmake/CompilerFlags.cmake)

# Enable tests before adding subdirectories so each one's add_test() is registered
enable_testing()

# Common utilities
add_subdirectory(src/common)

# Surface clients
add_subdirectory(src/core)
if(BUILD_WITH_EYIL)
    add_subdirectory(src/secure/core/client)
    add_subdirectory(src/crypto)
    add_subdirectory(src/media)
endif()

# Transports
add_subdirectory(transports/example)
if(BUILD_WITH_EYIL)
    add_subdirectory(transports/secure)
endif()

# WebSocket transport (optional, requires WebSocket++ and Asio)
find_package(WebSocketPP QUIET)
find_package(Asio QUIET)
if(WebSocketPP_FOUND AND Asio_FOUND)
    add_subdirectory(transports/websocket)
else()
    message(STATUS "WebSocket++ or Asio not found - skipping WebSocket transport")
endif()

# Mock WebSocket server for integration tests (optional, requires WebSocket++ and Asio)
if(WebSocketPP_FOUND AND Asio_FOUND AND BUILD_TESTING)
    add_executable(mock_websocket_server
        tests/integration/mock_websocket_server.cpp
    )
    
    target_include_directories(mock_websocket_server PRIVATE
        ${WebSocketPP_INCLUDE_DIRS}
        ${Asio_INCLUDE_DIRS}
    )
    
    target_compile_definitions(mock_websocket_server PRIVATE
        ASIO_STANDALONE
        WEBSOCKETPP_STANDALONE_ASIO
    )
    
    target_link_libraries(mock_websocket_server PRIVATE ssl crypto)
endif()

# Integration tests (includes both Eyil-dependent and WebSocket-only tests)
add_subdirectory(tests/integration)

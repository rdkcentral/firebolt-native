cmake_minimum_required(VERSION 3.15)

# Add cmake directory to CMake module path for find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Find WebSocket++ (header-only)
find_package(WebSocketPP REQUIRED)

# Find Asio (standalone)
find_package(Asio REQUIRED)

# WebSocket Transport Library
add_library(firebolt-core-transport SHARED
    src/WebSocketTransport.cpp
)

target_include_directories(firebolt-core-transport PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(firebolt-core-transport PRIVATE
    ${WebSocketPP_INCLUDE_DIRS}
    ${Asio_INCLUDE_DIRS}
)

# Set C++ standard
target_compile_features(firebolt-core-transport PUBLIC cxx_std_17)

# Define ASIO_STANDALONE and WEBSOCKETPP_STANDALONE_ASIO for standalone Asio
target_compile_definitions(firebolt-core-transport PRIVATE 
    ASIO_STANDALONE
    WEBSOCKETPP_STANDALONE_ASIO
)

# Unit tests
if(BUILD_TESTING)
    add_executable(test_websocket_transport
        tests/unit/test_websocket_transport.cpp
    )

    target_link_libraries(test_websocket_transport PRIVATE
        firebolt-core-transport
        ssl
        crypto
    )

    target_include_directories(test_websocket_transport PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    # Define standalone Asio for test compilation
    target_compile_definitions(test_websocket_transport PRIVATE
        ASIO_STANDALONE
        WEBSOCKETPP_STANDALONE_ASIO
    )

    add_test(NAME test_websocket_transport COMMAND test_websocket_transport)
endif()

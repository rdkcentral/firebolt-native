# Integration tests
enable_testing()

# Find WebSocket++ and Asio for mock WebSocket server
find_package(WebSocket++ QUIET)
find_package(Asio QUIET)

# Mock WebSocket server (doesn't require GTest)
if(WebSocket++_FOUND AND Asio_FOUND)
    add_executable(mock_websocket_server
        mock_websocket_server.cpp
    )

    target_include_directories(mock_websocket_server PRIVATE
        ${WebSocket++_INCLUDE_DIRS}
        ${Asio_INCLUDE_DIRS}
    )
else()
    message(STATUS "WebSocket++ or Asio not found - skipping mock WebSocket server")
endif()

# Find gtest
find_package(GTest QUIET)

if(GTest_FOUND)
    # Core path integration tests
    add_executable(test_core_lifecycle
        test_core_lifecycle.cpp
    )

    target_link_libraries(test_core_lifecycle PRIVATE
        firebolt-core-client
        firebolt-common
        EyilProtocol::EyilProtocol
        firebolt-example-transport
        GTest::gtest
        GTest::gtest_main
    )

    target_include_directories(test_core_lifecycle PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    add_test(NAME test_core_lifecycle COMMAND test_core_lifecycle)

    # Secure path and core/secure parity integration tests
    add_executable(test_secure_lifecycle
        test_secure_lifecycle.cpp
    )

    target_link_libraries(test_secure_lifecycle PRIVATE
        firebolt-core-client
        firebolt-secure-client
        firebolt-common
        EyilProtocol::EyilProtocol
        GTest::gtest
        GTest::gtest_main
    )

    target_include_directories(test_secure_lifecycle PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    add_test(NAME test_secure_lifecycle COMMAND test_secure_lifecycle)

    # WebSocket transport integration tests
    add_executable(test_websocket_lifecycle
        test_websocket_lifecycle.cpp
    )

    target_link_libraries(test_websocket_lifecycle PRIVATE
        firebolt-core-client
        firebolt-common
        firebolt-proto
        GTest::gtest
        GTest::gtest_main
    )

    target_include_directories(test_websocket_lifecycle PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    add_test(NAME test_websocket_lifecycle COMMAND test_websocket_lifecycle)
else()
    message(STATUS "GTest not found - skipping integration tests")
endif()

# Integration tests
enable_testing()

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
        firebolt-proto
        firebolt-example-transport
        GTest::gtest
        GTest::gtest_main
    )

    target_include_directories(test_core_lifecycle PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    add_test(NAME test_core_lifecycle COMMAND test_core_lifecycle)
else()
    message(STATUS "GTest not found - skipping integration tests")
endif()

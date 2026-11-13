# Secure client library (JSON-RPC wrapped in Protobuf over buffer transport)
add_library(firebolt-secure-client SHARED
    src/FireboltSecureClient.cpp
    src/EnvelopeWrapper.cpp
)

target_include_directories(firebolt-secure-client PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_include_directories(firebolt-secure-client PRIVATE
    include
)

target_link_libraries(firebolt-secure-client PUBLIC
    firebolt-common
    firebolt-proto
)

# Unit tests
find_package(GTest QUIET)

if(GTest_FOUND)
    add_executable(test_secure_client
        tests/unit/test_secure_client.cpp
    )

    target_link_libraries(test_secure_client PRIVATE
        firebolt-secure-client
        firebolt-common
        firebolt-proto
        GTest::gtest
        GTest::gtest_main
    )

    target_include_directories(test_secure_client PRIVATE
        include
    )

    add_test(NAME test_secure_client COMMAND test_secure_client)
else()
    message(STATUS "GTest not found - skipping secure client unit tests")
endif()

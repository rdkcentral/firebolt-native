# Crypto client library
add_library(firebolt-crypto-client SHARED
    src/CryptoClient.cpp
)

target_include_directories(firebolt-crypto-client PUBLIC
    include
    ${CMAKE_SOURCE_DIR}/include
)

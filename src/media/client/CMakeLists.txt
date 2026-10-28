# Media client library
add_library(firebolt-media-client SHARED
    src/MediaClient.cpp
)

target_include_directories(firebolt-media-client PUBLIC
    include
    ${CMAKE_SOURCE_DIR}/include
)

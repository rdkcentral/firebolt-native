# Common utilities as CMake object library
add_library(firebolt-common OBJECT
    src/Logging.cpp
    src/Utils.cpp
)

target_include_directories(firebolt-common PUBLIC
    include
)

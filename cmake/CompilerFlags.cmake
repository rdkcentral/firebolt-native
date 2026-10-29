# Compiler flags for firebolt-native

# Warning flags
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Position-independent code for shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Protobuf definitions and code generation

# Generate protobuf files for rpc.proto
protobuf_generate_cpp(RPC_PROTO_SRCS RPC_PROTO_HDRS rpc.proto)

# Generate protobuf files for core.proto
protobuf_generate_cpp(CORE_PROTO_SRCS CORE_PROTO_HDRS core.proto)

# Set output directories for generated files
set_source_files_properties(${RPC_PROTO_SRCS} ${CORE_PROTO_SRCS} PROPERTIES
    GENERATED TRUE
)

# Create protobuf library
add_library(firebolt-proto SHARED
    ${RPC_PROTO_SRCS}
    ${CORE_PROTO_SRCS}
)

# Set include directories
# Generated headers are located in CMAKE_CURRENT_BINARY_DIR and exported as an include root
target_include_directories(firebolt-proto PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${Protobuf_INCLUDE_DIRS}
)

# Link against protobuf library
target_link_libraries(firebolt-proto PUBLIC
    protobuf::libprotobuf
)

# Link against abseil if available (required for Protobuf 7.x on macOS, 
# may be provided by build system or Protobuf in Yocto)
if(absl_FOUND)
    target_link_libraries(firebolt-proto PUBLIC
        absl::check
        absl::log
        absl::base
    )
    # Disable DCHECK to avoid complex abseil dependencies
    target_compile_definitions(firebolt-proto PRIVATE
        NDEBUG
    )
endif()

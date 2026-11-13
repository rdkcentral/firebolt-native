# Protobuf definitions and code generation

# Set output directories for generated protobuf files
set(PROTO_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(PROTO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/firebolt/proto")

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

# Copy generated headers to include/firebolt/proto/ after generation
foreach(hdr ${RPC_PROTO_HDRS} ${CORE_PROTO_HDRS})
    get_filename_component(filename ${hdr} NAME)
    add_custom_command(TARGET firebolt-proto POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${hdr}
        ${PROTO_INCLUDE_DIR}/${filename}
        COMMENT "Copying ${filename} to include/firebolt/proto/"
    )
endforeach()

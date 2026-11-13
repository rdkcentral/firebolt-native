# Protobuf definitions and code generation

# Generate protobuf files for rpc.proto
protobuf_generate_cpp(RPC_PROTO_SRCS RPC_PROTO_HDRS rpc.proto)

# Generate protobuf files for core.proto
protobuf_generate_cpp(CORE_PROTO_SRCS CORE_PROTO_HDRS core.proto)

# Create include hierarchy in binary directory to match expected includes
# The code includes these as <firebolt/proto/rpc.pb.h>, but protobuf_generate_cpp
# outputs them to CMAKE_CURRENT_BINARY_DIR. We need to create the hierarchy.
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/firebolt/proto")

# Create custom commands to copy generated headers to the expected location
# Only copy headers, not source files (sources are compiled from original location)
set(PROTO_HIERARCHY_HEADERS "")
foreach(HEADER_FILE ${RPC_PROTO_HDRS} ${CORE_PROTO_HDRS})
    get_filename_component(HEADER_NAME ${HEADER_FILE} NAME)
    set(DEST_HEADER "${CMAKE_CURRENT_BINARY_DIR}/firebolt/proto/${HEADER_NAME}")
    
    add_custom_command(
        OUTPUT "${DEST_HEADER}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${HEADER_FILE}" "${DEST_HEADER}"
        DEPENDS "${HEADER_FILE}"
        COMMENT "Creating include hierarchy for ${HEADER_NAME}"
    )
    list(APPEND PROTO_HIERARCHY_HEADERS "${DEST_HEADER}")
endforeach()

# Set output directories for generated files
set_source_files_properties(${RPC_PROTO_SRCS} ${CORE_PROTO_SRCS} PROPERTIES
    GENERATED TRUE
)

# Set properties on hierarchy headers
set_source_files_properties(${PROTO_HIERARCHY_HEADERS} PROPERTIES
    GENERATED TRUE
    HEADER_FILE_ONLY TRUE
)

# Create protobuf library
# Use only original source files, copy headers to hierarchy for includes
add_library(firebolt-proto SHARED
    ${RPC_PROTO_SRCS}
    ${CORE_PROTO_SRCS}
    ${PROTO_HIERARCHY_HEADERS}
)

# Set include directories
# Generated headers are located in CMAKE_CURRENT_BINARY_DIR and exported as an include root
# The hierarchy (firebolt/proto/) is created by custom commands above
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

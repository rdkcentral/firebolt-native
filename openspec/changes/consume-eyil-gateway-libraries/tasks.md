# Tasks

## 1. CMake Dependency Updates

- [x] 1.1 Add find_package for EyilProtocol and EyilClient to root CMakeLists.txt and verify cmake configuration succeeds when packages are available
- [x] 1.2 Remove add_subdirectory(src/proto) from root CMakeLists.txt and verify the proto subdirectory is no longer part of the build
- [x] 1.3 Update firebolt-secure-client CMakeLists.txt to link EyilProtocol::EyilProtocol instead of firebolt-proto and verify the target links correctly
- [x] 1.4 Update transports/secure CMakeLists.txt to link EyilClient::EyilClient for secure transport and verify the target links correctly
- [x] 1.5 Remove firebolt-proto target references from all CMakeLists.txt files and verify no targets reference the removed library

## 2. Include Path Updates

- [x] 2.1 Search and replace all occurrences of `#include "firebolt/proto/` with `#include "eyilprotocol/` in source files and verify grep finds no remaining old includes
- [x] 2.2 Update enum references from RPC_SERVICE_JSON_RPC to RPC_SERVICE_CORE in EnvelopeWrapper.cpp and verify the code compiles with the new enum name
- [x] 2.3 Remove src/proto/CMakeLists.txt file and verify the file is deleted from the repository

## 3. Remove Duplicated Proto Files

- [x] 3.1 Delete src/proto/rpc.proto file and verify the file no longer exists
- [x] 3.2 Delete src/proto/core.proto file and verify the file no longer exists
- [x] 3.3 Delete src/crypto/client/proto/rpc.proto file and verify the file no longer exists
- [x] 3.4 Delete src/crypto/client/proto/crypto.proto file and verify the file no longer exists
- [x] 3.5 Delete src/proto/ directory and verify the directory is removed
- [x] 3.6 Delete src/crypto/client/proto/ directory and verify the directory is removed

## 4. Docker Build Setup

- [x] 4.1 Create Dockerfile with Ubuntu 24.04 base image and required dependencies (cmake, ninja, g++, protobuf-compiler, libprotobuf-dev, libzmq3-dev, libssl-dev) and verify docker build succeeds
- [x] 4.2 Create build script that builds eyil-secure-gateway with -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=lib, installs it, then builds firebolt-native and verify the script completes without errors
- [x] 4.3 Update README.md with docker build instructions documenting the two-stage build process and verify the documented steps are clear and accurate

## 5. Validation

- [x] 5.1 Run full build in docker environment and verify all targets build successfully
- [x] 5.2 Run existing unit tests (test_secure_client, test_core_lifecycle, test_secure_lifecycle) and verify all tests pass
- [x] 5.3 Run existing integration tests and verify all integration tests pass
- [x] 5.4 Verify that the build links against the installed EyilProtocol and EyilClient libraries by checking ldd output or CMake link properties

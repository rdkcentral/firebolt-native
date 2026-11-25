# Proposal

## Why

Firebolt-native currently maintains duplicate protobuf definitions (rpc.proto, core.proto, crypto.proto) instead of consuming the canonical versions from eyil-secure-gateway's EyilProtocol package. This creates maintenance burden and risks divergence between the two codebases. The goal is to establish eyil-secure-gateway as the single source of truth for these proto definitions and consume its EyilProtocol and EyilClient libraries.

## What Changes

- Remove duplicated proto files from firebolt-native:
  - `src/proto/rpc.proto`
  - `src/proto/core.proto`
  - `src/crypto/client/proto/rpc.proto`
  - `src/crypto/client/proto/crypto.proto`
- Remove protobuf generation CMake code from firebolt-native
- Add CMake dependencies on EyilProtocol and EyilClient from eyil-secure-gateway:
  - `find_package(EyilProtocol REQUIRED)`
  - `find_package(EyilClient REQUIRED)`
- Update CMakeLists.txt to link against `EyilProtocol::EyilProtocol` and `EyilClient::EyilClient`
- Create Dockerfile similar to eyil-secure-gateway for local Linux builds
- Add build documentation for the two-stage docker build process (build eyil-secure-gateway with install, then build firebolt-native)
- Update include paths in code to use installed protobuf headers from eyilprotocol package

## Capabilities

None. This change is a build dependency refactoring that removes duplicated code. It does not change firebolt-native's external behavior or API contracts. The existing `transport-interface` capability remains unchanged as this only affects how protobuf bindings are built and linked.

## Impact

- **src/proto/**: Remove duplicated proto files
- **src/crypto/client/proto/**: Remove duplicated proto files
- **CMakeLists.txt**: Add find_package for EyilProtocol and EyilClient, remove local protobuf generation
- **Dockerfile**: New file for Linux build environment (similar to eyil-secure-gateway)
- **README.md**: Update build documentation for docker-based local builds
- **eyil-secure-gateway**: No changes (upstream dependency, unaware of firebolt-native)
- **Compatibility**: Non-breaking for firebolt-native consumers. This is an internal build refactoring.

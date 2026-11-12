# Design

## Context

The firebolt-native repository is currently empty aside from OpenSpec configuration. The project needs to support multiple Firebolt surfaces (Core, Crypto, Media) with a flexible transport architecture. Each surface uses different protocols (JSON-RPC over WebSocket for Core, Protobuf over Unix Domain Socket for Crypto and Media). The build system must support Yocto builds with manual CMake find modules and out-of-tree builds.

See proposal.md for motivation.

## Goals / Non-Goals

**Goals:**
- Establish CMake-based C++ 17 project structure that can be built out-of-tree
- Create directory layout enabling independent development of each surface
- Implement pure header-only `IFireboltTransport` interface for runtime dependency injection
- Provide simple example implementations demonstrating the architecture without external dependencies
- Set up unit test structure alongside components
- Create integration test directory structure
- Add CMake modules for manual dependency finding (for future Yocto compatibility)

**Non-Goals:**
- Implementing actual transport protocols (WebSocket, ZMQ, Soup, Protobuf, gRPC)
- Implementing actual surface-specific APIs
- Setting up actual dependency management (Conan, vcpkg)
- Creating header generator (separate proposal)

## Decisions

### Directory Structure

Organize by surface with separate client libraries, shared transports, and common utilities:

```
firebolt-native/
├── CMakeLists.txt
├── cmake/
│   ├── CompilerFlags.cmake
│   └── [Future Find*.cmake for dependencies]
├── include/
│   └── firebolt/
│       └── IFireboltTransport.h
├── src/
│   ├── common/                    # CMake object library
│   ├── core/
│   │   ├── client/
│   │   └── generated/
│   ├── crypto/
│   │   ├── client/
│   │   └── generated/
│   └── media/
│       ├── client/
│       └── generated/
├── transports/
│   ├── example/                   # Simple example transport
└── tests/
    └── integration/
```

**Rationale:** Separates concerns - surfaces own their client code, transports are protocol-specific, common utilities are shared. Unit tests alongside components enables isolation. Transports as separate libraries allows reuse across surfaces and partner implementations.

**Alternatives considered:**
- Single monolithic library: Rejected - surfaces need independent versioning and testing
- Transports inside surface directories: Rejected - transports should be reusable across surfaces

### Transport Architecture

Use runtime dependency injection via pure header-only interface:

```cpp
// IFireboltTransport.h - pure header-only
class IFireboltTransport {
public:
    virtual ~IFireboltTransport() = default;
    virtual void send(const std::string& message) = 0;
    virtual std::string receive() = 0;
};

// Client accepts transport in constructor
class CoreClient {
public:
    CoreClient(std::shared_ptr<IFireboltTransport> transport);
};
```

**Rationale:** Enables testability (mock transports), partner extensibility (custom transports), and flexible protocol selection at runtime. No compile-time coupling between clients and transports.

**Alternatives considered:**
- Compile-time linking: Rejected - reduces testability and partner flexibility
- Abstract base class with implementation: Rejected - adds unnecessary .so for interface

### CMake Object Library for Common Utilities

Use CMake object library for shared utilities:

```cmake
add_library(firebolt-common OBJECT
    src/common/src/Logging.cpp
    src/common/src/Utils.cpp
)
target_include_directories(firebolt-common PUBLIC
    src/common/include
)
```

**Rationale:** Object libraries compile sources once and link into dependent targets without creating a separate .so file. Avoids versioning complexity while enabling code reuse.

**Alternatives considered:**
- Shared library: Rejected - adds unnecessary versioning overhead for internal utilities
- Header-only: Rejected - some utilities may require linking against system libraries

### Surface Client Libraries

Each surface produces a single .so file:

```
libfirebolt-core-client.so
libfirebolt-crypto-client.so
libfirebolt-media-client.so
```

Each includes:
- Client implementation
- Generated headers
- Exports IFireboltTransport.h

**Rationale:** Simple deployment model. Single .so per surface includes everything needed to use that surface.

**Alternatives considered:**
- Separate .so for generated code: Rejected - adds complexity without clear benefit
- Single .so for all surfaces: Rejected - surfaces need independent versioning

### Transport Libraries

Transports as separate .so files:

```
libfirebolt-example-transport.so
[Future: libfirebolt-ws-transport.so, etc.]
```

**Rationale:** Enables reuse across surfaces and partner implementations. Transports can be versioned independently.

**Alternatives considered:**
- Static linking into clients: Rejected - prevents runtime transport selection and partner custom implementations

### Simple Example Implementation

Implement a simple in-memory transport without external dependencies:

```cpp
// ExampleTransport.h
class ExampleTransport : public IFireboltTransport {
    std::queue<std::string> messageQueue;
public:
    void send(const std::string& message) override;
    std::string receive() override;
};
```

**Rationale:** Demonstrates the architecture without requiring external dependencies like Soup, ZMQ, or Protobuf. Provides a working example for partners to follow.

**Alternatives considered:**
- Implement actual WebSocket transport: Rejected - requires external dependencies, out of scope for initial setup
- No example: Rejected - partners need concrete example to understand the pattern

### Test Structure

Unit tests alongside components:

```
src/core/client/tests/unit/test_client.cpp
src/common/tests/unit/test_utils.cpp
transports/example/tests/unit/test_transport.cpp
```

Integration tests in separate directory:

```
tests/integration/test_core_integration.cpp
```

**Rationale:** Unit tests alongside components enable focused testing. Integration tests separate for cross-component testing.

**Alternatives considered:**
- All tests in single directory: Rejected - harder to locate relevant tests
- No integration tests: Rejected - need to verify surface + transport integration

### CMake Build System

Root CMakeLists.txt with subdirectory structure:

```cmake
cmake_minimum_required(VERSION 3.15)
project(firebolt-native VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src/common)
add_subdirectory(src/core)
add_subdirectory(src/crypto)
add_subdirectory(src/media)
add_subdirectory(transports/example)
add_subdirectory(tests/integration)
```

**Rationale:** Standard CMake organization. Each component has its own CMakeLists.txt for modularity.

**Alternatives considered:**
- Single monolithic CMakeLists.txt: Rejected - harder to maintain as project grows

### License Header Convention

All .h and .cpp source files must include the standard Apache 2.0 license header at the top:

```cpp
/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2026 RDK Management.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
```

**Rationale:** Ensures consistent licensing across the codebase and compliance with RDK Management's licensing requirements. The header clearly states copyright and licensing terms.

**Alternatives considered:**
- No license headers: Rejected - violates RDK Management requirements
- Different license per component: Rejected - adds complexity and confusion
- SPDX identifiers only: Rejected - RDK Management requires full header

## Risks / Trade-offs

**Risk:** CMake object library for common utilities may not work well with all CMake generators
→ **Mitigation:** Test with Ninja and Make generators initially. Document any generator-specific requirements.

**Risk:** Runtime transport injection may confuse developers expecting compile-time linking
→ **Mitigation:** Provide clear documentation and examples showing the pattern. Include integration tests demonstrating the injection.

**Risk:** Directory structure may need adjustment as actual dependencies (Soup, ZMQ, Protobuf) are added
→ **Mitigation:** Keep structure modular. Document rationale for each directory to guide future changes.

**Trade-off:** Simple example transport doesn't demonstrate real-world complexity (async I/O, error handling)
→ **Acceptance:** The example is intentionally simple to avoid external dependencies. Real transport implementations will be added in future changes.

## Migration Plan

No migration needed - this is greenfield project setup.

## Open Questions

None - this is foundational infrastructure with clear requirements.

# Firebolt Native C++ Client Library

This repository contains the Firebolt C++ client library for multiple surfaces.

## Project Structure

```
firebolt-native/
├── CMakeLists.txt                          # Root CMake configuration
├── Dockerfile                              # Docker image for building both implementations
├── docker-compose.yml                      # Docker Compose configuration
├── docker-build.sh                         # Helper script for Docker builds
├── build-docker.sh                         # Script for building with eyil-secure-gateway
├── cmake/                                  # CMake modules
│   ├── CompilerFlags.cmake                # Common compiler flags
│   ├── FindProtobuf.cmake                  # Protobuf library (placeholder)
│   ├── FindGRPC.cmake                      # gRPC library (placeholder)
│   ├── FindZMQ.cmake                       # ZMQ library (placeholder)
│   ├── FindWebSocketPP.cmake               # WebSocket++ library finder
│   └── FindAsio.cmake                      # Asio library finder
│
├── include/                                # Public headers
│   └── firebolt/
│       ├── IFireboltTextTransport.h        # Text-based transport interface (WebSocket)
│       └── IFireboltBufferTransport.h      # Buffer-based transport interface (Protobuf/UDS)
│
├── src/
│   ├── common/                             # Shared utilities
│   │   ├── include/firebolt/common/
│   │   │   ├── Logging.h
│   │   │   └── Utils.h
│   │   └── src/
│   │       ├── Logging.cpp
│   │       └── Utils.cpp
│   │
│   ├── core/                               # Firebolt Core
│   │   ├── client/                         # CoreClient library
│   │   │   ├── include/firebolt/core/
│   │   │   │   └── CoreClient.h
│   │   │   └── src/
│   │   │       └── CoreClient.cpp
│   │   └── generated/                     # Auto-generated headers
│   │
│   ├── crypto/                             # Firebolt Crypto (requires eyil)
│   │   ├── client/                         # CryptoClient library
│   │   │   ├── include/firebolt/crypto/
│   │   │   │   └── CryptoClient.h
│   │   │   └── src/
│   │   │       └── CryptoClient.cpp
│   │   └── generated/                     # Auto-generated headers
│   │
│   └── media/                              # Firebolt Media (requires eyil)
│       ├── client/                         # MediaClient library
│       │   ├── include/firebolt/media/
│       │   │   └── MediaClient.h
│       │   └── src/
│       │       └── MediaClient.cpp
│       └── generated/                     # Auto-generated headers
│
├── transports/                             # Transport implementations
│   ├── example/                           # Example in-memory transport
│   │   ├── include/firebolt/transport/
│   │   │   └── ExampleTransport.h
│   │   └── src/
│   │       └── ExampleTransport.cpp
│   ├── secure/                            # Secure transport (requires eyil)
│   │   ├── include/firebolt/transport/
│   │   │   └── SecureTransport.h
│   │   └── src/
│   │       └── SecureTransport.cpp
│   └── websocket/                         # WebSocket transport
│       ├── include/firebolt/transport/
│       │   └── WebSocketTransport.h
│       ├── src/
│       │   └── WebSocketTransport.cpp
│       └── tests/
│           └── unit/
│               └── test_websocket_transport.cpp
│
└── tests/
    └── integration/                        # Integration tests (requires eyil)
        ├── test_core_lifecycle.cpp
        ├── test_secure_lifecycle.cpp
        └── test_websocket_lifecycle.cpp
```

## Build Instructions

### Quick Start (Recommended)

The easiest way to build and test the project is using Docker with the provided helper scripts:

```bash
# Build WebSocket transport implementation
./docker-build.sh build-ws

# Test WebSocket transport implementation
./docker-build.sh test-ws

# Build Eyil protocol implementation (requires eyil-secure-gateway)
./docker-build.sh build-eyil

# Open interactive shell in container
./docker-build.sh shell
```

### Local Build

**Prerequisites:**
- CMake 3.15 or higher
- C++17 compatible compiler
- Protobuf (found via `find_package(Protobuf REQUIRED)`). Tested with 7.36.2 from Homebrew. No minimum version is enforced yet.

**Building:**

```bash
# Configure the project
cmake -S . -B build

# Build the project
cmake --build build

# Clean build
cmake --build build --clean-first
```

**Building specific libraries:**

```bash
cmake --build build --target firebolt-common firebolt-core-client firebolt-secure-client
```

Outputs (`.dylib` on macOS, `.so` on Linux):

- `build/src/common/libfirebolt-common`
- `build/src/core/client/libfirebolt-core-client`
- `build/src/secure/core/client/libfirebolt-secure-client`

**Out-of-tree build:**

```bash
# Build in a different directory
cmake -S /path/to/firebolt-native -B /tmp/firebolt-build
cmake --build /tmp/firebolt-build
```

### Docker Build

The Dockerfile supports building both the eyil protocol implementation and the websocket transport implementation.

#### Implementation Options

The project supports two transport implementations:

1. **WebSocket Transport** - Local implementation using WebSocket++ and Asio
   - Use for development and testing without external dependencies
   - No additional repositories required
   - Supports JSON-RPC over WebSocket

2. **Eyil Protocol** - Production implementation using eyil-secure-gateway
   - Use for production builds with the full protocol stack
   - Requires access to eyil-secure-gateway repository
   - Supports Protobuf over ZeroMQ

The build system automatically detects which dependencies are available and builds the appropriate components.

#### Building with Eyil Protocol

For local Linux builds that mirror the Yocto environment, use the provided Dockerfile and build script. This is a two-stage process:

1. Build and install eyil-secure-gateway (provides EyilProtocol and EyilClient)
2. Build firebolt-native (consumes the installed packages)

**Prerequisites:**
- Docker installed locally
- Access to eyil-secure-gateway repository

**Using helper script (recommended):**

```bash
./docker-build.sh build-eyil
```

**Manual build steps:**

```bash
# Build the Docker image
docker build -t firebolt-native-build .

# Run the two-stage build
# Set EYIL_GATEWAY_DIR to point to your eyil-secure-gateway checkout
docker run --rm \
    -v /path/to/eyil-secure-gateway:/workspace/eyil-secure-gateway \
    -v /path/to/firebolt-native:/workspace/firebolt-native \
    -v /path/to/firebolt-build:/workspace/build \
    -e EYIL_GATEWAY_DIR=/workspace/eyil-secure-gateway \
    firebolt-native-build \
    /workspace/firebolt-native/build-docker.sh
```

The build script:
1. Configures and builds eyil-secure-gateway with `-DCMAKE_INSTALL_PREFIX=/usr`
2. Installs eyil-secure-gateway to `/usr` in the container
3. Configures and builds firebolt-native (which finds EyilProtocol and EyilClient via `find_package`)

**Note:** The script requires sudo privileges during the eyil-secure-gateway installation step.

#### Building with WebSocket Transport

For building the websocket transport implementation (without eyil), use the Dockerfile directly:

**Using helper script (recommended):**

```bash
# Build
./docker-build.sh build-ws

# Run tests
./docker-build.sh test-ws
```

**Manual build steps:**

```bash
# Build the Docker image
docker build -t firebolt-native-ws .

# Build the project inside the container
docker run --rm \
    -v $(pwd):/workspace \
    firebolt-native-ws \
    bash -c "cd /workspace && mkdir build && cd build && cmake .. -DBUILD_TESTING=ON && make"

# Run the websocket transport tests
docker run --rm \
    -v $(pwd):/workspace \
    firebolt-native-ws \
    bash -c "cd /workspace/build && ./transports/websocket/test_websocket_transport"
```

**Dockerfile includes:**
- WebSocket++ 0.8.2 (header-only library)
- Asio 1.12.2 (standalone, header-only library)
- Protobuf 3.21.12 (for protobuf_generate_cpp support)
- Google Test (for testing)

The CMake build system conditionally builds the websocket transport if WebSocket++ and Asio are found.

#### Using Docker Compose

For more complex workflows, use docker-compose with profiles:

```bash
# Build websocket transport
docker-compose --profile websocket run websocket-build

# Test websocket transport
docker-compose --profile websocket run websocket-test

# Build eyil protocol
docker-compose --profile eyil run eyil-build

# Open interactive shell
docker-compose run shell
```

#### Available Helper Scripts

**docker-build.sh** - Main helper script for Docker builds:

```bash
./docker-build.sh build-ws     # Build websocket transport
./docker-build.sh test-ws      # Test websocket transport
./docker-build.sh build-eyil   # Build eyil protocol (requires eyil-secure-gateway)
./docker-build.sh shell        # Open interactive shell
./docker-build.sh help         # Show help message
```

### Running Tests

Tests use GoogleTest. If it is not installed, CMake skips them with a "GTest not found" message
and ctest reports no tests. On macOS, install it with `brew install googletest`.

#### Running Tests with Docker

The easiest way to run tests is using the helper scripts:

```bash
# Test WebSocket transport
./docker-build.sh test-ws
```

For eyil protocol tests, build first then run ctest:

```bash
# Build with eyil
./docker-build.sh build-eyil

# Run tests in the container
docker run --rm \
    -v $(pwd):/workspace/firebolt-native \
    -v "$(pwd)/build-eyil":/workspace/build \
    firebolt-native-ws \
    bash -c "cd /workspace/build/firebolt-native && ctest --output-on-failure"
```

#### Running Tests Locally

Run the tests with the helper script, which configures, builds, and runs ctest:

```bash
scripts/run-tests.sh                        # all tests
scripts/run-tests.sh unit                   # unit tests only
scripts/run-tests.sh integration            # integration tests only
scripts/run-tests.sh all out-dir            # use another build directory (default: build)
```

The second argument is the build directory, relative to the repository root, and works with every suite.
The script stops at the first failed build step and exits non-zero if any test fails.

| Suite       | Tests (requires eyil)                     | Tests (websocket)          |
|-------------|--------------------------------------------|----------------------------|
| unit        | `test_secure_client`                       | `test_websocket_transport` |
| integration | `test_core_lifecycle`, `test_secure_lifecycle` | -                          |

To run ctest directly, build first and then run it against the build directory:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

**Note:** Integration tests require eyil-secure-gateway to be built and installed. WebSocket transport tests can run without eyil dependencies.

## Transport Architecture

The library uses runtime dependency injection for transports with protocol-specific interfaces:

- **IFireboltTextTransport**: For text-based protocols (e.g., WebSocket with JSON-RPC)
- **IFireboltBufferTransport**: For buffer-based protocols (e.g., Protobuf over Unix Domain Socket)

Each surface client accepts the appropriate transport interface:

```cpp
// Core uses text transport (WebSocket)
#include "firebolt/IFireboltTextTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/transport/ExampleTransport.h"

auto transport = std::make_shared<firebolt::transport::ExampleTransport>();
firebolt::core::FireboltClient client(transport);

// Secure path uses buffer transport (Protobuf envelopes over UDS)
#include "firebolt/IFireboltBufferTransport.h"
#include "firebolt/secure/core/FireboltSecureClient.h"

std::shared_ptr<firebolt::IFireboltBufferTransport> secureTransport = MakeBufferTransport();
firebolt::secure::core::FireboltSecureClient secureClient(secureTransport);
```

This enables:
- Testability with mock transports
- Partner extensibility with custom transports
- Flexible protocol selection at runtime
- Type-safe separation between text and binary protocols

## Adding a New Surface

To add a new surface:

1. Create the directory structure:
   ```
   src/<surface>/
   ├── client/
   │   ├── include/firebolt/<surface>/
   │   └── src/
   └── generated/
   ```

2. Add a CMakeLists.txt in `src/<surface>/`:
   ```cmake
   add_subdirectory(client)
   ```

3. Add a CMakeLists.txt in `src/<surface>/client/`:
   ```cmake
   add_library(firebolt-<surface>-client SHARED
       src/<Surface>Client.cpp
   )

   target_include_directories(firebolt-<surface>-client PUBLIC
       include
       ${CMAKE_SOURCE_DIR}/include
   )
   ```

4. Add the subdirectory to the root CMakeLists.txt:
   ```cmake
   add_subdirectory(src/<surface>)
   ```

5. Create the client header and implementation following the pattern of existing surfaces.

## Implementing a Custom Transport

To implement a custom transport, choose the appropriate interface based on your protocol:

### Text-Based Transport (WebSocket, JSON-RPC)

1. Create the directory structure:
   ```
   transports/<transport-name>/
   ├── include/firebolt/transport/
   └── src/
   ```

2. Add a CMakeLists.txt:
   ```cmake
   add_library(firebolt-<transport-name>-transport SHARED
       src/<TransportName>Transport.cpp
   )

   target_include_directories(firebolt-<transport-name>-transport PUBLIC
       include
       ${CMAKE_SOURCE_DIR}/include
   )
   ```

3. Inherit from `IFireboltTextTransport`:
   ```cpp
   #include "firebolt/IFireboltTextTransport.h"

   namespace firebolt {
   namespace transport {

   class CustomTextTransport : public IFireboltTextTransport {
   public:
       bool open() override;
       bool isConnected() override;
       void onConnectionStatusChanged(
           std::function<void(bool, const std::string&)> callback) override;
       bool send(const std::string& message) override;
       void onMessageReceived(
           std::function<void(const std::string&)> callback) override;
       void disconnect() override;
   };

   } // namespace transport
   } // namespace firebolt
   ```

### Buffer-Based Transport (Protobuf, UDS)

Follow the same structure but inherit from `IFireboltBufferTransport`:

```cpp
#include "firebolt/IFireboltBufferTransport.h"

namespace firebolt {
namespace transport {

class CustomBufferTransport : public IFireboltBufferTransport {
public:
    bool open() override;
    bool isConnected() override;
    void onConnectionStatusChanged(
        std::function<void(bool, const std::string&)> callback) override;
    bool send(const std::vector<uint8_t>& data) override;
    void onMessageReceived(
        std::function<void(const std::vector<uint8_t>&)> callback) override;
    void disconnect() override;
};

} // namespace transport
} // namespace firebolt
```

4. Implement the methods according to your protocol.

## License

All source files must include the Apache 2.0 license header at the top:

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

## License Header Requirement

All new source files (.h and .cpp) must include the Apache 2.0 license header shown above. This ensures consistent licensing across the codebase and compliance with RDK Management's licensing requirements.

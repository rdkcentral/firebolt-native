# Firebolt Native C++ Client Library

This repository contains the Firebolt C++ client library for multiple surfaces.

## Project Structure

```
firebolt-native/
├── CMakeLists.txt                          # Root CMake configuration
├── cmake/                                  # CMake modules
│   ├── CompilerFlags.cmake                # Common compiler flags
│   ├── FindSoup.cmake                      # Soup library (placeholder)
│   ├── FindProtobuf.cmake                  # Protobuf library (placeholder)
│   ├── FindGRPC.cmake                      # gRPC library (placeholder)
│   └── FindZMQ.cmake                       # ZMQ library (placeholder)
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
│   ├── crypto/                             # Firebolt Crypto
│   │   ├── client/                         # CryptoClient library
│   │   │   ├── include/firebolt/crypto/
│   │   │   │   └── CryptoClient.h
│   │   │   └── src/
│   │   │       └── CryptoClient.cpp
│   │   └── generated/                     # Auto-generated headers
│   │
│   └── media/                              # Firebolt Media
│       ├── client/                         # MediaClient library
│       │   ├── include/firebolt/media/
│       │   │   └── MediaClient.h
│       │   └── src/
│       │       └── MediaClient.cpp
│       └── generated/                     # Auto-generated headers
│
├── transports/                             # Transport implementations
│   └── example/                           # Example in-memory transport
│       ├── include/firebolt/transport/
│       │   └── ExampleTransport.h
│       └── src/
│           └── ExampleTransport.cpp
│
└── tests/
    └── integration/                        # Integration tests
```

## Build Instructions

### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler
- Protobuf (found via `find_package(Protobuf REQUIRED)`). Tested with 7.36.2 from Homebrew. No minimum version is enforced yet.

### Building

```bash
# Configure the project
cmake -S . -B build

# Build the project
cmake --build build

# Clean build
cmake --build build --clean-first
```

### Building Libraries

The three client libraries can be built on their own:

```bash
cmake --build build --target firebolt-common firebolt-core-client firebolt-secure-client
```

Outputs (`.dylib` on macOS, `.so` on Linux):

- `build/src/common/libfirebolt-common`
- `build/src/core/client/libfirebolt-core-client`
- `build/src/secure/core/client/libfirebolt-secure-client`

### Out-of-tree Build

The project supports out-of-tree builds:

```bash
# Build in a different directory
cmake -S /path/to/firebolt-native -B /tmp/firebolt-build
cmake --build /tmp/firebolt-build
```

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

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
│       └── IFireboltTransport.h            # Transport interface
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

### Building

```bash
# Configure the project
cmake -S . -B build

# Build the project
cmake --build build

# Clean build
cmake --build build --clean-first
```

### Out-of-tree Build

The project supports out-of-tree builds:

```bash
# Build in a different directory
cmake -S /path/to/firebolt-native -B /tmp/firebolt-build
cmake --build /tmp/firebolt-build
```

## Transport Architecture

The library uses runtime dependency injection for transports. Each surface client accepts an `IFireboltTransport` implementation:

```cpp
#include "firebolt/IFireboltTransport.h"
#include "firebolt/core/CoreClient.h"
#include "firebolt/transport/ExampleTransport.h"

auto transport = std::make_shared<firebolt::transport::ExampleTransport>();
firebolt::core::CoreClient client(transport);
```

This enables:
- Testability with mock transports
- Partner extensibility with custom transports
- Flexible protocol selection at runtime

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

To implement a custom transport:

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

3. Inherit from `IFireboltTransport`:
   ```cpp
   #include "firebolt/IFireboltTransport.h"

   namespace firebolt {
   namespace transport {

   class CustomTransport : public IFireboltTransport {
   public:
       void send(const std::string& message) override;
       std::string receive() override;
   };

   } // namespace transport
   } // namespace firebolt
   ```

4. Implement the send() and receive() methods according to your protocol.

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

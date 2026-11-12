# Proposal

## Why

The current single `IFireboltTransport` interface uses string-based messaging (`send(string)`, `receive()`), which cannot support the protocol requirements of different Firebolt surfaces. Core uses JSON-RPC over WebSocket with text frames, while Crypto and Media use Protobuf over Unix Domain Socket with binary data. A single string-only interface forces inefficient conversions and doesn't match WebSocket's text frame requirement.

## What Changes

- Replace the current single `IFireboltTransport` interface with two specialized interfaces:
  - `IFireboltTextTransport` - for text-based protocols (WebSocket with text frames)
  - `IFireboltBufferTransport` - for buffer-based protocols (Protobuf over UDS)
- Deprecate and remove the existing `IFireboltTransport` interface
- Update all client implementations (CoreClient, CryptoClient, MediaClient) to use the appropriate specialized interface
- Update transport implementations to implement the correct interface
- Ensure all new interface header files include the standard Apache 2.0 license header with the following structure:
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
- Document all interface methods with comprehensive documentation

**BREAKING**: The existing `IFireboltTransport` interface will be removed. All code using it must migrate to either `IFireboltTextTransport` or `IFireboltBufferTransport`.

## Capabilities

### New Capabilities
- `transport-interface`: Defines the dual-transport interface architecture that enables protocol-specific transport implementations (text for WebSocket, binary for Protobuf) with clear separation of concerns and type safety.

### Modified Capabilities
None

## Impact

- **API Headers**: New header files `IFireboltTextTransport.h` and `IFireboltBufferTransport.h` in `include/firebolt/`; existing `IFireboltTransport.h` will be removed
- **Client Libraries**: CoreClient, CryptoClient, and MediaClient will be updated to use the appropriate specialized interface
- **Transport Implementations**: ExampleTransport will be updated to implement `IFireboltTextTransport`; SecureTransport (wrapping SecureGateway) will implement `IFireboltBufferTransport`
- **Build System**: CMake targets will be updated to reflect the new interface files
- **Documentation**: All interface methods will have comprehensive documentation

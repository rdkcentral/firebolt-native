# Design

## Context

The current `IFireboltTransport` interface (see proposal.md - Why) uses string-based messaging only. Firebolt surfaces use different protocols: Core uses JSON-RPC over WebSocket (text frames), while Crypto and Media use Protobuf over Unix Domain Socket (binary). The existing SecureGateway C API already supports both paradigms with separate functions for string and binary messaging. The design doc from the completed cpp-project-layout change established a pure header-only transport interface pattern, but the interface itself needs to be refined to support both text and binary protocols.

## Goals / Non-Goals

**Goals:**
- Create two specialized transport interfaces (text and buffer) that match protocol requirements
- Maintain the pure header-only interface pattern established in cpp-project-layout
- Ensure type safety and clear separation between text and binary transports
- Support the existing SecureGateway C API's dual-mode capability
- Enable WebSocket implementations to use text frames and UDS implementations to use binary

**Non-Goals:**
- Implementing actual WebSocket or UDS transports (future work)
- Changing the client library architecture beyond updating to use new interfaces
- Implementing multiplexing logic for shared transports (future work)

## Decisions

### Two Separate Interfaces

Create two distinct interfaces instead of a single unified interface:

```cpp
// IFireboltTextTransport.h - for text-based protocols (WebSocket)
class IFireboltTextTransport {
public:
    virtual ~IFireboltTextTransport() = default;

    virtual bool open() = 0;
    virtual bool isConnected() = 0;
    virtual void onConnectionStatusChanged(
        std::function<void(bool connected, const std::string& errorInfo)> callback) = 0;
    virtual bool send(const std::string& message) = 0;
    virtual void onMessageReceived(
        std::function<void(const std::string& message)> callback) = 0;
    virtual void disconnect() = 0;
};

// IFireboltBufferTransport.h - for buffer-based protocols (Protobuf/UDS)
class IFireboltBufferTransport {
public:
    virtual ~IFireboltBufferTransport() = 0;

    virtual bool open() = 0;
    virtual bool isConnected() = 0;
    virtual void onConnectionStatusChanged(
        std::function<void(bool connected, const std::string& errorInfo)> callback) = 0;
    virtual bool send(const std::vector<uint8_t>& data) = 0;
    virtual void onMessageReceived(
        std::function<void(const std::vector<uint8_t>& data)> callback) = 0;
    virtual void disconnect() = 0;
};
```

**Rationale:**
- WebSocket protocol requires text frames, cannot use binary frames
- Secure UDS uses binary protobuf for efficiency
- Type safety: compile-time enforcement of correct usage
- Each implementation only needs to implement what it uses
- Clear intent from interface name

**Alternatives considered:**
- Single binary interface: Rejected - WebSocket cannot send binary frames
- Single text interface: Rejected - Inefficient for binary protobuf data
- Single interface with both text and binary methods: Rejected - "Fat" interface with unused methods, type confusion
- Template-based interface: Rejected - Over-engineering for this use case

### Naming Convention

Use descriptive names that reflect the data semantics:
- `IFireboltTextTransport` - for text/string-based protocols
- `IFireboltBufferTransport` - for discrete buffer/chunk-based protocols

**Rationale:**
- "Buffer" better matches protobuf's discrete message pattern than "Stream"
- "Stream" implies continuous flow (audio/video), which doesn't fit request/response RPC
- "Buffer" matches the pattern: serialize → buffer → send → receive → buffer → deserialize

**Alternatives considered:**
- `IFireboltBinaryTransport`: Rejected - too generic
- `IFireboltProtobufTransport`: Rejected - implementation detail, not all binary uses protobuf
- `IFireboltStreamTransport`: Rejected - implies continuous flow, not discrete messages

### Configuration in Implementation

Transport implementations handle their own configuration (URL, authentication, etc.) through implementation-specific mechanisms (constructor, builder, config files, environment variables). The interface only exposes runtime operations.

**Rationale:**
- Different transports have different configuration needs
- Keeps interface clean and focused
- Implementation can use whatever mechanism fits (constructor params, config files, env vars)
- Client doesn't need to know transport-specific details

**Alternatives considered:**
- Configuration through interface methods: Rejected - adds complexity, each transport has different needs
- Configuration through open() parameters: Rejected - user specified open() should have no parameters

### Client-Owned Connection Lifecycle

Clients (CoreClient, CryptoClient, etc.) decide when to call open() and disconnect(). The transport does not auto-connect.

**Rationale:**
- Gives clients control over connection timing
- Supports lazy connection (connect on first use)
- Allows clients to implement their own connection pooling/retry logic
- Transport focuses on protocol, not policy

**Alternatives considered:**
- Auto-connect on first send: Rejected - clients should control lifecycle
- Connect in constructor: Rejected - user specified connection happens via open() method

### Header-Only Interface Pattern

Both interfaces are pure header-only with no implementation code, following the pattern established in cpp-project-layout.

**Rationale:**
- Enables runtime dependency injection
- No separate .so for interface
- Testable with mock implementations
- Partners can implement custom transports without library coupling

**Alternatives considered:**
- Abstract base class with implementation: Rejected - adds unnecessary .so, cpp-project-layout already decided against this

### License Header Convention

All new interface header files must include the standard Apache 2.0 license header at the top, matching the project convention established in cpp-project-layout. The header must use the following exact structure:

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

**Rationale:**
- Ensures consistent licensing across codebase
- Compliance with RDK Management requirements
- Matches existing project convention

### Method Documentation

All interface methods must have comprehensive documentation including purpose, parameters, return values, and behavior.

**Rationale:**
- Clear contract for implementers
- Self-documenting API
- Helps partners implement custom transports

## Architecture

```
+---------------------------------------------------+
|           TRANSPORT INTERFACE ARCHITECTURE        |
+---------------------------------------------------+

include/firebolt/
├── IFireboltTextTransport.h (new)
└── IFireboltBufferTransport.h (new)

transports/
├── example/
│   └── ExampleTransport (implements IFireboltTextTransport)
└── secure/
    └── SecureTransport (implements IFireboltBufferTransport)

src/
├── core/client/
│   └── CoreClient (uses IFireboltTextTransport)
├── crypto/client/
│   └── CryptoClient (uses IFireboltBufferTransport)
└── media/client/
    └── MediaClient (uses IFireboltBufferTransport)
```

**Flow:**
1. Transport library (.so) implements appropriate interface
2. Transport handles its own configuration (URL, auth, etc.)
3. Client accepts transport via constructor
4. Client decides when to call open()
5. Client sends/receives messages through transport

## Risks / Trade-offs

**Risk:** Two interfaces might confuse developers expecting a single transport interface
→ **Mitigation:** Clear documentation and examples showing which interface to use for which protocol

**Risk:** Future protocols might need both text and binary capabilities in a single transport
→ **Mitigation:** Design allows for a transport to implement both interfaces if needed (two separate objects or a combined implementation)

**Trade-off:** More interface surface area than single interface
→ **Acceptance:** Type safety and protocol correctness outweigh the slight increase in surface area

**Risk:** Existing code using IFireboltTransport will break
→ **Mitigation:** This is explicitly marked as a breaking change in the proposal; migration path is straightforward (choose text or buffer based on protocol)

## Migration Plan

1. Create new interface header files (`IFireboltTextTransport.h`, `IFireboltBufferTransport.h`)
2. Update ExampleTransport to implement `IFireboltTextTransport`
3. Create SecureTransport wrapper for SecureGateway implementing `IFireboltBufferTransport`
4. Update CoreClient to use `IFireboltTextTransport`
5. Update CryptoClient to use `IFireboltBufferTransport`
6. Update MediaClient to use `IFireboltBufferTransport`
7. Remove deprecated `IFireboltTransport.h`
8. Update CMake targets and build configuration
9. Update documentation and examples

**Rollback strategy:** Keep `IFireboltTransport.h` as deprecated for one release cycle, then remove.

## Open Questions

None - the interface design is clear and aligns with protocol requirements.

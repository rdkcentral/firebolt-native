# Design

## Context

The transport interface architecture (see `openspec/specs/transport-interface/spec.md`) is already defined with `IFireboltTextTransport` for text-based protocols. An ExampleTransport exists as an in-memory implementation for testing. This design adds a production WebSocket transport using WebSocket++ and Asio, following the existing transport pattern established by ExampleTransport and SecureTransport.

## Goals / Non-Goals

**Goals:**
- Implement a WebSocket transport using WebSocket++ and Asio that satisfies the `IFireboltTextTransport` interface
- Configure the transport via `FIREBOLT_ENDPOINT` environment variable
- Provide thread-safe callback registration and invocation
- Handle apps launched before network is connected (Asio retry logic)
- Include unit tests for configuration and state management
- Include integration tests with a C++ mock WebSocket server using WebSocket++
- Follow the existing transport directory structure and CMake patterns

**Non-Goals:**
- Implementing TLS/SSL support (ws:// only, not wss://)
- Implementing WebSocket sub-protocol negotiation beyond basic handshake
- Implementing automatic reconnection logic beyond Asio's network retry
- Adding WebSocket-specific features like ping/pong or compression
- Changing the existing `IFireboltTextTransport` interface

## Decisions

### WebSocket++ for WebSocket Operations

Use WebSocket++ for all WebSocket client operations (connection, send, receive, disconnect).

**Rationale:**
- Header-only library - no separate build step
- Synchronous blocking API that matches `IFireboltTextTransport::open()` interface
- Uses Asio for networking, which handles apps launched before network is connected
- Widely used and well-documented
- No GLib dependency or main loop complexity

**Alternatives considered:**
- libsoup3: Rejected - requires GLib main loop integration for async operations, doesn't have a simple blocking connect API
- ixwebsocket: Rejected - requires building a library, less widely used
- libwebsockets: Rejected - C-based API, more complex

### Environment Variable Configuration

Read the WebSocket URL from `FIREBOLT_ENDPOINT` environment variable during transport construction.

**Rationale:**
- Follows the transport-interface design principle that configuration is implementation-specific
- Simple and flexible for containerized environments
- No need to pass URL through constructor (interface has no parameters)
- Consistent with common patterns for service configuration

**Alternatives considered:**
- Constructor parameter: Rejected - interface design specifies open() has no parameters, configuration happens at construction time
- Config file: Rejected - adds complexity, environment variable is simpler
- Hardcoded URL: Rejected - not flexible for different environments

### Asio for Networking

Use Asio (standalone or Boost.Asio) for networking to handle apps launched before network is connected.

**Rationale:**
- Asio provides robust network retry logic
- Handles DNS resolution failures and network unavailability
- Works well with WebSocket++
- Can be used as standalone header-only library or with Boost

**Alternatives considered:**
- Raw sockets: Rejected - complex, no retry logic
- cURL: Rejected - overkill for WebSocket, doesn't have WebSocket support built-in

### Thread Safety with Mutex and Atomic

Use `std::mutex` for protecting callback registration and `std::atomic<bool>` for connection state.

**Rationale:**
- Follows the pattern established in SecureTransport
- Asio's io_service runs on a dedicated thread, callbacks need protection
- Atomic for simple boolean flag is sufficient for connection state
- Mutex for callback registration prevents race conditions

**Alternatives considered:**
- Lock-free data structures: Rejected - overkill for this use case
- Single-threaded design: Rejected - Asio's io_service requires a thread for async operations

### CMake Target Naming

Name the CMake target `firebolt-core-transport` to indicate it's the primary transport for the Firebolt Core client.

**Rationale:**
- Clear naming that indicates purpose
- Follows existing pattern (`firebolt-example-transport`, `firebolt-secure-transport`)
- "core" indicates it's the main production transport

**Alternatives considered:**
- `firebolt-websocket-transport`: Rejected - user specified `firebolt-core-transport`

### Mock WebSocket Server in C++

Implement the mock WebSocket server in C++ using WebSocket++ rather than Python or Node.js.

**Rationale:**
- Same library as the transport (WebSocket++) - consistent build
- No additional runtime dependencies (Python, Node.js)
- Can be built and run as part of the CMake test suite
- Consistent with existing C++ test infrastructure

**Alternatives considered:**
- Python websockets: Rejected - adds Python dependency
- Node.js ws: Rejected - adds Node.js dependency
- External echo server: Rejected - not suitable for CI

### Unit Tests for Configuration and State

Focus unit tests on configuration, state management, and error handling - areas that don't require a real WebSocket server.

**Rationale:**
- Can test without network dependencies
- Tests transport logic independent of WebSocket++ behavior
- Faster to run
- Follows existing ExampleTransport test pattern

**Alternatives considered:**
- Full network tests: Rejected - would require mock server, better as integration test

### Integration Tests with Mock Device

Use a mock WebSocket transport (similar to TextDevice pattern) for integration tests that test the full client flow.

**Rationale:**
- Follows existing integration test pattern (see `tests/integration/test_secure_lifecycle.cpp`)
- Tests client integration without network dependencies
- Can be run in CI without external server
- Separates concerns: transport logic vs client logic

**Alternatives considered:**
- End-to-end with real server: Rejected - adds complexity to CI, optional for later

## Architecture

```
+---------------------------------------------------------------+
|           WEBSOCKET TRANSPORT ARCHITECTURE                    |
+---------------------------------------------------------------+

transports/websocket/
├── include/firebolt/transport/
│   └── WebSocketTransport.h
├── src/
│   └── WebSocketTransport.cpp
├── CMakeLists.txt → firebolt-core-transport
└── tests/unit/
    └── test_websocket_transport.cpp

tests/integration/
├── mock_websocket_server.cpp (C++ WebSocket++ echo server)
└── test_websocket_lifecycle.cpp (integration test)

cmake/
└── FindWebSocket.cmake (WebSocket++ discovery)
└── FindAsio.cmake (Asio discovery)

Root CMakeLists.txt:
  add_subdirectory(transports/websocket)

Dependencies:
  - WebSocket++ (header-only)
  - Asio (header-only, standalone or Boost.Asio)
  - IFireboltTextTransport.h (existing)
  - C++17
```

### WebSocketTransport Class Structure

```cpp
class WebSocketTransport : public IFireboltTextTransport {
public:
    WebSocketTransport();  // Reads FIREBOLT_ENDPOINT
    ~WebSocketTransport();

    bool open() override;
    bool isConnected() override;
    void onConnectionStatusChanged(
        std::function<void(bool, const std::string&)> callback) override;
    bool send(const std::string& message) override;
    void onMessageReceived(
        std::function<void(const std::string& message)> callback) override;
    void disconnect() override;

private:
    std::string m_endpoint;
    std::shared_ptr<asio::io_context> m_io_service;
    std::unique_ptr<client_t> m_client;
    websocketpp::connection_hdl m_connection;
    std::unique_ptr<asio::io_context::work> m_work;
    std::thread m_io_thread;
    std::string m_lastError;

    std::atomic<bool> m_connected;
    std::mutex m_callbackMutex;

    std::function<void(bool, const std::string&)> m_connectionCallback;
    std::function<void(const std::string&)> m_messageCallback;
};
```

### Mock WebSocket Server Structure

```cpp
// Simple echo server using WebSocket++
class MockWebSocketServer {
public:
    void Start(int port);
    void Stop();

private:
    std::shared_ptr<asio::io_context> m_io_service;
    std::unique_ptr<websocketpp::server<websocketpp::config::asio>> m_server;
    websocketpp::server<websocketpp::config::asio>::connection_ptr m_connection;
    std::thread m_io_thread;
    std::unique_ptr<asio::io_context::work> m_work;
};
```

## Risks / Trade-offs

**Risk:** WebSocket++ or Asio may not be available in all build environments
→ **Mitigation:** Both are header-only libraries that can be easily vendored if needed. FindWebSocket.cmake and FindAsio.cmake will provide clear errors if not found.

**Risk:** WebSocket connection failures may be difficult to debug without logging
→ **Mitigation:** Include detailed error information in connection status callback error string. Consider adding logging in implementation (not required by spec).

**Risk:** Thread safety issues with Asio io_service running on internal thread
→ **Mitigation:** Use mutex for callback registration and atomic for connection state, following SecureTransport pattern. Ensure callbacks do not call transport APIs (documented in interface).

**Risk:** Mock WebSocket server may not fully replicate real server behavior
→ **Mitigation:** Use mock server for basic echo functionality. For comprehensive testing, optional end-to-end tests can be added later with a real server.

**Trade-off:** No TLS support (ws:// only)
→ **Acceptance:** Use case is container-to-systemhost communication on trusted network. TLS can be added later if needed.

**Trade-off:** No automatic reconnection
→ **Acceptance:** Client controls lifecycle per transport-interface design. Reconnection logic can be added at client level if needed.

## Migration Plan

No migration required - this is a new transport implementation. Existing code using ExampleTransport or other transports is unaffected.

To use the new WebSocket transport:
```cpp
#include "firebolt/transport/WebSocketTransport.h"
#include "firebolt/core/FireboltClient.h"

// Set FIREBOLT_ENDPOINT environment variable
// auto transport = std::make_shared<firebolt::transport::WebSocketTransport>();
// firebolt::core::FireboltClient client(transport);
```

## Open Questions

None - the design is clear and aligns with the existing transport architecture and WebSocket++/Asio capabilities.

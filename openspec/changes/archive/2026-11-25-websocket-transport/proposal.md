# Proposal

## Why

The Firebolt Core client currently has only an in-memory ExampleTransport for testing. A production-ready WebSocket transport is needed to enable real communication between applications running inside containers and the system host over WebSocket protocol. This is essential for the FireboltCoreClient to function in production environments.

## What Changes

- Create a new WebSocket transport implementation in `transports/websocket/` that implements `IFireboltTextTransport`
- Use WebSocket++ for WebSocket client operations
- Use Asio (standalone or Boost.Asio) for networking with proper handling of apps launched before network is connected
- Configure the transport via `FIREBOLT_ENDPOINT` environment variable for the WebSocket URL
- Create a CMake target `firebolt-core-transport` for the WebSocket transport library
- Implement CMake find modules for WebSocket++ and Asio
- Add unit tests for configuration, state management, and error handling
- Add integration tests with a C++ mock WebSocket server using WebSocket++
- Add the WebSocket transport subdirectory to the root CMakeLists.txt

## Capabilities

### New Capabilities
- `websocket-transport`: Defines the WebSocket transport implementation that provides text-based communication over WebSocket protocol using WebSocket++ and Asio, configured via environment variable, with support for connection lifecycle, message send/receive, error handling, and proper handling of apps launched before network is connected.

### Modified Capabilities
None

## Impact

- **New Library**: `libfirebolt-core-transport` (shared library) will be built and installed
- **New Transport**: FireboltCoreClient can use WebSocketTransport for real network communication
- **Build System**: Root CMakeLists.txt will include `transports/websocket` subdirectory
- **Dependencies**: Links against WebSocket++ and Asio (standalone Asio or Boost.Asio)
- **Testing**: Adds unit tests in `transports/websocket/tests/unit/` and integration tests in `tests/integration/` with mock WebSocket server

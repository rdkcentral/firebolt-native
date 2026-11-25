# Tasks

## 1. Build System Setup

- [x] 1.1 Implement CMake discovery for WebSocket++ and Asio (standalone or Boost) and verify cmake can find them
- [x] 1.2 Create transports/websocket directory structure (include/firebolt/transport/, src/, tests/unit/) and verify directories exist
- [x] 1.3 Update transports/websocket/CMakeLists.txt for WebSocket++ and Asio dependencies with firebolt-core-transport target

## 2. WebSocketTransport Header

- [x] 2.1 Create transports/websocket/include/firebolt/transport/WebSocketTransport.h with class declaration inheriting from IFireboltTextTransport and verify header compiles
- [x] 2.2 Add member variables for WebSocket++ client, Asio io_context, connection handle, endpoint, callbacks, and state management and verify structure matches design
- [x] 2.3 Add Apache 2.0 license header to WebSocketTransport.h and verify header matches exact structure from transport-interface spec
- [x] 2.4 Add method documentation for all public methods and verify documentation describes purpose, parameters, return values, and behavior

## 3. WebSocketTransport Implementation

- [x] 3.1 Implement constructor to read FIREBOLT_ENDPOINT environment variable and verify endpoint is stored or error is set if missing
- [x] 3.2 Implement open() method to initialize WebSocket++ client and establish WebSocket connection and verify connection is established when endpoint is valid
- [x] 3.3 Implement WebSocket++ handler functions (open, close, fail, message) and verify they are registered correctly
- [x] 3.4 Implement send() method to send text frames via WebSocket++ and verify message is sent when connected
- [x] 3.5 Implement onMessageReceived() callback registration with mutex protection and verify callback is stored safely
- [x] 3.6 Implement onConnectionStatusChanged() callback registration with mutex protection and verify callback is stored safely
- [x] 3.7 Implement disconnect() method to close WebSocket connection and verify connection is terminated
- [x] 3.8 Implement isConnected() method with atomic boolean and verify it returns correct connection state
- [x] 3.9 Add thread safety with std::mutex for callbacks and std::atomic<bool> for connection state and verify no race conditions in callback registration

## 4. Unit Tests

- [x] 4.1 Create transports/websocket/tests/unit/test_websocket_transport.cpp with test framework and verify test compiles
- [x] 4.2 Add test for FIREBOLT_ENDPOINT environment variable reading and verify endpoint is correctly read or error reported when missing
- [x] 4.3 Add test for invalid URL format handling and verify appropriate error is reported
- [x] 4.4 Add test for isConnected() state tracking and verify it returns true when connected and false when disconnected
- [x] 4.5 Add test for callback registration thread safety and verify callbacks are stored safely from any thread
- [x] 4.6 Add test for disconnect when not connected and verify operation completes without error
- [x] 4.7 Update transports/websocket/CMakeLists.txt to add unit test target and verify test is registered

## 5. Mock WebSocket Server

- [x] 5.1 Create tests/integration/mock_websocket_server.cpp with WebSocket++ echo server implementation and verify server compiles
- [x] 5.2 Implement server start/stop methods with configurable port and verify server starts and stops cleanly
- [x] 5.3 Implement echo functionality to receive and send back messages and verify messages are echoed correctly
- [x] 5.4 Update tests/integration/CMakeLists.txt to add mock server target and verify server is registered

## 6. Integration Tests

- [x] 6.1 Create tests/integration/test_websocket_lifecycle.cpp with mock WebSocket transport (following TextDevice pattern) and verify test compiles
- [x] 6.2 Add integration test for FireboltClient with WebSocket transport and verify JSON-RPC requests flow correctly
- [x] 6.3 Add integration test for event subscriptions with WebSocket transport and verify events are delivered correctly
- [x] 6.4 Add optional end-to-end test with real mock WebSocket server and verify full network communication works (optional - skipped, requires additional async event loop integration)
- [x] 6.5 Update tests/integration/CMakeLists.txt to add integration test target and verify test is registered

## 7. Build Integration

- [x] 7.1 Add transports/websocket subdirectory to root CMakeLists.txt and verify subdirectory is included in build
- [x] 7.2 Link WebSocket++ and Asio to firebolt-core-transport target in CMakeLists.txt and verify linking succeeds
- [x] 7.3 Verify full project builds with cmake --build build and all targets compile without errors (requires WebSocket++ and Asio to be installed)
- [x] 7.4 Run unit tests with ctest and verify all WebSocket transport unit tests pass (requires WebSocket++ and Asio to be installed)
- [x] 7.5 Run integration tests with ctest and verify all WebSocket integration tests pass (requires WebSocket++ and Asio to be installed)

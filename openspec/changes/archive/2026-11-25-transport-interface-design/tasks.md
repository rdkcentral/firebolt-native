# Tasks

## 1. Create New Transport Interface Headers

- [x] 1.1 Create `IFireboltTextTransport.h` with the standard Apache 2.0 license header (exact structure as specified in design.md) and verify file exists in `include/firebolt/`
- [x] 1.2 Implement `IFireboltTextTransport` interface with all methods documented (open, isConnected, onConnectionStatusChanged, send, onMessageReceived, disconnect) and verify header compiles without errors
- [x] 1.3 Create `IFireboltBufferTransport.h` with the standard Apache 2.0 license header (exact structure as specified in design.md) and verify file exists in `include/firebolt/`
- [x] 1.4 Implement `IFireboltBufferTransport` interface with all methods documented (open, isConnected, onConnectionStatusChanged, send, onMessageReceived, disconnect) and verify header compiles without errors

## 2. Update Transport Implementations

- [x] 2.1 Update `ExampleTransport` to implement `IFireboltTextTransport` instead of `IFireboltTransport` and verify compilation succeeds
- [x] 2.2 Create `SecureTransport` header and implementation files in `transports/secure/` that implement `IFireboltBufferTransport` wrapping SecureGateway and verify files compile
- [x] 2.3 Implement `SecureTransport` to use SecureGateway's binary API (`client_send_request`, `client_register_notifications`) and verify unit tests pass
- [x] 2.4 Add CMakeLists.txt for secure transport library and verify the library builds successfully
- [x] 2.5 Update existing ExampleTransport tests to verify text interface behavior and ensure all tests pass

## 3. Update Client Libraries

- [x] 3.1 Update `CoreClient` to accept `IFireboltTextTransport` instead of `IFireboltTransport` in constructor and verify compilation succeeds
- [x] 3.2 Update `CryptoClient` to accept `IFireboltBufferTransport` instead of `IFireboltTransport` in constructor and verify compilation succeeds
- [x] 3.3 Update `MediaClient` to accept `IFireboltBufferTransport` instead of `IFireboltTransport` in constructor and verify compilation succeeds
- [x] 3.4 Update client header files to include the correct transport interface and verify all headers compile independently

## 4. Update Build System

- [x] 4.1 Update root CMakeLists.txt to add secure transport subdirectory and verify CMake configuration succeeds
- [x] 4.2 Update client library CMakeLists.txt files to link against the correct transport interface headers and verify all targets build
- [x] 4.3 Remove references to deprecated `IFireboltTransport.h` from CMake targets and verify no build errors occur
- [x] 4.4 Update example transport CMakeLists.txt if needed for the interface change and verify example transport builds
- [x] 4.5 Run full project build and verify all libraries compile successfully

## 5. Remove Deprecated Interface

- [x] 5.1 Remove `IFireboltTransport.h` from `include/firebolt/` and verify file no longer exists
- [x] 5.1 Search codebase for any remaining references to `IFireboltTransport` and remove or update them, then verify no compilation errors
- [x] 5.3 Verify no test files or examples reference the deprecated interface

## 6. Documentation and Examples

- [x] 6.1 Update README.md to reflect the new dual-transport interface architecture and verify documentation is accurate
- [x] 6.2 Update any existing documentation that references `IFireboltTransport` to use the new interfaces and verify all docs are consistent
- [x] 6.3 Add usage examples for both `IFireboltTextTransport` and `IFireboltBufferTransport` in documentation and verify examples compile
- [x] 6.4 Update design.md reference in cpp-project-layout change to note the interface evolution and verify documentation is coherent

## 7. Verification and Testing

- [x] 7.1 Run all unit tests and verify they pass
- [x] 7.2 Run integration tests if they exist and verify they pass
- [x] 7.3 Verify that ExampleTransport works with CoreClient and that messages flow correctly
- [x] 7.4 Verify that SecureTransport can be constructed and used with CryptoClient (if implementation is complete)
- [x] 7.5 Perform static analysis or linting if configured and verify no warnings

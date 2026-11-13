# Tasks

## 1. Common Framework Setup

- [x] 1.1 Create include/firebolt/common/FireboltModel.h with CRTP base class template and verify the template compiles with a test model
- [x] 1.2 Create include/firebolt/common/FireboltObject.h with JSON utility class using nlohmann::json and verify set<T>() and get<T>() methods work with string, int, and bool types
- [x] 1.3 Create include/firebolt/common/FireboltResult.h with success/failure state and verify the result correctly indicates success/failure and provides value access on success
- [x] 1.4 Create include/firebolt/common/FireboltOperation.h header-only wrapper template and verify it compiles with IOperationState forward declaration
- [x] 1.5 Create include/firebolt/common/FireboltSubscription.h header-only wrapper class and verify it compiles with ISubscriptionState forward declaration

## 2. Protobuf Definitions and Build Integration

- [x] 2.1 Create src/proto/rpc.proto with RpcService enum (add RPC_SERVICE_JSON_RPC), RpcRequest, RpcResponse, and RpcHeartbeat messages and verify protoc compiles the file without errors
- [x] 2.2 Create src/proto/core.proto with CoreRequest (id, method, params) and CoreResponse (id, method, result, error) messages and verify protoc compiles the file without errors
- [x] 2.3 Update CMakeLists.txt to find_package(Protobuf REQUIRED) and verify the build system finds protobuf successfully
- [x] 2.4 Add protobuf_generate_cpp macro for rpc.proto and core.proto in CMakeLists.txt and verify .pb.h and .pb.cc files are generated in include/firebolt/proto/ and src/proto/generated/
- [x] 2.5 Create src/proto/CMakeLists.txt to build libfirebolt-proto.so from generated protobuf sources and verify the library builds and links correctly

## 3. Common Runtime Library

- [x] 3.1 Create include/firebolt/common/IOperationState.h interface with request lifecycle management methods and verify the interface compiles
- [x] 3.2 Create include/firebolt/common/ISubscriptionState.h interface with event registration and cleanup methods and verify the interface compiles
- [x] 3.3 Implement IOperationState in src/common/src/OperationState.cpp with request correlation, callback registration, completion tracking, and blocking Get() method for synchronous consumption
- [x] 3.4 Implement ISubscriptionState in src/common/src/SubscriptionState.cpp with listener registration, unsubscription, and automatic cleanup and verify unit tests cover registration and unsubscription
- [x] 3.5 Create src/common/CMakeLists.txt to build libfirebolt-common.so with Utils.cpp, Logging.cpp, OperationState.cpp, and SubscriptionState.cpp and verify the library builds and exports the required symbols

## 4. Core Client Implementation

- [x] 4.1 Create include/firebolt/core/FireboltClient.h with constructor accepting IFireboltTextTransport and Send<T>(), Subscribe<T>(), and GetInterface<T>() methods and verify the header compiles
- [x] 4.2 Implement FireboltClient in src/core/client/src/FireboltClient.cpp with JSON-RPC request construction, response deserialization, and IOperationState/ISubscriptionState integration and verify unit tests mock the transport and test request/response flow
- [x] 4.3 Update src/core/client/CMakeLists.txt to build libfirebolt-core-client.so linking to libfirebolt-common.so and libfirebolt-proto.so and verify the library builds and links correctly
- [x] 4.4 Add JSON-RPC serialization helpers in src/core/client/src/JsonRpcBuilder.cpp and verify the builder correctly constructs JSON-RPC request strings from method and params

## 5. Secure Client Implementation

- [x] 5.1 Create include/firebolt/secure/core/FireboltSecureClient.h with constructor accepting IFireboltBufferTransport and Send<T>(), Subscribe<T>(), and GetInterface<T>() methods and verify the header compiles
- [x] 5.2 Implement FireboltSecureClient in src/secure/core/client/src/FireboltSecureClient.cpp with Protobuf envelope wrapping, JSON-RPC-to-Protobuf translation, and envelope unwrapping and verify unit tests mock the transport and test the wrapping/unwrapping flow
- [x] 5.3 Create src/secure/core/client/CMakeLists.txt to build libfirebolt-secure-client.so linking to libfirebolt-common.so and libfirebolt-proto.so and verify the library builds and links correctly
- [x] 5.4 Implement envelope wrapping logic in src/secure/core/client/src/EnvelopeWrapper.cpp with RpcRequest/RpcResponse construction and CoreRequest/CoreResponse extraction and verify unit tests cover the secure send and receive flows

## 6. Generated Lifecycle Interface - Core Path

- [x] 6.1 Create include/firebolt/core/Lifecycle.h with LifecycleState model (FireboltString state), ILifecycle interface, and Lifecycle implementation using FireboltClient and verify the header compiles and matches the example from the proposal
- [x] 6.2 Implement LifecycleState write(FireboltObject&) and read(FireboltObject&) methods and verify the model correctly serializes and deserializes to/from JSON
- [x] 6.3 Implement Lifecycle.GetLifecycleState() method calling client.Send<LifecycleState>("lifecycle.state", FireboltObject{}) and verify the method signature matches the spec
- [x] 6.4 Implement Lifecycle.OnStateChanged() method calling client.Subscribe<LifecycleState>("lifecycle.onStateChanged", listener) and verify the method signature matches the spec
- [x] 6.5 Implement LifecycleInterface(FireboltClient&) helper function calling client.GetInterface<Lifecycle>() and verify the function returns the correct interface

## 7. Generated Lifecycle Interface - Secure Path

- [x] 7.1 Create include/firebolt/secure/core/Lifecycle.h with LifecycleState model (identical to core), ILifecycle interface, and Lifecycle implementation using FireboltSecureClient and verify the header compiles and has identical API surface to core path
- [x] 7.2 Verify LifecycleState write(FireboltObject&) and read(FireboltObject&) methods are identical to core path (reuse or copy)
- [x] 7.3 Implement Lifecycle.GetLifecycleState() method calling client.Send<LifecycleState>("lifecycle.state", FireboltObject{}) and verify the method signature is identical to core path
- [x] 7.4 Implement Lifecycle.OnStateChanged() method calling client.Subscribe<LifecycleState>("lifecycle.onStateChanged", listener) and verify the method signature is identical to core path
- [x] 7.5 Implement LifecycleInterface(FireboltSecureClient&) helper function calling client.GetInterface<Lifecycle>() and verify the function signature matches the secure client type

## 8. Integration Testing

- [x] 8.1 Create integration test for core path using ExampleTransport (text transport mock) and verify Lifecycle.GetLifecycleState() sends correct JSON-RPC and deserializes response
- [x] 8.2 Create integration test for secure path using ExampleTransport (buffer transport mock) and verify Lifecycle.GetLifecycleState() wraps JSON-RPC in Protobuf envelope correctly
- [x] 8.3 Create integration test for core path subscription and verify Lifecycle.OnStateChanged() registers listener and receives events
- [x] 8.4 Create integration test for secure path subscription and verify Lifecycle.OnStateChanged() registers listener and receives events through envelope unwrapping
- [x] 8.5 Verify both core and secure paths produce identical application-visible behavior with different underlying transports

## 9. Documentation and Examples

- [x] 9.1 Create README.md in include/firebolt/core/ documenting the core path usage with FireboltClient and Lifecycle example and verify the documented code compiles and runs
- [x] 9.2 Create README.md in include/firebolt/secure/core/ documenting the secure path usage with FireboltSecureClient and Lifecycle example and verify the documented code compiles and runs
- [x] 9.3 Add build instructions to main README.md explaining how to build libfirebolt-common.so, libfirebolt-core-client.so, and libfirebolt-secure-client.so and verify the instructions are accurate
- [x] 9.4 Document the protobuf envelope structure in a design doc or code comments and verify the documentation matches the actual implementation

## 10. Build System Verification

- [x] 10.1 Verify the complete build (including protobuf generation, all libraries, and generated interfaces) succeeds without errors
- [x] 10.2 Verify all libraries export the correct symbols (nm or similar tool) and verify no missing symbols
- [x] 10.3 Verify include paths are correctly set so that #include <firebolt/core/Lifecycle.h> and #include <firebolt/secure/core/Lifecycle.h> resolve correctly
- [ ] 10.4 Verify the build works with both local protobuf installation (vcpkg or system) and documents the minimum protobuf version required (partial: built and tested with Homebrew 7.36.2, vcpkg not available, minimum version not established)

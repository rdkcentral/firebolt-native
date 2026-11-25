# Spec Delta

## Purpose

Provides a header-only framework for generating Firebolt API interfaces that support both JSON-RPC over WebSocket and JSON-RPC wrapped in Protobuf over Unix Domain Socket, with identical user-facing APIs across both transport paths.

## ADDED Requirements

### Requirement: Common model framework uses CRTP for header-only type safety
The system SHALL provide a CRTP-based FireboltModel base class that generated models inherit from, enabling compile-time type safety and shared serialization behavior without runtime virtual overhead.

#### Scenario: Generated model inherits from CRTP base
- **WHEN** a generated API model is created
- **THEN** it inherits from FireboltModel<T> with its concrete type as template parameter
- **AND** the model implements write(FireboltObject&) and read(FireboltObject&) methods

### Requirement: FireboltObject provides JSON serialization abstraction
The system SHALL provide a FireboltObject utility class that abstracts JSON representation, allowing models to serialize and deserialize without coupling to specific JSON libraries.

#### Scenario: FireboltObject serializes model fields to JSON
- **WHEN** a model writes its fields to a FireboltObject
- **THEN** the object stores the data in an internal JSON representation
- **AND** the object can be converted to a JSON string via toString()

#### Scenario: FireboltObject deserializes JSON to model fields
- **WHEN** a model reads fields from a FireboltObject
- **THEN** the object parses the internal JSON representation
- **AND** typed values are extracted via get<T>() methods

### Requirement: Operation wrapper provides async and sync consumption
The system SHALL provide a FireboltOperation<T> template class that represents a pending request, supporting both callback-based async consumption and blocking sync consumption with timeout. The template parameter T is the generated model type (e.g., LifecycleState), not a string.

#### Scenario: Operation consumed asynchronously with callback
- **WHEN** application calls Then() on a FireboltOperation<T>
- **THEN** the callback receives a FireboltResult<T> when the operation completes
- **AND** the result contains the deserialized model of type T
- **AND** the callback runs on an appropriate thread

#### Scenario: Operation consumed synchronously with timeout
- **WHEN** application calls Get(timeout) on a FireboltOperation<T>
- **THEN** the call blocks until the operation completes or timeout expires
- **AND** the method returns a FireboltResult<T> containing the deserialized model of type T

### Requirement: Subscription wrapper manages event lifetime
The system SHALL provide a FireboltSubscription class that represents an active event registration, supporting explicit unsubscription and automatic cleanup on destruction.

#### Scenario: Subscription created with listener callback
- **WHEN** application subscribes to an event
- **THEN** a FireboltSubscription is returned
- **AND** the listener callback is registered with the runtime
- **AND** the subscription remains active until unsubscribed

#### Scenario: Subscription explicitly unsubscribed
- **WHEN** application calls Unsubscribe() on a FireboltSubscription
- **THEN** the listener callback is unregistered
- **AND** no further events are delivered

### Requirement: Result wrapper provides error-safe value access
The system SHALL provide a FireboltResult<T> template class that wraps operation results, distinguishing success from failure and providing safe value access.

#### Scenario: Result checked for success
- **WHEN** application checks if a FireboltResult<T> is successful
- **THEN** the result returns true if the operation succeeded
- **AND** returns false if the operation failed

#### Scenario: Result value accessed on success
- **WHEN** application calls Value() on a successful FireboltResult<T>
- **THEN** the result returns the typed value

### Requirement: Core client handles JSON-RPC over text transport
The system SHALL provide a FireboltClient class that accepts an IFireboltTextTransport, sends JSON-RPC requests directly as text, and deserializes JSON-RPC responses to generated models.

#### Scenario: Core client sends JSON-RPC request
- **WHEN** application calls Send<T>(method, params) on FireboltClient
- **THEN** the client constructs a JSON-RPC request with method and params
- **AND** the request is sent via the text transport as a JSON string

#### Scenario: Core client receives JSON-RPC response
- **WHEN** the text transport delivers a JSON-RPC response
- **THEN** the client deserializes the JSON to the requested model type T
- **AND** the FireboltOperation<T> completes with the result

### Requirement: Secure client wraps JSON-RPC in Protobuf envelope
The system SHALL provide a FireboltSecureClient class that accepts an IFireboltBufferTransport, wraps JSON-RPC requests in a Protobuf envelope, and unwraps Protobuf responses to JSON-RPC before deserialization.

#### Scenario: Secure client wraps JSON-RPC in Protobuf
- **WHEN** application calls Send<T>(method, params) on FireboltSecureClient
- **THEN** the client constructs a JSON-RPC request as a JSON string
- **AND** the JSON string is wrapped in a CoreRequest Protobuf message
- **AND** the CoreRequest is wrapped in an RpcRequest Protobuf envelope with service set to RPC_SERVICE_JSON_RPC
- **AND** the serialized envelope is sent via the buffer transport

#### Scenario: Secure client unwraps Protobuf to JSON-RPC
- **WHEN** the buffer transport delivers a Protobuf envelope
- **THEN** the client deserializes the RpcResponse envelope
- **AND** extracts the CoreResponse payload
- **AND** deserializes the CoreResponse JSON to the requested model type T
- **AND** the FireboltOperation<T> completes with the result

### Requirement: Generated interfaces use identical API surface across paths
The system SHALL generate interface headers that expose the same methods for both core and secure paths, differing only in which client type they accept and which header directory they reside in.

#### Scenario: Core interface uses FireboltClient
- **WHEN** application includes <firebolt/core/Lifecycle.h>
- **THEN** the Lifecycle constructor accepts FireboltClient&
- **AND** all methods are identical to the secure path

#### Scenario: Secure interface uses FireboltSecureClient
- **WHEN** application includes <firebolt/secure/core/Lifecycle.h>
- **THEN** the Lifecycle constructor accepts FireboltSecureClient&
- **AND** all methods are identical to the core path

### Requirement: Protobuf envelope defines shared RPC service types
The system SHALL provide rpc.proto with RpcService enum including RPC_SERVICE_JSON_RPC, RpcRequest, RpcResponse, and RpcHeartbeat message types for shared envelope structure.

#### Scenario: RPC service enum includes JSON-RPC service
- **WHEN** rpc.proto is compiled
- **THEN** RpcService enum contains RPC_SERVICE_JSON_RPC value
- **AND** the value can be used in RpcRequest.service field

#### Scenario: RPC request envelope wraps service-specific payload
- **WHEN** a service-specific request is created
- **THEN** RpcRequest contains request_id, service, schema_version, and payload fields
- **AND** payload is a bytes field containing the serialized service-specific message

### Requirement: Core protobuf defines JSON-RPC request and response
The system SHALL provide core.proto with CoreRequest and CoreResponse message types that represent JSON-RPC method calls and results.

#### Scenario: Core request contains JSON-RPC fields
- **WHEN** a CoreRequest message is created
- **THEN** it contains id, method, and params fields
- **AND** method and params are string fields containing JSON-RPC method name and parameters

#### Scenario: Core response contains JSON-RPC result or error
- **WHEN** a CoreResponse message is created
- **THEN** it contains id, method, result, error, and params fields
- **AND** result and error are string fields containing JSON-RPC response data
- **AND** params is a string field containing JSON-RPC notification/event parameters

### Requirement: Runtime libraries provide shared state management
The system SHALL provide libfirebolt-common.so with shared runtime infrastructure for operation state, subscription state, and callback dispatching, usable by both core and secure clients.

#### Scenario: Common library provides operation state interface
- **WHEN** a client creates an operation
- **THEN** the operation uses IOperationState from libfirebolt-common.so
- **AND** the state manages request lifecycle and correlation

#### Scenario: Common library provides subscription state interface
- **WHEN** a client creates a subscription
- **THEN** the subscription uses ISubscriptionState from libfirebolt-common.so
- **AND** the state manages event registration and cleanup

### Requirement: Client libraries provide path-specific implementations
The system SHALL provide libfirebolt-core-client.so and libfirebolt-secure-client.so with implementations specific to their transport paths, sharing common runtime infrastructure.

#### Scenario: Core client library links to common runtime
- **WHEN** libfirebolt-core-client.so is built
- **THEN** it links to libfirebolt-common.so
- **AND** it implements FireboltClient with text-transport-specific logic

#### Scenario: Secure client library links to common runtime
- **WHEN** libfirebolt-secure-client.so is built
- **THEN** it links to libfirebolt-common.so
- **AND** it implements FireboltSecureClient with Protobuf envelope wrapping logic

### Requirement: Build system integrates protobuf code generation
The system SHALL configure CMake to find libprotobuf, use protobuf_generate_cpp macro for .proto files, and generate .pb.h and .pb.cc files in appropriate directories.

#### Scenario: Protobuf library found via CMake
- **WHEN** the project is configured with CMake
- **THEN** find_package(Protobuf) locates libprotobuf
- **AND** the protobuf compiler is available

#### Scenario: Protobuf files generate C++ headers and sources
- **WHEN** rpc.proto and core.proto are processed
- **THEN** protobuf_generate_cpp generates .pb.h and .pb.cc files
- **AND** headers are placed in include/firebolt/proto/
- **AND** sources are placed in src/proto/generated/

### Requirement: Error handling provides basic error reporting
The system SHALL provide basic error handling through FireboltResult<T> error state, with extensibility for future error classification and recovery strategies.

#### Scenario: Result contains error information on failure
- **WHEN** an operation fails
- **THEN** the FireboltResult<T> indicates failure
- **AND** the result contains error information accessible via error methods

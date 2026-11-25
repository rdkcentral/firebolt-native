# Design

## Context

The codebase currently has IFireboltTextTransport and IFireboltBufferTransport interfaces (see transport-interface spec), but lacks a complete client framework for generating and consuming Firebolt APIs. Partners need a user-friendly system that supports both JSON-RPC over WebSocket and JSON-RPC wrapped in Protobuf over Unix Domain Socket. The design must keep envelope abstraction transparent to application code while providing identical APIs across both paths.

## Goals / Non-Goals

**Goals:**
- Create header-only framework for API generation with CRTP-based type safety
- Implement dual-path clients (core and secure) with identical user-facing APIs
- Establish protobuf envelope structure for JSON-RPC-in-Protobuf wrapping
- Provide shared runtime infrastructure for operation and subscription state management
- Generate example Lifecycle interface demonstrating both paths with one method and one subscription

**Non-Goals:**
- Implementing actual WebSocket or UDS transport implementations (future work)
- Complete error classification and recovery strategies (future proposal)
- Code generator implementation (this proposal uses manual example generation)
- Multiplexing logic for shared transports (future work)

## Decisions

### Header-Only Framework with CRTP

Use CRTP (Curiously Recurring Template Pattern) for FireboltModel<T> to enable compile-time type safety and shared serialization behavior without runtime virtual overhead.

**Rationale:**
- Generated models know their concrete types at compile time
- Eliminates virtual method call overhead
- Reuses serialization logic across all models
- Matches the Firebolt CPP 2.0 architecture from the reference implementation

**Alternatives considered:**
- Virtual interface: Rejected - adds runtime overhead, unnecessary for generated types
- Runtime polymorphism: Rejected - models are generated, not dynamically loaded
- Separate serialization per model: Rejected - code duplication, maintenance burden

### FireboltObject as JSON Abstraction

Implement FireboltObject as a utility class that internally uses nlohmann::json but abstracts the JSON library from generated models.

**Rationale:**
- Allows changing JSON library implementation without affecting generated code
- Partners can use the same pattern for custom headers
- Provides type-safe get<T>() and set<T>() methods
- Matches the reference Firebolt CPP 2.0 design

**Alternatives considered:**
- Direct nlohmann::json usage: Rejected - couples generated code to specific library
- Custom JSON parser: Rejected - unnecessary complexity, nlohmann is standard
- Protocol-agnostic object: Rejected - adds complexity for this scope, JSON is primary

### Separate Client Libraries

Create three separate libraries: libfirebolt-common.so (shared runtime), libfirebolt-core-client.so (text transport), and libfirebolt-secure-client.so (buffer transport with envelope wrapping).

**Rationale:**
- Clear separation of concerns: common runtime vs path-specific logic
- Apps can link only what they need (core-only, secure-only, or both)
- Prevents code bloat in core-only deployments
- Allows independent versioning of each path

**Alternatives considered:**
- Single monolithic library: Rejected - forces core apps to pay for secure path overhead
- Template-based client: Rejected - increases binary size, harder to version
- Client in headers only: Rejected - runtime state (correlation, dispatching) needs implementation

### Protobuf Envelope Wrapping

Wrap JSON-RPC strings in Protobuf messages: CoreRequest/Response for JSON-RPC structure, RpcRequest/Response for shared envelope with service routing.

**Rationale:**
- Keeps generated models JSON-based (simpler for partners)
- Envelope wrapping is transparent to application code
- Enables service routing via RpcService enum
- Matches existing rpc.proto structure from crypto client

**Alternatives considered:**
- Native Protobuf models: Rejected - would require regenerating all models, breaks compatibility
- Custom binary format: Rejected - protobuf is standard, already used by crypto client
- JSON-only for both paths: Rejected - UDS requires binary for efficiency, security requirements

### Internal State Interfaces

Define IOperationState and ISubscriptionState as opaque interfaces in libfirebolt-common.so, with FireboltOperation<T> and FireboltSubscription as header-only wrappers.

IOperationState and ISubscriptionState deal with JSON strings internally (transport-level data), while FireboltOperation<T> and FireboltSubscription provide typed facades over this state where T is the generated model type.

**Rationale:**
- Runtime library remains independent of generated model types (deals with JSON strings only)
- Generated headers stay header-only (template parameter T is the model type)
- Enables runtime to manage correlation, dispatching, and synchronization
- FireboltOperation<T> deserializes JSON to model T before invoking callbacks
- Matches the reference Firebolt CPP 2.0 architecture

**Alternatives considered:**
- State in headers: Rejected - would require template instantiation in runtime, bloats binary
- Concrete state types: Rejected - couples runtime to specific implementations
- Callback-only approach: Rejected - no support for blocking Get() or cancellation

### Identical API Surface Across Paths

Generate interface headers with identical method signatures for both core and secure paths, differing only in client type and include directory.

**Rationale:**
- Application code is transport-agnostic
- Easy migration between core and secure paths
- Partners write code once, choose transport at build time
- Simplifies testing and documentation

**Alternatives considered:**
- Different APIs per path: Rejected - forces app code to be path-specific
- Runtime path selection: Rejected - adds complexity, compile-time selection is sufficient
- Single header with conditional compilation: Rejected - harder to understand, maintenance burden

### Protobuf Build Integration

Use CMake's find_package(Protobuf) and protobuf_generate_cpp macro for code generation, with headers in include/firebolt/proto/ and sources in src/proto/generated/.

**Rationale:**
- Standard CMake pattern for protobuf
- Works with both local builds (vcpkg/system) and yocto builds (provided dependency)
- Clear separation of generated headers and sources
- Matches existing crypto client proto structure

**Alternatives considered:**
- Custom protoc invocation: Rejected - non-standard, harder to maintain
- Bundle protobuf source: Rejected - yocto provides dependency, bundling is redundant
- Generate in source tree: Rejected - pollutes source, generated files should be separate

### Basic Error Handling

Implement FireboltResult<T> with success/failure state and basic error information, deferring detailed error classification to future proposals.

**Rationale:**
- Provides immediate value without over-engineering
- Establishes pattern for future error handling enhancements
- Sufficient for initial example (Lifecycle)
- Allows learning from real usage before committing to complex error taxonomy

**Alternatives considered:**
- Comprehensive error taxonomy: Rejected - premature optimization, hard to design without usage
- Exception-based: Rejected - C++17 lacks std::expected, exceptions add complexity
- Error codes only: Rejected - loses context, harder to debug

## Architecture

```
HEADER STRUCTURE
================

include/firebolt/
├── common/
│   ├── FireboltModel.h (CRTP base)
│   ├── FireboltObject.h (JSON utility)
│   ├── FireboltOperation.h (header-only wrapper)
│   ├── FireboltSubscription.h (header-only wrapper)
│   ├── FireboltResult.h (result wrapper)
│   ├── Utils.h (existing)
│   └── Logging.h (existing)
├── core/
│   ├── FireboltClient.h (text transport client)
│   └── Lifecycle.h (generated example)
└── secure/
    └── core/
        ├── FireboltSecureClient.h (buffer transport client)
        └── Lifecycle.h (generated example)


LIBRARY STRUCTURE
==================

libfirebolt-common.so
  - IOperationState implementation
  - ISubscriptionState implementation
  - Request correlation
  - Callback dispatching
  - Thread synchronization
  - Utils.cpp
  - Logging.cpp

libfirebolt-core-client.so
  - FireboltClient implementation
  - JSON-RPC request construction
  - JSON-RPC response deserialization
  - Links to libfirebolt-common.so

libfirebolt-secure-client.so
  - FireboltSecureClient implementation
  - Protobuf envelope wrapping/unwrapping
  - JSON-RPC-to-Protobuf translation
  - Links to libfirebolt-common.so


PROTOBUF DEFINITIONS
====================

src/proto/
├── rpc.proto
│   - enum RpcService (add RPC_SERVICE_JSON_RPC)
│   - message RpcRequest
│   - message RpcResponse
│   - message RpcHeartbeat
│
├── core.proto
│   - message CoreRequest (id, method, params)
│   - message CoreResponse (id, method, result, error, params)
│
└── crypto/
    └── crypto.proto (existing)


DATA FLOW - CORE PATH
=====================

Application
    ↓
Generated Interface (Lifecycle.h)
    ↓
FireboltClient
    ↓
IOperationState (libfirebolt-common.so)
    ↓
JSON-RPC string
    ↓
IFireboltTextTransport (WebSocket)


DATA FLOW - SECURE PATH
========================

Application
    ↓
Generated Interface (Lifecycle.h)
    ↓
FireboltSecureClient
    ↓
IOperationState (libfirebolt-common.so)
    ↓
JSON-RPC string
    ↓
CoreRequest protobuf
    ↓
RpcRequest protobuf (service = RPC_SERVICE_JSON_RPC)
    ↓
IFireboltBufferTransport (UDS)


ENVELOPE WRAPPING DETAIL
========================

Secure Send Flow:
1. FireboltSecureClient.Send<T>(method, params)
2. Build JSON-RPC request string: {"id":1,"method":"lifecycle.state","params":{}}
3. Create CoreRequest: id=1, method="lifecycle.state", params="{}"
4. Create RpcRequest: request_id=1, service=RPC_SERVICE_JSON_RPC, schema_version=1, payload=CoreRequest.SerializeAsString()
5. Send via buffer transport

Secure Receive Flow (reverse):
1. Buffer transport delivers RpcResponse
2. Deserialize RpcResponse
3. Extract payload as CoreResponse
4. For method calls: Deserialize CoreResponse.result JSON string
5. For notifications/events: Deserialize CoreResponse.params JSON string
6. Deserialize JSON to model T
7. Complete FireboltOperation<T> or deliver event to listener
```

## Risks / Trade-offs

**Risk:** Protobuf version compatibility across different build environments
→ **Mitigation:** Use find_package(Protobuf) with minimum version requirement compatible with C++17, document version requirements

**Risk:** JSON library (nlohmann::json) may not be available in all partner environments
→ **Mitigation:** FireboltObject abstraction allows switching JSON libraries in future if needed, document nlohmann dependency

**Risk:** CRTP template instantiation bloats binary size
→ **Mitigation:** Common base class shared across models, template instantiation limited to model-specific code, measure and optimize if needed

**Risk:** Header-only framework may increase compile times
→ **Mitigation:** Framework is minimal, generated code is what it is, consider precompiled headers for partners if needed

**Trade-off:** Three libraries vs single library
→ **Acceptance:** Clear separation of concerns and link-time flexibility outweigh the slight complexity in build configuration

**Risk:** Error handling may be insufficient for production use
→ **Mitigation:** This is explicitly a basic blueprint, future proposals will expand error classification based on real usage

**Risk:** Envelope wrapping adds latency to secure path
→ **Mitigation:** Protobuf is efficient, binary transport is faster than text, net effect should be acceptable; measure and optimize if needed

## Migration Plan

This is a greenfield implementation with no existing API header client code, so no migration is required. The change adds new capabilities without modifying existing behavior.

**Deployment steps:**
1. Implement common framework (FireboltModel, FireboltObject, wrappers)
2. Implement protobuf definitions and build integration
3. Implement libfirebolt-common.so runtime
4. Implement FireboltClient and libfirebolt-core-client.so
5. Implement FireboltSecureClient and libfirebolt-secure-client.so
6. Generate example Lifecycle interface for both paths
7. Add tests for core and secure paths
8. Update documentation with usage examples

**Rollback strategy:**
- New libraries are optional additions, existing code continues to work
- If issues arise, partners can continue using direct transport access
- Remove new libraries and headers from build configuration

## Open Questions

None - the design is complete and addresses all requirements from the spec. Future proposals will expand error handling, add more generated interfaces, and optimize based on real usage.

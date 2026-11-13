# Proposal

## Why

The current codebase has transport interfaces but lacks a complete client framework for generating and consuming Firebolt APIs. Partners need a user-friendly, header-only API generation system that supports both JSON-RPC over WebSocket and JSON-RPC wrapped in Protobuf over Unix Domain Socket, while keeping the envelope abstraction transparent to application code.

## What Changes

- Create common framework for header-only API generation (FireboltModel CRTP base, FireboltObject JSON utility, FireboltOperation/Subscription/Result wrappers)
- Implement FireboltClient for text transport (direct JSON-RPC over WebSocket)
- Implement FireboltSecureClient for buffer transport (JSON-RPC wrapped in Protobuf envelope over UDS)
- Add protobuf definitions for shared RPC envelope (rpc.proto) and core API envelope (core.proto)
- Generate example Lifecycle interface with one method (GetLifecycleState) and one subscription (OnStateChanged) for both core and secure paths
- Create CMake build configuration for new libraries: libfirebolt-common.so, libfirebolt-core-client.so, libfirebolt-secure-client.so
- Establish basic error handling blueprint (expandable in future proposals)

## Capabilities

### New Capabilities
- `api-header-client`: Header-only API generation framework with dual-path support (JSON-RPC and JSON-RPC-in-Protobuf), including common model utilities, client implementations, and generated interface pattern

### Modified Capabilities
- `transport-interface`: No requirement changes - the existing transport interfaces remain unchanged; this change builds upon them

## Impact

Affected systems:
- New public headers in include/firebolt/common/, include/firebolt/core/, include/firebolt/secure/core/
- New runtime libraries: libfirebolt-common.so, libfirebolt-core-client.so, libfirebolt-secure-client.so
- New protobuf definitions in src/proto/ (rpc.proto, core.proto)
- Build system updates for protobuf integration and new library targets
- Application code will use generated interfaces (e.g., lifecycle.h) with identical API surface for both paths

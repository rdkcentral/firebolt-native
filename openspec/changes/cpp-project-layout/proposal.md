# Proposal

## Why

The firebolt-native repository needs a structured C++ project layout to support multiple Firebolt surfaces (Core, Crypto, Media) with a flexible transport architecture. Currently, the repository is empty aside from OpenSpec configuration. This change establishes the foundational project structure, CMake build system, and example implementations that enable surfaces to be developed independently with runtime transport injection for testability and partner extensibility.

## What Changes

- Create CMake-based C++ 17 project structure with out-of-tree build support
- Establish directory layout for surface clients (Core, Crypto, Media) with separate client libraries
- Create pure header-only `IFireboltTransport` interface for runtime dependency injection
- Implement simple example transport implementation (without external dependencies)
- Implement simple example client implementation demonstrating transport injection
- Create shared utilities structure as CMake object library
- Set up unit test structure alongside components
- Create integration test directory structure
- Add CMake modules for manual dependency finding (for Yocto compatibility)

## Capabilities

### New Capabilities

This is infrastructure/setup work that establishes the project structure without defining system behavior. The actual behavior for each surface will be defined in subsequent changes. Setting `skip_specs: true` since no spec-level behavior changes are introduced.

### Modified Capabilities

None

## Impact

- Establishes the foundational build system and directory structure for all future Firebolt C++ client development
- Enables surfaces to be developed and tested independently
- Provides a template for adding new surfaces in the future
- Supports Yocto builds with manual CMake find modules
- Enables partners to implement custom transports by implementing IFireboltTransport

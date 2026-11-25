# Design

## Context

The firebolt-native project currently has only one manually-generated header (Lifecycle.h) as an example. The firebolt-js-client repository contains 13 API module specs with YAML frontmatter that define the complete API surface. These specs follow the OpenSpec format with modules, types, properties, actions, and events. The existing header framework (FireboltModel, FireboltObject, FireboltOperation, FireboltSubscription, FireboltResult) is already in place and provides the foundation for generated headers. The project needs to scale from one example header to 13 module headers without manual effort. Specs will be copied locally to openspec/specs/api/core/ from the current firebolt-js-client snapshot, allowing independent versioning and future organization for secure paths.

## Goals / Non-Goals

**Goals:**
- Create a Python-based generator that reads YAML specs and produces C++ headers
- Generate headers for all 13 API modules with correct platform filtering
- Maintain consistency with existing Lifecycle.h pattern
- Handle type mapping, enum serialization, and constraint documentation
- Generate integration tests following the existing test pattern
- Commit generated files to repository with manual regeneration workflow

**Non-Goals:**
- Implementing a full AST builder (direct YAML-to-header is sufficient for C++-only scope)
- Auto-generating headers on every build (manual regeneration is acceptable)
- Generating secure path headers (scope is Core client only)
- Implementing setters for properties (no writable properties in current specs)
- Generating headers for web-only modules or APIs

## Decisions

### Python with PyYAML and Jinja2

Use Python as the implementation language with PyYAML for parsing and Jinja2 for templating.

**Rationale:**
- Python has excellent YAML parsing support via PyYAML
- Jinja2 is a mature templating engine ideal for code generation
- Easy CMake integration via add_custom_command
- Widely supported and scalable for future enhancements
- No external runtime dependencies in the C++ build (generator runs separately)

**Alternatives considered:**
- C++ with YAML-CPP: Rejected - more complex, less expressive for templating
- TypeScript with existing AST builder: Rejected - adds Node.js dependency to C++ build
- Custom C++ AST parser: Rejected - reimplements parsing logic already available in firebolt-js-client

### Direct YAML-to-Header Approach

Parse YAML specs directly and generate headers without an intermediate AST layer.

**Rationale:**
- Simpler architecture for C++-only scope
- Full control over C++-specific idioms and patterns
- Can use existing Lifecycle.h as a direct template
- No dependency on TypeScript/Node.js toolchain in C++ build

**Alternatives considered:**
- Reuse firebolt-js-client AST builder: Rejected - requires Node.js in build, AST may have web-specific assumptions
- Custom C++ AST: Rejected - reimplements YAML parsing and AST logic

### Local Spec Organization

Copy specs from firebolt-js-client to local openspec/specs/api/core/<module>/spec.md structure with current snapshot as starting point.

**Rationale:**
- Independent versioning - firebolt-native and firebolt-js-client can update at different rates
- Full control over local organization (api/core/ for current, api/secure/ for future)
- No external dependency on firebolt-js-client checkout location after initial copy
- Clear separation between core and secure paths for future extensibility
- Current snapshot provides stable starting point for native implementation

**Alternatives considered:**
- External reference to firebolt-js-client: Rejected - ties to external checkout location, harder to organize for secure paths
- Git submodule: Rejected - adds complexity, submodule management overhead
- Symlink structure: Rejected - platform-specific, harder to manage across environments

### Namespace Structure: firebolt::core::<Module>

Use namespace structure firebolt::core::<module> where <module> is the lowercase module name.

**Rationale:**
- Cleaner than deeply nested firebolt::core::lifecycle::lifecycle
- Matches module names from specs (Device, Accessibility, etc.)
- Consistent with existing pattern while avoiding redundancy

**Alternatives considered:**
- firebolt::core::<Module>::<Module>: Rejected - redundant nesting
- firebolt::<Module>: Rejected - loses core/secure distinction

### Enum Serialization with Inline Logic

Generate enum classes with safe identifiers and include inline toString/fromString logic in model write and read methods.

**Rationale:**
- Header-only APIs avoid separate implementation files
- Inline conversion keeps serialization logic close to model definition
- Safe identifiers (e.g., AppAdult from "app:adult") ensure valid C++ identifiers
- No external helper functions needed

**Alternatives considered:**
- Separate serialization helper functions: Rejected - adds complexity for header-only pattern
- Store wire values as enum underlying type: Rejected - C++ enums cannot have string underlying types

### Optional Parameters with std::optional

Map optional parameters (required: false) to std::optional<T> with default std::nullopt.

**Rationale:**
- Idiomatic C++17 for optional values
- Clear distinction between required and optional parameters
- Matches existing patterns in modern C++ codebases

**Alternatives considered:**
- Method overloading: Rejected - combinatorial explosion with multiple optional params
- Default-constructed values: Rejected - ambiguous between "not provided" and "provided as default"

### Constraint Comments as Inline Documentation

Emit constraint comments on fields and method signatures using format "Constraints: minimum=X, maximum=Y".

**Rationale:**
- IDEs pick up comments for tooltips and documentation
- Can be used to prepare external documentation
- Non-intrusive, doesn't affect generated code behavior
- Preserves spec constraints in generated code

**Alternatives considered:**
- Skip constraints: Rejected - loses valuable information from specs
- Type-safe annotations: Rejected - C++17 lacks comprehensive constraint annotation system

### Manual Regeneration Workflow

Generate headers once, commit to repository, and regenerate manually when specs change.

**Rationale:**
- Build time overhead avoided (no generation on every build)
- Generated files are versioned and reviewed like other code
- Clear when regeneration is needed (spec changes)
- Simpler CMake integration

**Alternatives considered:**
- Auto-generate on every build: Rejected - adds build time overhead, unnecessary for infrequent spec changes
- Pre-commit hook: Rejected - adds friction, manual regeneration is sufficient

### Shared Module: Only AgePolicy Enum

Generate only the AgePolicy enum from the shared module, skipping ListenResponse and FireboltError.

**Rationale:**
- ListenResponse is internal JSON-RPC protocol detail
- FireboltError is already handled by existing FireboltResult<T> pattern
- AgePolicy is the only shared type referenced by other modules
- Keeps generated headers focused on application-facing APIs

**Alternatives considered:**
- Generate all shared types: Rejected - exposes internal protocol details to applications
- Skip shared module entirely: Rejected - AgePolicy is needed by other modules

## Architecture

```
GENERATOR ARCHITECTURE
======================

openspec/specs/api/core/<module>/spec.md (local copy from firebolt-js-client)
         |
         v  [Python Generator: scripts/generate_headers.py]
    1. Parse YAML frontmatter
    2. Filter by platform (native or both)
    3. For each module:
       a. Generate model types (enums, structs)
       b. Generate interface class
       c. Generate implementation class
       d. Generate interface getter function
    4. For each module:
       a. Generate integration test file
    5. Write outputs:
       - Headers: include/firebolt/core/<Module>.h
       - Tests: tests/integration/test_core_<module>.cpp


SPEC STRUCTURE
==============

openspec/specs/api/
├── core/
│   ├── accessibility/spec.md
│   ├── actions/spec.md
│   ├── advertising/spec.md
│   ├── device/spec.md
│   ├── discovery/spec.md
│   ├── display/spec.md
│   ├── lifecycle2/spec.md
│   ├── localization/spec.md
│   ├── metrics/spec.md
│   ├── network/spec.md
│   ├── video-output/spec.md
│   └── shared/spec.md
└── secure/ (future)
    ├── core/
    │   └── <module>/spec.md
    └── media/
        └── <module>/spec.md


HEADER STRUCTURE
================

include/firebolt/core/
├── Accessibility.h
├── Actions.h
├── Advertising.h
├── Device.h
├── Discovery.h
├── Display.h
├── Lifecycle2.h
├── Localization.h
├── Metrics.h
├── Network.h
├── VideoOutput.h
└── Shared.h (AgePolicy enum only)


TEST STRUCTURE
==============

tests/integration/
├── test_core_accessibility.cpp
├── test_core_actions.cpp
├── test_core_advertising.cpp
├── test_core_device.cpp
├── test_core_discovery.cpp
├── test_core_display.cpp
├── test_core_lifecycle2.cpp
├── test_core_localization.cpp
├── test_core_metrics.cpp
├── test_core_network.cpp
└── test_core_video_output.cpp


TYPE MAPPING
============

Spec Type          C++ Type
-----------        ----------
bool               bool
string             std::string
unsigned           uint64_t
double             double
string+date-time    std::string (documented as ISO 8601)
array<T>           std::vector<T>
$ref enum          enum class
$ref object        struct


HEADER TEMPLATE PATTERN
=======================

namespace firebolt {
namespace core {

namespace <module> {

// Primitive type aliases
using FireboltString = std::string;

// Model types
class <Type> : public FireboltModel<<Type>> {
public:
    <FieldType> <fieldName>; // Constraints: ...
    void write(FireboltObject& object) const override;
    void read(const FireboltObject& object) override;
};

// Interface
class I<Module> {
public:
    virtual ~I<Module>() = default;
    virtual FireboltOperation<<ResultType>> <Method>(<Params>) = 0;
};

// Implementation
class <Module> : public I<Module> {
public:
    explicit <Module>(FireboltClient& client);
    FireboltOperation<<ResultType>> <Method>(<Params>) override;
private:
    FireboltClient& m_client;
};

// Interface getter
inline I<Module>& <Module>Interface(FireboltClient& client);

} // namespace <module>
} // namespace core
} // namespace firebolt


ENUM IDENTIFIER SANITIZATION
=============================

Wire Value        Safe Identifier
-----------        ----------------
"app:adult"       AppAdult
"hdcp2.2"         Hdcp2_2
"bt709"           Bt709
"initializing"    Initializing
```

## Risks / Trade-offs

**Risk:** Spec format changes in firebolt-js-client may break the generator
→ **Mitigation:** Generator is versioned with the codebase; spec format changes require generator updates. Document spec format version in generator code.

**Risk:** Enum identifier collisions when sanitizing wire values
→ **Mitigation:** Use deterministic sanitization rules (replace special chars with underscores, prefix with digit if starts with number). Review generated enum identifiers after first run.

**Risk:** Platform filtering logic may miss edge cases
→ **Mitigation:** Comprehensive test coverage with existing modules. Verify all 13 modules generate correctly. Add unit tests for platform filtering logic.

**Risk:** Manual regeneration workflow may be forgotten when specs change
→ **Mitigation:** Document regeneration workflow in README. Consider adding pre-commit hook reminder in future if needed.

**Risk:** Python dependencies may have known vulnerabilities
→ **Mitigation:** Pin exact versions in requirements.txt. Run vulnerability scanning (pip-audit or safety) during setup and document security workflow in README. Update dependencies when vulnerabilities are found.

**Trade-off:** Direct YAML-to-header vs reusing AST builder
→ **Acceptance:** Direct approach is simpler for C++-only scope and avoids Node.js dependency. If future needs require AST, can refactor later.

**Trade-off:** Manual regeneration vs auto-generation on build
→ **Acceptance:** Manual regeneration avoids build time overhead and keeps generated files under version control. Specs change infrequently, so manual workflow is acceptable.

## Migration Plan

This is a new capability with no existing implementation to migrate. The adoption plan is:

1. Implement generator script (scripts/generate_headers.py)
2. Run generator to produce initial set of 13 headers and 13 tests
3. Review generated headers for correctness against existing Lifecycle.h pattern
4. Commit generated files to repository
5. Update README with generator usage instructions
6. Remove manually-generated Lifecycle.h (or keep as reference until verified)

**Rollback strategy:**
- Generated files are new additions; existing code continues to work
- If issues arise, keep manually-generated headers and disable generator
- Remove generator script and generated files from build

## Open Questions

None - the design is complete and addresses all requirements from the spec. Future enhancements (secure path headers, AST builder integration, auto-generation on build) can be added in separate proposals if needed.

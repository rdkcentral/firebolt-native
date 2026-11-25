# Spec Delta

## Purpose

Automates generation of C++ API headers from OpenSpec YAML specifications, ensuring consistency between spec definitions and native implementation while reducing manual maintenance effort.

## ADDED Requirements

### Requirement: Specs are copied from firebolt-js-client to local structure
The system SHALL copy API module specs from firebolt-js-client openspec/specs/api to local openspec/specs/api/core/<module>/spec.md structure, using the current snapshot as the starting point.

#### Scenario: Copy all 13 module specs
- **WHEN** the initial setup is performed
- **THEN** all 13 module specs are copied to openspec/specs/api/core/<module>/spec.md

#### Scenario: Preserve spec format
- **WHEN** specs are copied
- **THEN** the YAML frontmatter and content remain unchanged

#### Scenario: Support future secure path organization
- **WHEN** the local spec structure is created
- **THEN** it allows future addition of api/secure/<surface>/<module>/spec.md paths

### Requirement: Generator parses YAML spec frontmatter
The system SHALL parse YAML frontmatter from spec.md files in the local openspec/specs/api/core directory, extracting module metadata, type definitions, properties, actions, and events.

#### Scenario: Parse module metadata
- **WHEN** the generator reads a spec.md file
- **THEN** it extracts module name, version, platform, stability, and description from the YAML frontmatter

#### Scenario: Parse type definitions
- **WHEN** the generator encounters a types section
- **THEN** it extracts all enum and object type definitions with their properties and values

#### Scenario: Parse API definitions
- **WHEN** the generator encounters properties, actions, or events sections
- **THEN** it extracts all API definitions with their parameters, results, and platform overrides

### Requirement: Generator filters by platform
The system SHALL filter modules and APIs based on platform classification, processing only modules with platform "native" or "both", and skipping web-only modules and web-only APIs within both-platform modules.

#### Scenario: Skip web-only module
- **WHEN** a module has platform "web"
- **THEN** the generator skips generating headers for that module

#### Scenario: Process native-only module
- **WHEN** a module has platform "native"
- **THEN** the generator generates headers for that module

#### Scenario: Process both-platform module
- **WHEN** a module has platform "both"
- **THEN** the generator generates headers for that module

#### Scenario: Skip web-only API in both-platform module
- **WHEN** an API within a both-platform module has platform "web"
- **THEN** the generator skips that specific API

#### Scenario: Process native API in both-platform module
- **WHEN** an API within a both-platform module has platform "native" or no platform override
- **THEN** the generator generates that API

### Requirement: Generator maps spec types to C++ types
The system SHALL map spec primitive types and type references to appropriate C++ types, including bool, string, unsigned, double, array, enum, and object types.

#### Scenario: Map primitive types
- **WHEN** the generator encounters a primitive type
- **THEN** it maps bool to bool, string to std::string, unsigned to uint64_t, and double to double

#### Scenario: Map array types
- **WHEN** the generator encounters an array type
- **THEN** it maps it to std::vector<T> where T is the item type

#### Scenario: Map enum types
- **WHEN** the generator encounters an enum type reference
- **THEN** it maps it to an enum class with safe identifiers

#### Scenario: Map object types
- **WHEN** the generator encounters an object type reference
- **THEN** it maps it to a struct with the appropriate fields

#### Scenario: Map date-time format
- **WHEN** the generator encounters a string with format date-time
- **THEN** it maps it to std::string with ISO 8601 format documentation

### Requirement: Generator handles enum serialization with safe identifiers
The system SHALL generate enum classes with safe identifiers derived from wire values, and include inline toString and fromString logic in model write and read methods for serialization.

#### Scenario: Generate safe enum identifiers
- **WHEN** an enum value has a wire value containing non-identifier characters
- **THEN** the generator creates a safe C++ identifier (e.g., "app:adult" becomes AppAdult, "hdcp2.2" becomes Hdcp2_2)

#### Scenario: Serialize enum to string
- **WHEN** a model writes an enum field to FireboltObject
- **THEN** the write method converts the enum value to its wire string representation

#### Scenario: Deserialize string to enum
- **WHEN** a model reads an enum field from FireboltObject
- **THEN** the read method converts the wire string to the appropriate enum value

### Requirement: Generator emits constraint comments
The system SHALL emit inline comments documenting numeric and string constraints on fields and method signatures, using the format "Constraints: minimum=X, maximum=Y" or "Constraints: minLength=A, maxLength=B, pattern=C".

#### Scenario: Emit numeric constraint comment
- **WHEN** a field has minimum or maximum constraints
- **THEN** the generator emits a comment with the constraint values

#### Scenario: Emit string constraint comment
- **WHEN** a field has minLength, maxLength, or pattern constraints
- **THEN** the generator emits a comment with the constraint values

#### Scenario: No constraint comment when unconstrained
- **WHEN** a field has no constraints
- **THEN** the generator does not emit a constraint comment

### Requirement: Generator handles optional parameters
The system SHALL map optional parameters (required: false) to std::optional<T> with default value std::nullopt in method signatures.

#### Scenario: Generate optional parameter
- **WHEN** a parameter has required: false
- **THEN** the method signature uses std::optional<T> with default std::nullopt

#### Scenario: Generate required parameter
- **WHEN** a parameter has required: true
- **THEN** the method signature uses a plain reference or value type without optional wrapper

### Requirement: Generator generates C++ headers following existing pattern
The system SHALL generate C++ headers following the pattern established by Lifecycle.h, including model types, interface class, implementation class, and interface getter function, with namespace structure firebolt::core::<Module>.

#### Scenario: Generate model types
- **WHEN** the generator processes type definitions
- **THEN** it generates CRTP-based model structs inheriting from FireboltModel with write and read methods

#### Scenario: Generate interface class
- **WHEN** the generator processes API definitions
- **THEN** it generates an interface class with virtual methods for properties, actions, and events

#### Scenario: Generate implementation class
- **WHEN** the generator processes API definitions
- **THEN** it generates an implementation class that accepts FireboltClient& and calls Send or Subscribe

#### Scenario: Generate interface getter
- **WHEN** the generator completes a module
- **THEN** it generates an inline function that returns the interface from client.GetInterface<Module>()

#### Scenario: Use correct namespace structure
- **WHEN** the generator generates a module header
- **THEN** it uses namespace firebolt::core::<module> where <module> is the lowercase module name

### Requirement: Generator generates integration tests
The system SHALL generate integration test files for each module following the test_core_lifecycle.cpp pattern, including basic instantiation, method call, and event subscription tests.

#### Scenario: Generate basic instantiation test
- **WHEN** the generator creates a test file
- **THEN** it includes a test that instantiates the module with FireboltClient

#### Scenario: Generate method call test
- **WHEN** the generator processes actions or properties
- **THEN** it includes a test that calls a method and verifies the operation is valid

#### Scenario: Generate event subscription test
- **WHEN** the generator processes events
- **THEN** it includes a test that subscribes to an event and verifies the subscription is active

#### Scenario: Generate event injection tests
- **WHEN** the generator creates a test file for a module with events
- **THEN** it includes tests for event injection with result field, params field, and preference for result over params

### Requirement: Generator handles shared module types
The system SHALL generate a Shared.h header containing only the AgePolicy enum from the shared module, skipping ListenResponse and FireboltError as they are internal-only types not exposed to applications.

#### Scenario: Generate shared enum
- **WHEN** the generator processes the shared module
- **THEN** it generates a Shared.h header with the AgePolicy enum

#### Scenario: Skip internal shared types
- **WHEN** the generator encounters ListenResponse or FireboltError in the shared module
- **THEN** it does not generate headers for these types

### Requirement: Generator commits generated files to repository
The system SHALL generate headers and test files that are committed to the repository, with regeneration performed manually when specs change rather than automatically on every build.

#### Scenario: Generate to correct output directory
- **WHEN** the generator runs
- **THEN** it writes headers to include/firebolt/core/ and tests to tests/integration/

#### Scenario: Manual regeneration workflow
- **WHEN** specs change in firebolt-js-client
- **THEN** developers manually run the generator script and commit the updated files

### Requirement: Generator generates all 13 API modules
The system SHALL generate headers and tests for all 13 API modules: Accessibility, Actions, Advertising, Device, Discovery, Display, Lifecycle2, Localization, Metrics, Network, VideoOutput, and Shared.

#### Scenario: Generate all module headers
- **WHEN** the generator runs with the spec directory
- **THEN** it produces 13 header files in include/firebolt/core/

#### Scenario: Generate all module tests
- **WHEN** the generator runs with the spec directory
- **THEN** it produces 13 test files in tests/integration/

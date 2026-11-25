# Tasks

## 1. Setup

- [x] 1.1 Create scripts directory if it does not exist and verify directory structure is correct
- [x] 1.2 Create openspec/specs/api/core directory structure for local specs, and verify directory is created
- [x] 1.3 Create generator script scaffold (scripts/generate_headers.py) with argument parsing for spec-dir, output-dir, and test-dir, and verify script runs with --help
- [x] 1.4 Create requirements.txt with pinned versions of PyYAML and Jinja2 (e.g., PyYAML==6.0.1, Jinja2==3.1.3), and verify requirements.txt specifies exact versions
- [x] 1.5 Install dependencies from requirements.txt and verify installation succeeds
- [x] 1.6 Run vulnerability scanner (pip-audit or safety) on requirements.txt and verify no known vulnerabilities are reported

## 2. Spec Copy

- [x] 2.1 Copy all 13 module specs from firebolt-js-client/openspec/specs/api to local openspec/specs/api/core/<module>/spec.md, and verify all 13 spec files are present
- [x] 2.2 Verify copied specs preserve YAML frontmatter and content unchanged, and verify spec format is intact
- [x] 2.3 Document spec sync workflow in README (how to update from firebolt-js-client when needed), and verify workflow is documented

## 3. YAML Parser

- [x] 3.1 Implement YAML frontmatter parser to extract module metadata (name, version, platform, stability, description) from spec.md files, and verify parser correctly extracts metadata from lifecycle2/spec.md
- [x] 3.2 Implement type definition parser to extract enums and objects from types section, and verify parser correctly extracts all types from device/spec.md
- [x] 3.3 Implement API definition parser to extract properties, actions, and events with their parameters, results, and platform overrides, and verify parser correctly extracts all APIs from accessibility/spec.md

## 4. Platform Filter

- [x] 4.1 Implement module-level platform filtering to skip web-only modules, and verify Lifecycle2 (native) is included while web-only modules are skipped
- [x] 4.2 Implement API-level platform filtering to skip web-only APIs within both-platform modules, and verify Display.colorimetry (web-only) is skipped while Device.uid (both) is included
- [x] 4.3 Add unit tests for platform filtering logic with edge cases (native module, both module, web-only API), and verify all tests pass

## 5. Type Mapper

- [x] 5.1 Implement primitive type mapping (bool, string, unsigned, double), and verify all primitive types map correctly
- [x] 5.2 Implement array type mapping to std::vector<T>, and verify array types map correctly
- [x] 5.3 Implement date-time format handling to map to std::string with ISO 8601 documentation, and verify date-time fields are documented correctly
- [x] 5.4 Implement enum type reference mapping to enum class, and verify enum references resolve correctly
- [x] 5.5 Implement object type reference mapping to struct, and verify object references resolve correctly
- [x] 5.6 Implement shared module type reference resolution (shared: prefix), and verify shared types like AgePolicy resolve correctly

## 6. Enum Handler

- [x] 6.1 Implement enum identifier sanitization to convert wire values to safe C++ identifiers (e.g., "app:adult" → AppAdult, "hdcp2.2" → Hdcp2_2), and verify all enum values from Lifecycle2 and Device sanitize correctly
- [x] 6.2 Implement inline enum serialization logic in model write methods (enum → string), and verify write methods convert enums to wire strings
- [x] 6.3 Implement inline enum deserialization logic in model read methods (string → enum), and verify read methods convert wire strings to enum values
- [x] 6.4 Add unit tests for enum sanitization and serialization, and verify all tests pass

## 7. Constraint Emitter

- [x] 7.1 Implement numeric constraint comment emission (minimum, maximum), and verify numeric constraints appear as comments on fields
- [x] 7.2 Implement string constraint comment emission (minLength, maxLength, pattern), and verify string constraints appear as comments on fields
- [x] 7.3 Ensure no constraint comments are emitted for unconstrained fields, and verify unconstrained fields have no constraint comments
- [x] 7.4 Add unit tests for constraint comment generation, and verify all tests pass

## 8. Optional Parameter Handler

- [x] 8.1 Implement optional parameter mapping to std::optional<T> with default std::nullopt, and verify optional parameters use std::optional in method signatures
- [x] 8.2 Ensure required parameters use plain types without optional wrapper, and verify required parameters do not use std::optional
- [x] 8.3 Add unit tests for optional parameter generation, and verify all tests pass

## 9. Header Generator

- [x] 9.1 Create Jinja2 template for model types (enums and structs) with CRTP inheritance, write/read methods, and constraint comments, and verify template renders valid C++ code
- [x] 9.2 Create Jinja2 template for interface class with virtual methods for properties, actions, and events, and verify template renders valid C++ code
- [x] 9.3 Create Jinja2 template for implementation class with FireboltClient& constructor and Send/Subscribe calls, and verify template renders valid C++ code
- [x] 9.4 Create Jinja2 template for interface getter function using client.GetInterface<Module>(), and verify template renders valid C++ code
- [x] 9.5 Implement header file generation with correct namespace structure (firebolt::core::<module>), and verify generated headers use correct namespaces
- [x] 9.6 Implement Shared.h generation with only AgePolicy enum (skip ListenResponse and FireboltError), and verify Shared.h contains only AgePolicy
- [x] 9.7 Generate all 13 module headers (Accessibility, Actions, Advertising, Device, Discovery, Display, Lifecycle2, Localization, Metrics, Network, VideoOutput, Shared), and verify all headers compile without errors

## 10. Test Generator

- [x] 10.1 Create Jinja2 template for integration test file following test_core_lifecycle.cpp pattern, and verify template renders valid C++ test code
- [x] 10.2 Implement test generation with basic instantiation test, method call test, and event subscription test, and verify generated tests include all required test cases
- [x] 10.3 Implement event injection test generation (result field, params field, prefer result over params) for modules with events, and verify event injection tests are present
- [x] 10.4 Generate all 13 module integration tests, and verify all tests compile without errors

## 11. Generator Integration

- [x] 11.1 Run generator script with local openspec/specs/api/core directory and verify all 13 headers and 13 tests are generated to correct output directories
- [x] 11.2 Review generated headers against existing Lifecycle.h pattern and verify structure, naming, and patterns are consistent
- [x] 11.3 Compile generated headers with existing build system and verify compilation succeeds with zero errors and warnings
- [x] 11.4 Compile generated tests with existing build system and verify compilation succeeds with zero errors and warnings
- [x] 11.5 Run generated integration tests and verify all tests pass

## 12. Build System Integration

- [x] 12.1 Add CMake custom command or documentation for running the generator script, and verify command can be invoked from build directory
- [x] 12.2 Update README.md with generator usage instructions (how to run, when to regenerate, spec sync workflow, dependency installation, and vulnerability scanning), and verify README documents the complete generator workflow including security checks

## 13. Verification and Cleanup

- [x] 13.1 Manually review generated headers for edge cases (complex nested types, multiple optional parameters, enum with special characters), and verify all edge cases are handled correctly
- [x] 13.2 Compare generated Lifecycle2.h against manually-generated Lifecycle.h and verify they are functionally equivalent (or replace Lifecycle.h with generated version)
- [x] 13.3 Add generator script to version control and verify script is committed with proper executable permissions
- [x] 13.4 Commit all generated headers, tests, and local specs to repository and verify all files are under version control

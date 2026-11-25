# Proposal

## Why

The current firebolt-native project has only one manually-generated header (Lifecycle.h) as an example. The firebolt-js-client repository contains 13 API module specs that need to be converted to C++ headers for the native implementation. Manually writing these headers is error-prone, time-consuming, and hard to maintain as specs evolve. We need an automated generator to produce headers from the YAML specs, ensuring consistency and reducing maintenance burden.

## What Changes

- Copy current snapshot of 13 API module specs from firebolt-js-client to local `openspec/specs/api/core/<module>/spec.md` structure
- Create a Python-based generator script (`scripts/generate_headers.py`) that reads YAML specs from local openspec/specs directory
- Generate C++ headers for 13 API modules (Accessibility, Actions, Advertising, Device, Discovery, Display, Lifecycle2, Localization, Metrics, Network, VideoOutput, and Shared)
- Generate integration tests for each module following the existing test_core_lifecycle.cpp pattern
- Filter generated APIs by platform (native or both, skip web-only modules and APIs)
- Implement type mapping from spec primitives to C++ types
- Handle enum serialization with safe identifiers and inline conversion logic
- Emit constraint comments for numeric and string constraints
- Integrate generator into the build system for manual regeneration (not auto-generated every build)
- Structure local specs to support future secure path organization (api/secure/<surface>/<module>/spec.md)

## Capabilities

### New Capabilities

- `spec-header-generator`: Automated generation of C++ API headers from OpenSpec YAML specifications, including platform filtering, type mapping, enum handling, constraint documentation, and test generation for native platform APIs.

### Modified Capabilities

None

## Impact

Affected systems:
- New local spec directory: `openspec/specs/api/core/` (13 spec.md files copied from firebolt-js-client)
- New generator script: `scripts/generate_headers.py` (Python with PyYAML and Jinja2)
- New generated headers in `include/firebolt/core/` (13 .hpp files)
- New integration tests in `tests/integration/` (13 .cpp test files)
- Build system integration for manual header regeneration
- Spec organization supports future secure path structure (api/secure/<surface>/<module>/spec.md)

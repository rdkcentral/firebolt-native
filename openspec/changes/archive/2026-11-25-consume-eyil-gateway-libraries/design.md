# Design

## Context

Firebolt-native currently builds protobuf bindings locally from duplicated proto files in `src/proto/` and `src/crypto/client/proto/`. The `src/proto/CMakeLists.txt` generates C++ bindings using `protobuf_generate_cpp()` and creates a custom include hierarchy (`firebolt/proto/`) to match the code's include pattern (`#include "firebolt/proto/rpc.pb.h"`). The generated `firebolt-proto` library is linked by `firebolt-secure-client` and other components.

The eyil-secure-gateway repository provides canonical proto definitions and exports them via the EyilProtocol CMake package. It also provides EyilClient for the client library. Both are installed to standard system paths (`/usr`) when built with `-DCMAKE_INSTALL_PREFIX=/usr`.

See proposal.md for motivation.

## Goals / Non-Goals

**Goals:**
- Remove all duplicated proto files from firebolt-native
- Replace local protobuf generation with EyilProtocol and EyilClient dependencies
- Enable local docker builds that mirror the Yocto build sequence (build eyil-secure-gateway first, then firebolt-native)
- Maintain compatibility with existing firebolt-native code that uses protobuf bindings

**Non-Goals:**
- Changes to eyil-secure-gateway (it remains unaware of firebolt-native)
- Changes to firebolt-native's external API or behavior
- Yocto recipe updates (out of scope for this change)

## Decisions

### Dependency Resolution: find_package() with CMake

Use `find_package(EyilProtocol REQUIRED)` and `find_package(EyilClient REQUIRED)` in firebolt-native's root CMakeLists.txt. The eyil-secure-gateway installs CMake config files to `/usr/lib/cmake/EyilProtocol/` and `/usr/lib/cmake/EyilClient/`, which CMake's find_package can locate.

**Rationale:** This is the standard CMake pattern for consuming pre-installed packages. It matches how Yocto will resolve the dependency and works for local docker builds when eyil-secure-gateway is installed first.

**Alternatives considered:**
- `add_subdirectory(../eyil-secure-gateway)`: Would couple the build trees and require both repos to be built together, violating the goal of keeping eyil-secure-gateway unaware of firebolt-native.
- `FetchContent`: Would download eyil-secure-gateway at build time, which doesn't match the Yocto model where packages are pre-built.

### Include Path Migration: eyilprotocol namespace

EyilProtocol installs headers to `/usr/include/eyilprotocol/` (e.g., `eyilprotocol/rpc.pb.h`). Firebolt-native code currently includes `firebolt/proto/rpc.pb.h`. Update include statements to use the eyilprotocol namespace.

**Rationale:** Using the upstream package's install namespace avoids fragile header-copying logic and matches the intended consumption pattern. EyilProtocol's CMake config provides the include directory via `target_include_directories(EyilProtocol::EyilProtocol PUBLIC ...)`.

**Alternatives considered:**
- Copy headers to `firebolt/proto/` hierarchy: Would require custom install logic and maintenance overhead. The current CMakeLists.txt already has complex custom commands for this; removing that complexity is a benefit of consuming EyilProtocol.

### RPC Service Enum Mapping: RPC_SERVICE_CORE vs RPC_SERVICE_JSON_RPC

Firebolt-native's current `rpc.proto` defines `RPC_SERVICE_JSON_RPC = 2`. Eyil-secure-gateway's `rpc.proto` defines `RPC_SERVICE_CORE = 2`. Both represent the same service (JSON-RPC wrapped in protobuf). Update firebolt-native code to use `RPC_SERVICE_CORE` from EyilProtocol.

**Rationale:** RPC_SERVICE_CORE is the canonical name in eyil-secure-gateway. The numeric value (2) is identical, so this is a naming-only change with no wire protocol impact.

**Alternatives considered:**
- Keep RPC_SERVICE_JSON_RPC: Would require maintaining a local enum or mapping layer, adding complexity without benefit.

### Docker Build: Two-Stage Sequential Build

Create a Dockerfile similar to eyil-secure-gateway (Ubuntu 24.04, cmake, ninja, protobuf, zmq dependencies). Provide a build script that:
1. Builds eyil-secure-gateway with `-DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=lib`
2. Installs eyil-secure-gateway (`cmake --install`)
3. Builds firebolt-native, which finds the installed EyilProtocol and EyilClient via find_package()

**Rationale:** This matches the Yocto sequence where eyil-secure-gateway is built and installed first. Using the same Docker image ensures dependency compatibility (protobuf version, etc.).

**Alternatives considered:**
- Multi-stage Dockerfile: Would work but adds complexity; a sequential script is simpler and easier to debug.
- Separate docker images: Would risk environment divergence; using the same base image ensures consistency.

### Protobuf Version Coupling: Exact Match Required

EyilProtocolConfig.cmake requires `find_dependency(Protobuf @Protobuf_VERSION@ EXACT MODULE)`. This means firebolt-native must use the exact same Protobuf version as eyil-secure-gateway.

**Rationale:** The protobuf runtime and generated bindings must be ABI-compatible. Using different versions risks runtime crashes or subtle bugs.

**Trade-off:** This tightens version coupling but is necessary for correctness. In practice, both repos will be built together in the same environment, so version alignment is manageable.

## Risks / Trade-offs

### Risk: Include Path Updates May Miss Files

**Risk:** Code in multiple locations may include protobuf headers using the old pattern, causing build failures after the migration.

**Mitigation:** Use grep to find all occurrences of `#include "firebolt/proto/` and update them systematically. Run a full build after changes to catch any missed includes.

### Risk: Docker Build Script Complexity

**Risk:** The two-stage docker build script may be error-prone or hard for developers to understand.

**Mitigation:** Keep the script simple and well-commented. Provide clear documentation in README.md. Test the script end-to-end before completing the change.

### Trade-off: Tighter Dependency Coupling

Firebolt-native now requires eyil-secure-gateway to be built and installed first. This increases build complexity but eliminates code duplication and maintenance burden.

### Risk: Protobuf ABI Incompatibility

**Risk:** If eyil-secure-gateway and firebolt-native ever use different protobuf versions, the build will fail (due to the EXACT requirement) or runtime may crash (if the requirement were relaxed).

**Mitigation:** The EXACT requirement prevents silent ABI mismatches. In Yocto, the recipe will ensure version alignment. For local docker builds, using the same Docker image for both builds guarantees alignment.

## Migration Plan

1. **Phase 1: Update CMake Dependencies**
   - Add `find_package(EyilProtocol REQUIRED)` and `find_package(EyilClient REQUIRED)` to root CMakeLists.txt
   - Remove `add_subdirectory(src/proto)` from root CMakeLists.txt
   - Update `firebolt-secure-client` and other targets to link `EyilProtocol::EyilProtocol` instead of `firebolt-proto`
   - Add `EyilClient::EyilClient` to targets that need the client library (e.g., secure transport)

2. **Phase 2: Update Include Paths**
   - Search and replace `#include "firebolt/proto/` with `#include "eyilprotocol/`
   - Update enum references from `RPC_SERVICE_JSON_RPC` to `RPC_SERVICE_CORE`
   - Remove the custom header-hierarchy logic from `src/proto/CMakeLists.txt` (delete the entire file)

3. **Phase 3: Remove Duplicated Files**
   - Delete `src/proto/` directory (rpc.proto, core.proto, CMakeLists.txt)
   - Delete `src/crypto/client/proto/` directory (rpc.proto, crypto.proto)

4. **Phase 4: Docker Build Setup**
   - Create Dockerfile matching eyil-secure-gateway's (Ubuntu 24.04, cmake, ninja, protobuf, zmq, ssl dependencies)
   - Create build script that builds eyil-secure-gateway with install, then builds firebolt-native
   - Update README.md with docker build instructions

5. **Phase 5: Validation**
   - Run full build in docker environment
   - Run existing tests to verify no behavior changes
   - Verify that generated protobuf bindings from EyilProtocol are correctly linked

**Rollback Strategy:** Since there is no active usage of the duplicated proto files (per proposal), rollback is not a concern. The change can be reverted by restoring the removed files and reverting CMake changes if issues arise.

## Open Questions

None. The approach is straightforward: remove duplicated code, consume upstream packages, update includes. All technical decisions are resolved above.

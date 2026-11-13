# Tasks

## 1. CMake Build System Setup

- [x] 1.1 Create root CMakeLists.txt with C++ 17 standard and project setup, and verify `cmake -S . -B build` succeeds
- [x] 1.2 Create cmake/CompilerFlags.cmake with common compiler flags, and verify it's included in root CMakeLists.txt
- [x] 1.3 Create placeholder cmake/Find*.cmake files for future dependencies (Soup, Protobuf, gRPC, ZMQ), and verify files exist in cmake/ directory

## 2. Common Utilities Structure

- [x] 2.1 Create src/common directory structure (include, src, tests/unit), and verify directory layout matches design
- [x] 2.2 Create src/common/CMakeLists.txt with CMake object library configuration, and verify CMake parses without errors
- [x] 2.3 Add simple utility headers (Logging.h, Utils.h) in src/common/include/firebolt/common/ with Apache 2.0 license header, and verify headers compile
- [x] 2.4 Add simple utility implementations (Logging.cpp, Utils.cpp) in src/common/src/ with Apache 2.0 license header, and verify they compile
- [x] 2.5 Add placeholder unit test file in src/common/tests/unit/, and verify test directory structure exists

## 3. Transport Interface

- [x] 3.1 Create include/firebolt/ directory, and verify directory exists
- [x] 3.2 Implement IFireboltTransport.h pure header-only interface with send() and receive() methods and Apache 2.0 license header, and verify header compiles independently
- [x] 3.3 Add documentation in IFireboltTransport.h explaining the interface and usage pattern, and verify documentation is present

## 4. Surface Client Structure

- [x] 4.1 Create src/core directory structure (client, generated), and verify directories exist
- [x] 4.2 Create src/core/CMakeLists.txt for libfirebolt-core-client.so, and verify CMake parses without errors
- [x] 4.3 Create src/core/client directory structure (include, src, tests/unit), and verify directory layout matches design
- [x] 4.4 Create src/core/client/CMakeLists.txt, and verify CMake parses without errors
- [x] 4.5 Add placeholder CoreClient.h in src/core/client/include/firebolt/core/ with Apache 2.0 license header, and verify header compiles
- [x] 4.6 Add placeholder CoreClient.cpp in src/core/client/src/ with Apache 2.0 license header, and verify it compiles
- [x] 4.7 Add placeholder unit test in src/core/client/tests/unit/, and verify test structure exists
- [x] 4.8 Create src/core/generated directory structure (include/firebolt/core/api), and verify directory exists
- [x] 4.9 Add README in src/core/generated/ explaining future auto-generated headers, and verify README exists

- [x] 4.10 Create src/crypto directory structure (client, generated), and verify directories exist
- [x] 4.11 Create src/crypto/CMakeLists.txt for libfirebolt-crypto-client.so, and verify CMake parses without errors
- [x] 4.12 Create src/crypto/client directory structure (include, src, tests/unit), and verify directory layout matches design
- [x] 4.13 Create src/crypto/client/CMakeLists.txt, and verify CMake parses without errors
- [x] 4.14 Add placeholder CryptoClient.h in src/crypto/client/include/firebolt/crypto/ with Apache 2.0 license header, and verify header compiles
- [x] 4.15 Add placeholder CryptoClient.cpp in src/crypto/client/src/ with Apache 2.0 license header, and verify it compiles
- [x] 4.16 Add placeholder unit test in src/crypto/client/tests/unit/, and verify test structure exists
- [x] 4.17 Create src/crypto/generated directory structure (include/firebolt/crypto/api), and verify directory exists
- [x] 4.18 Add README in src/crypto/generated/ explaining future auto-generated headers, and verify README exists

- [x] 4.19 Create src/media directory structure (client, generated), and verify directories exist
- [x] 4.20 Create src/media/CMakeLists.txt for libfirebolt-media-client.so, and verify CMake parses without errors
- [x] 4.21 Create src/media/client directory structure (include, src, tests/unit), and verify directory layout matches design
- [x] 4.22 Create src/media/client/CMakeLists.txt, and verify CMake parses without errors
- [x] 4.23 Add placeholder MediaClient.h in src/media/client/include/firebolt/media/ with Apache 2.0 license header, and verify header compiles
- [x] 4.24 Add placeholder MediaClient.cpp in src/media/client/src/ with Apache 2.0 license header, and verify it compiles
- [x] 4.25 Add placeholder unit test in src/media/client/tests/unit/, and verify test structure exists
- [x] 4.26 Create src/media/generated directory structure (include/firebolt/media/api), and verify directory exists
- [x] 4.27 Add README in src/media/generated/ explaining future auto-generated headers, and verify README exists

## 5. Example Transport Implementation

- [x] 5.1 Create transports/ directory, and verify directory exists
- [x] 5.2 Create transports/example directory structure (include, src, tests/unit), and verify directory layout matches design
- [x] 5.3 Create transports/example/CMakeLists.txt for libfirebolt-example-transport.so, and verify CMake parses without errors
- [x] 5.4 Implement ExampleTransport.h inheriting from IFireboltTransport with simple in-memory queue and Apache 2.0 license header, and verify header compiles
- [x] 5.5 Implement ExampleTransport.cpp with send() and receive() methods using in-memory queue and Apache 2.0 license header, and verify it compiles
- [x] 5.6 Add unit test for ExampleTransport in transports/example/tests/unit/, and verify test passes
- [x] 5.7 Verify ExampleTransport.so builds successfully and is linkable

## 6. Integration Test Structure

- [x] 6.1 Create tests/integration directory, and verify directory exists
- [x] 6.2 Create tests/integration/CMakeLists.txt, and verify CMake parses without errors
- [x] 6.3 Add placeholder integration test for CoreClient + ExampleTransport, and verify test structure exists
- [x] 6.4 Add placeholder integration test for CryptoClient + ExampleTransport, and verify test structure exists
- [x] 6.5 Add placeholder integration test for MediaClient + ExampleTransport, and verify test structure exists

## 7. Build Verification

- [x] 7.1 Verify out-of-tree build succeeds with `cmake -S . -B /tmp/firebolt-build && cmake --build /tmp/firebolt-build`
- [x] 7.2 Verify all .so files are generated (libfirebolt-core-client.so, libfirebolt-crypto-client.so, libfirebolt-media-client.so, libfirebolt-example-transport.so)
- [x] 7.3 Verify IFireboltTransport.h is exported from each client .so
- [x] 7.4 Verify clean build with `cmake --build /tmp/firebolt-build --clean-first` succeeds
- [x] 7.5 Verify CMake can find and configure the project from a different directory

## 8. Documentation

- [x] 8.1 Create README.md in root explaining project structure and build instructions, and verify README exists
- [x] 8.2 Document how to add a new surface in README.md, and verify documentation is present
- [x] 8.3 Document how to implement a custom transport in README.md, and verify documentation is present
- [x] 8.4 Add build example in README.md showing transport injection pattern, and verify example compiles

## 9. License Header Verification

- [x] 9.1 Verify all .h files include the Apache 2.0 license header at the top, and verify header matches the standard format
- [x] 9.2 Verify all .cpp files include the Apache 2.0 license header at the top, and verify header matches the standard format
- [x] 9.3 Add a note in README.md about the license header requirement for new source files, and verify documentation is present


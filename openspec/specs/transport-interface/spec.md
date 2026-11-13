# Transport Interface Specification

## Purpose

Defines the dual-transport interface architecture that enables protocol-specific transport implementations with clear separation of concerns and type safety.

## Requirements

### Requirement: Text transport interface for text-based protocols
The system SHALL provide a text-based transport interface for protocols requiring text frames (e.g., WebSocket with JSON-RPC).

#### Scenario: Text transport sends and receives strings
- **WHEN** a client sends a message through a text transport
- **THEN** the message is transmitted as a string
- **AND** received messages are delivered as strings to the client

### Requirement: Buffer transport interface for binary-based protocols
The system SHALL provide a buffer-based transport interface for protocols requiring binary data (e.g., Protobuf over UDS).

#### Scenario: Buffer transport sends and receives byte buffers
- **WHEN** a client sends a message through a buffer transport
- **THEN** the message is transmitted as a byte buffer with size
- **AND** received messages are delivered as byte buffers to the client

### Requirement: Transport connection lifecycle management
All transport interfaces SHALL support connection lifecycle operations including open, disconnect, and connection status monitoring.

#### Scenario: Transport opens connection
- **WHEN** a client calls open() on a transport
- **THEN** the transport establishes a connection using pre-configured settings

#### Scenario: Transport reports connection status
- **WHEN** a transport's connection status changes
- **THEN** registered callbacks receive the new status and error information

#### Scenario: Transport disconnects
- **WHEN** a client calls disconnect() on a transport
- **THEN** the transport terminates the connection and releases resources

### Requirement: Transport configuration is implementation-specific
Transport implementations SHALL handle their own configuration (URL, authentication, etc.) without exposing it through the interface.

#### Scenario: Transport configured at construction
- **WHEN** a transport is constructed
- **THEN** it reads implementation-specific configuration from its environment
- **AND** the open() method uses this pre-configured state

### Requirement: Client owns transport connection lifecycle
Clients SHALL decide when to establish and terminate transport connections, not the transport itself.

#### Scenario: Client connects on first use
- **WHEN** a client performs its first operation
- **THEN** the client opens the transport connection if not already connected

### Requirement: Transport interfaces are pure virtual
Transport interfaces SHALL be pure virtual header-only interfaces without implementation details.

#### Scenario: Interface is header-only
- **WHEN** a transport interface header is included
- **THEN** it contains only pure virtual methods
- **AND** no implementation code is present in the header

### Requirement: All transport headers include license documentation
All transport interface header files SHALL include the standard Apache 2.0 license header at the top of the file with the following exact structure:
```cpp
/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2026 RDK Management.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
```

#### Scenario: License header present
- **WHEN** a transport interface header file is created
- **THEN** it includes the complete Apache 2.0 license header
- **AND** the header matches the exact structure specified above

### Requirement: All transport methods are documented
All methods in transport interfaces SHALL have comprehensive documentation explaining their purpose, parameters, return values, and behavior.

#### Scenario: Method documentation complete
- **WHEN** a method is defined in a transport interface
- **THEN** it has documentation describing its purpose
- **AND** all parameters are documented
- **AND** return values and error conditions are documented

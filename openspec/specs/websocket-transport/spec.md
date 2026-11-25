# WebSocket Transport Specification

## Purpose

Provides a production-ready WebSocket transport implementation for text-based communication between containerized applications and system host using WebSocket++ and Asio.

## Requirements

### Requirement: WebSocket transport configuration via environment variable
The transport SHALL read the WebSocket URL from the FIREBOLT_ENDPOINT environment variable during construction.

#### Scenario: Transport reads endpoint from environment
- **WHEN** a WebSocketTransport is constructed
- **THEN** it reads the FIREBOLT_ENDPOINT environment variable
- **AND** the value is used as the WebSocket connection URL

#### Scenario: Missing environment variable causes error
- **WHEN** FIREBOLT_ENDPOINT is not set during construction
- **THEN** the transport reports a configuration error
- **AND** open() fails with an appropriate error message

### Requirement: WebSocket connection establishment
The transport SHALL establish a WebSocket connection using WebSocket++ when open() is called.

#### Scenario: Successful WebSocket connection
- **WHEN** open() is called with a valid FIREBOLT_ENDPOINT
- **THEN** the transport connects to the WebSocket server
- **AND** isConnected() returns true
- **AND** the connection status callback is invoked with connected=true

#### Scenario: Connection failure reports error
- **WHEN** open() fails to connect to the WebSocket server
- **THEN** open() returns false
- **AND** isConnected() returns false
- **AND** the connection status callback is invoked with connected=false and error information

### Requirement: Text message sending over WebSocket
The transport SHALL send text messages through the WebSocket connection using WebSocket++.

#### Scenario: Send text message when connected
- **WHEN** send() is called with a text message while connected
- **THEN** the message is sent as a WebSocket text frame
- **AND** send() returns true

#### Scenario: Send fails when disconnected
- **WHEN** send() is called while the transport is not connected
- **THEN** send() returns false
- **AND** no message is transmitted

### Requirement: Text message receiving from WebSocket
The transport SHALL deliver received WebSocket text frames to registered callbacks.

#### Scenario: Receive text message
- **WHEN** a text frame is received from the WebSocket server
- **THEN** the message callback is invoked with the received text
- **AND** the message content is delivered as a string

#### Scenario: Callback registration before connection
- **WHEN** onMessageReceived() is called before the connection is established
- **THEN** the callback is stored
- **AND** the callback is invoked when messages arrive after connection

### Requirement: WebSocket connection status monitoring
The transport SHALL notify registered callbacks when the WebSocket connection status changes.

#### Scenario: Connection established callback
- **WHEN** the WebSocket connection is successfully established
- **THEN** the connection status callback is invoked with connected=true
- **AND** the error information string is empty

#### Scenario: Connection lost callback
- **WHEN** the WebSocket connection is lost or closed
- **THEN** the connection status callback is invoked with connected=false
- **AND** the error information string describes the reason

### Requirement: WebSocket connection termination
The transport SHALL close the WebSocket connection and release resources when disconnect() is called.

#### Scenario: Graceful disconnect
- **WHEN** disconnect() is called
- **THEN** the WebSocket connection is closed
- **AND** isConnected() returns false
- **AND** the connection status callback is invoked with connected=false

#### Scenario: Disconnect when not connected
- **WHEN** disconnect() is called while not connected
- **THEN** the operation completes without error
- **AND** no connection status callback is invoked

### Requirement: Connection state tracking
The transport SHALL accurately report its connection state through isConnected().

#### Scenario: IsConnected returns true when connected
- **WHEN** the WebSocket connection is established
- **THEN** isConnected() returns true

#### Scenario: IsConnected returns false when disconnected
- **WHEN** the WebSocket connection is not established or closed
- **THEN** isConnected() returns false

### Requirement: Thread-safe callback registration
The transport SHALL handle callback registration in a thread-safe manner.

#### Scenario: Callback registration from any thread
- **WHEN** onMessageReceived() or onConnectionStatusChanged() is called
- **THEN** the callback is stored safely regardless of calling thread
- **AND** subsequent callback invocations are protected from race conditions

### Requirement: Optional WebSocket sub-protocol
The transport SHALL support an optional WebSocket sub-protocol for handshake negotiation.

#### Scenario: Connect with sub-protocol
- **WHEN** a sub-protocol is specified during construction
- **THEN** the WebSocket handshake includes the sub-protocol
- **AND** the server can validate the protocol

#### Scenario: Connect without sub-protocol
- **WHEN** no sub-protocol is specified
- **THEN** the WebSocket handshake proceeds without a protocol header

### Requirement: License header in transport header file
The WebSocketTransport header file SHALL include the standard Apache 2.0 license header.

#### Scenario: License header present
- **WHEN** the WebSocketTransport.h file is created
- **THEN** it includes the complete Apache 2.0 license header at the top
- **AND** the header matches the exact structure specified in the transport-interface spec

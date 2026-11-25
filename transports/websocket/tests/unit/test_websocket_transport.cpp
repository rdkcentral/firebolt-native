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

#include "firebolt/transport/WebSocketTransport.h"

#include <cassert>
#include <iostream>
#include <thread>

int main() {
    using namespace firebolt::transport;

    std::cout << "WebSocket Transport Unit Tests" << std::endl;

    // Test 1: Create transport
    WebSocketTransport transport;
    std::cout << "Test 1: Transport created successfully" << std::endl;

    // Test 2: Check FIREBOLT_ENDPOINT reading
    const char* testEndpoint = "ws://localhost:8765/test";
    setenv("FIREBOLT_ENDPOINT", testEndpoint, 1);
    WebSocketTransport transportWithEndpoint;
    // Note: We can't directly access m_endpoint, but we can verify it doesn't crash
    std::cout << "Test 2: FIREBOLT_ENDPOINT can be set" << std::endl;

    // Test 3: Missing environment variable
    unsetenv("FIREBOLT_ENDPOINT");
    WebSocketTransport transportNoEndpoint;
    // open() should fail without endpoint
    bool opened = transportNoEndpoint.open();
    assert(opened == false);
    std::cout << "Test 3: open() fails when FIREBOLT_ENDPOINT is not set" << std::endl;

    // Test 4: Connection status callback registration
    bool connectionCallbackCalled = false;
    transportWithEndpoint.onConnectionStatusChanged([&](bool connected, const std::string& errorInfo) {
        connectionCallbackCalled = true;
    });
    assert(connectionCallbackCalled == false);  // Callback stored but not called yet
    std::cout << "Test 4: Connection status callback registered successfully" << std::endl;

    // Test 5: Message callback registration
    bool messageCallbackCalled = false;
    transportWithEndpoint.onMessageReceived([&](const std::string& message) {
        messageCallbackCalled = true;
    });
    assert(messageCallbackCalled == false);  // Callback stored but not called yet
    std::cout << "Test 5: Message callback registered successfully" << std::endl;

    // Test 6: Disconnect when not connected
    transportWithEndpoint.disconnect();
    std::cout << "Test 6: Disconnect when not connected completes without error" << std::endl;

    // Test 7: isConnected returns false by default
    assert(transportWithEndpoint.isConnected() == false);
    std::cout << "Test 7: isConnected() returns false when not connected" << std::endl;

    // Test 8: Send fails when disconnected
    bool sent = transportWithEndpoint.send("test message");
    assert(sent == false);
    std::cout << "Test 8: send() fails when not connected" << std::endl;

    // Note: We cannot test actual connection without a real WebSocket server
    // Those tests are better suited for integration tests

    std::cout << "All unit tests passed!" << std::endl;
    return 0;
}

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

#include "firebolt/transport/ExampleTransport.h"

#include <cassert>
#include <iostream>

int main() {
    using namespace firebolt::transport;

    // Test 1: Create transport
    ExampleTransport transport;
    std::cout << "Test 1: Transport created successfully" << std::endl;

    // Test 2: Open connection
    bool connectionCallbackCalled = false;
    transport.onConnectionStatusChanged([&](bool connected, const std::string& errorInfo) {
        connectionCallbackCalled = true;
        assert(connected == true);
        assert(errorInfo.empty());
    });
    bool opened = transport.open();
    assert(opened == true);
    assert(transport.isConnected() == true);
    assert(connectionCallbackCalled == true);
    std::cout << "Test 2: Connection opened successfully" << std::endl;

    // Test 3: Send message
    bool messageCallbackCalled = false;
    std::string receivedMessage;
    transport.onMessageReceived([&](const std::string& message) {
        messageCallbackCalled = true;
        receivedMessage = message;
    });
    bool sent = transport.send("test message");
    assert(sent == true);
    assert(messageCallbackCalled == true);
    assert(receivedMessage == "test message");
    std::cout << "Test 3: Message sent and received successfully" << std::endl;

    // Test 4: Disconnect
    bool disconnectCallbackCalled = false;
    transport.onConnectionStatusChanged([&](bool connected, const std::string& errorInfo) {
        disconnectCallbackCalled = true;
        assert(connected == false);
    });
    transport.disconnect();
    assert(transport.isConnected() == false);
    assert(disconnectCallbackCalled == true);
    std::cout << "Test 4: Disconnected successfully" << std::endl;

    std::cout << "All tests passed!" << std::endl;
    return 0;
}

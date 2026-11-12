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

namespace firebolt {
namespace transport {

bool ExampleTransport::open() {
    m_connected = true;
    if (m_connectionCallback) {
        m_connectionCallback(true, "");
    }
    return true;
}

bool ExampleTransport::isConnected() {
    return m_connected;
}

void ExampleTransport::onConnectionStatusChanged(
    std::function<void(bool connected, const std::string& errorInfo)> callback) {
    m_connectionCallback = std::move(callback);
}

bool ExampleTransport::send(const std::string& message) {
    if (!m_connected) {
        return false;
    }
    m_messageQueue.push(message);
    
    // Simulate receiving a message by calling the callback
    if (m_messageCallback && !m_messageQueue.empty()) {
        std::string msg = m_messageQueue.front();
        m_messageCallback(msg);
    }
    
    return true;
}

void ExampleTransport::onMessageReceived(
    std::function<void(const std::string& message)> callback) {
    m_messageCallback = std::move(callback);
}

void ExampleTransport::disconnect() {
    m_connected = false;
    if (m_connectionCallback) {
        m_connectionCallback(false, "");
    }
    while (!m_messageQueue.empty()) {
        m_messageQueue.pop();
    }
}

} // namespace transport
} // namespace firebolt

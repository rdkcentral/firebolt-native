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

#include "firebolt/transport/SecureTransport.h"

#include <cstring>

namespace firebolt {
namespace transport {

// Static registry for C callbacks
std::unordered_map<Client*, SecureTransport*> SecureTransport::s_instanceRegistry;
std::mutex SecureTransport::s_registryMutex;

SecureTransport::SecureTransport()
    : m_client(nullptr)
    , m_connected(false)
{
}

SecureTransport::~SecureTransport() {
    disconnect();

    // Remove from registry if still present
    std::lock_guard<std::mutex> lock(s_registryMutex);
    if (m_client) {
        s_instanceRegistry.erase(m_client);
    }
}

bool SecureTransport::open() {
    if (m_connected.load()) {
        return true;
    }

    if (m_client) {
        client_unregister_notifications(m_client);
        client_disconnect(m_client);
        {
            std::lock_guard<std::mutex> lock(s_registryMutex);
            s_instanceRegistry.erase(m_client);
        }
        m_client = nullptr;
        m_connected = false;
    }

    ClientResult result = client_connect(&m_client, connectionEventHandler);
    if (result != CLIENT_OK) {
        if (m_connectionCallback) {
            std::string errorInfo = "Failed to connect: error code " + std::to_string(result);
            m_connectionCallback(false, errorInfo);
        }
        return false;
    }

    // Register this instance before enabling notifications
    {
        std::lock_guard<std::mutex> lock(s_registryMutex);
        s_instanceRegistry[m_client] = this;
    }

    if (m_messageCallback) {
        result = client_register_notifications(m_client, notificationHandler);
        if (result != CLIENT_OK) {
            client_unregister_notifications(m_client);
            client_disconnect(m_client);
            {
                std::lock_guard<std::mutex> lock(s_registryMutex);
                s_instanceRegistry.erase(m_client);
            }
            m_client = nullptr;
            m_connected = false;
            if (m_connectionCallback) {
                std::string errorInfo = "Failed to register notifications: error code " + std::to_string(result);
                m_connectionCallback(false, errorInfo);
            }
            return false;
        }
    }

    m_connected.store(true);
    if (m_connectionCallback) {
        m_connectionCallback(true, "");
    }
    return true;
}

bool SecureTransport::isConnected() {
    return m_connected.load();
}

void SecureTransport::onConnectionStatusChanged(
    std::function<void(bool connected, const std::string& errorInfo)> callback) {
    m_connectionCallback = std::move(callback);
}

bool SecureTransport::send(const std::vector<uint8_t>& data) {
    if (!m_connected.load() || !m_client) {
        return false;
    }

    uint8_t* response_data = nullptr;
    size_t response_size = 0;

    ClientResult result = client_send_request(
        m_client,
        data.data(),
        data.size(),
        &response_data,
        &response_size
    );

    if (result != CLIENT_OK) {
        return false;
    }

    // If there's a response, deliver it through the message callback
    if (response_data && response_size > 0 && m_messageCallback) {
        std::vector<uint8_t> response(response_data, response_data + response_size);
        m_messageCallback(response);
    }

    // Free the response data
    if (response_data) {
        client_free_response(response_data);
    }

    return true;
}

void SecureTransport::onMessageReceived(
    std::function<void(const std::vector<uint8_t>& data)> callback) {
    m_messageCallback = std::move(callback);

    // Register the notification handler with the C API
    if (m_client && m_connected.load()) {
        client_register_notifications(m_client, notificationHandler);
    }
}

void SecureTransport::disconnect() {
    if (!m_connected.load() || !m_client) {
        return;
    }

    // Unregister notifications
    client_unregister_notifications(m_client);

    // Disconnect
    client_disconnect(m_client);

    // Remove from registry
    {
        std::lock_guard<std::mutex> lock(s_registryMutex);
        s_instanceRegistry.erase(m_client);
    }

    m_client = nullptr;
    m_connected.store(false);

    if (m_connectionCallback) {
        m_connectionCallback(false, "");
    }
}

// Static callback for connection events from C API
void SecureTransport::connectionEventHandler(Client* client, ClientConnectionEvent event) {
    std::lock_guard<std::mutex> lock(s_registryMutex);
    auto it = s_instanceRegistry.find(client);
    if (it == s_instanceRegistry.end()) {
        return;
    }

    SecureTransport* instance = it->second;
    bool connected = false;
    std::string errorInfo;

    switch (event) {
        case CLIENT_CONNECTION_LOST:
            connected = false;
            errorInfo = "Connection lost";
            instance->m_connected.store(false);
            break;
        case CLIENT_CONNECTION_RESTORED:
            connected = true;
            errorInfo = "";
            instance->m_connected.store(true);
            break;
        case CLIENT_CONNECTION_TERMINATED:
            connected = false;
            errorInfo = "Connection terminated due to auth failures";
            instance->m_connected.store(false);
            break;
        default:
            return;
    }

    if (instance->m_connectionCallback) {
        instance->m_connectionCallback(connected, errorInfo);
    }
}

// Static callback for notifications from C API
void SecureTransport::notificationHandler(Client* client, uint8_t* notification_data, size_t notification_size) {
    std::lock_guard<std::mutex> lock(s_registryMutex);
    auto it = s_instanceRegistry.find(client);
    if (it == s_instanceRegistry.end() || !notification_data || notification_size == 0) {
        client_free_response(notification_data);
        return;
    }

    SecureTransport* instance = it->second;

    if (instance->m_messageCallback) {
        std::vector<uint8_t> notification(notification_data, notification_data + notification_size);
        instance->m_messageCallback(notification);
    }

    // Free the notification data (ownership was transferred to us)
    client_free_response(notification_data);
}

} // namespace transport
} // namespace firebolt

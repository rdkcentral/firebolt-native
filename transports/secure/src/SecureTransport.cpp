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

// Static instance pointer for C callbacks
SecureTransport* SecureTransport::s_instance = nullptr;

SecureTransport::SecureTransport()
    : m_client(nullptr)
    , m_connected(false)
{
    s_instance = this;
}

SecureTransport::~SecureTransport() {
    disconnect();
    s_instance = nullptr;
}

bool SecureTransport::open() {
    if (m_connected) {
        return true;
    }

    ClientResult result = client_connect(&m_client, connectionEventHandler);
    if (result != CLIENT_OK) {
        if (m_connectionCallback) {
            std::string errorInfo = "Failed to connect: error code " + std::to_string(result);
            m_connectionCallback(false, errorInfo);
        }
        return false;
    }

    m_connected = true;
    if (m_connectionCallback) {
        m_connectionCallback(true, "");
    }
    return true;
}

bool SecureTransport::isConnected() {
    return m_connected;
}

void SecureTransport::onConnectionStatusChanged(
    std::function<void(bool connected, const std::string& errorInfo)> callback) {
    m_connectionCallback = std::move(callback);
}

bool SecureTransport::send(const std::vector<uint8_t>& data) {
    if (!m_connected || !m_client) {
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
    if (m_client && m_connected) {
        client_register_notifications(m_client, notificationHandler);
    }
}

void SecureTransport::disconnect() {
    if (!m_connected || !m_client) {
        return;
    }

    // Unregister notifications
    client_unregister_notifications(m_client);

    // Disconnect
    client_disconnect(m_client);
    m_client = nullptr;
    m_connected = false;

    if (m_connectionCallback) {
        m_connectionCallback(false, "");
    }
}

// Static callback for connection events from C API
void SecureTransport::connectionEventHandler(Client* /*client*/, ClientConnectionEvent event) {
    if (!s_instance) {
        return;
    }

    bool connected = false;
    std::string errorInfo;

    switch (event) {
        case CLIENT_CONNECTION_LOST:
            connected = false;
            errorInfo = "Connection lost";
            s_instance->m_connected = false;
            break;
        case CLIENT_CONNECTION_RESTORED:
            connected = true;
            errorInfo = "";
            s_instance->m_connected = true;
            break;
        case CLIENT_CONNECTION_TERMINATED:
            connected = false;
            errorInfo = "Connection terminated due to auth failures";
            s_instance->m_connected = false;
            break;
        default:
            return;
    }

    if (s_instance->m_connectionCallback) {
        s_instance->m_connectionCallback(connected, errorInfo);
    }
}

// Static callback for notifications from C API
void SecureTransport::notificationHandler(Client* /*client*/, uint8_t* notification_data, size_t notification_size) {
    if (!s_instance || !notification_data || notification_size == 0) {
        return;
    }

    if (s_instance->m_messageCallback) {
        std::vector<uint8_t> notification(notification_data, notification_data + notification_size);
        s_instance->m_messageCallback(notification);
    }

    // Free the notification data (ownership was transferred to us)
    client_free_response(notification_data);
}

} // namespace transport
} // namespace firebolt

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

#pragma once

#include "EyilClient.h"
#include "firebolt/IFireboltBufferTransport.h"

#include <functional>
#include <memory>
#include <vector>
#include <cstdint>
#include <atomic>
#include <unordered_map>
#include <mutex>

namespace firebolt {
namespace transport {

/**
 * @brief Secure transport implementation wrapping SecureGateway C API
 *
 * This transport wraps the SecureGateway (EyilClient) C API to provide a
 * C++ interface for binary buffer-based communication over Unix Domain Socket.
 * It implements IFireboltBufferTransport for use with Crypto and Media clients.
 *
 * Configuration (UDS path, authentication, etc.) is handled by the underlying
 * SecureGateway library and is not exposed through this interface.
 */
class SecureTransport : public IFireboltBufferTransport {
public:
    SecureTransport();
    ~SecureTransport() override;

    // Disable copy and move
    SecureTransport(const SecureTransport&) = delete;
    SecureTransport& operator=(const SecureTransport&) = delete;
    SecureTransport(SecureTransport&&) = delete;
    SecureTransport& operator=(SecureTransport&&) = delete;

    bool open() override;
    bool isConnected() override;
    void onConnectionStatusChanged(
        std::function<void(bool connected, const std::string& errorInfo)> callback) override;
    bool send(const std::vector<uint8_t>& data) override;
    void onMessageReceived(
        std::function<void(const std::vector<uint8_t>& data)> callback) override;
    void disconnect() override;

private:
    // Connection event handler callback for C API
    static void connectionEventHandler(Client* client, ClientConnectionEvent event);

    // Notification handler callback for C API
    static void notificationHandler(Client* client, uint8_t* notification_data, size_t notification_size);

    // Registry mapping Client* to SecureTransport instances for C callbacks
    static std::unordered_map<Client*, SecureTransport*> s_instanceRegistry;
    static std::mutex s_registryMutex;

    // C API client handle
    Client* m_client;

    // Connection state (atomic for thread safety)
    std::atomic<bool> m_connected;

    // Mutex protecting callback assignment and snapshotting
    mutable std::mutex m_callbackMutex;

    // C++ callbacks
    std::function<void(bool, const std::string&)> m_connectionCallback;
    std::function<void(const std::vector<uint8_t>&)> m_messageCallback;
};

} // namespace transport
} // namespace firebolt

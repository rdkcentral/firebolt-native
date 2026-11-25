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

// Use standalone Asio (must be defined before any websocketpp or asio includes)
#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif

#ifndef WEBSOCKETPP_STANDALONE_ASIO
#define WEBSOCKETPP_STANDALONE_ASIO
#endif

#pragma once

#include "firebolt/IFireboltTextTransport.h"

#include <functional>
#include <string>
#include <atomic>
#include <mutex>
#include <memory>
#include <thread>
#include <condition_variable>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <asio.hpp>

namespace websocketpp {
namespace lib {
template<typename config>
class client;
}
}

namespace firebolt {
namespace transport {

/**
 * @brief WebSocket transport implementation using WebSocket++ and Asio
 *
 * This transport provides text-based communication over WebSocket protocol
 * using WebSocket++ library with Asio for networking. It is configured via the
 * FIREBOLT_ENDPOINT environment variable and implements the IFireboltTextTransport
 * interface for use with FireboltCoreClient.
 *
 * Configuration:
 * - The WebSocket URL is read from the FIREBOLT_ENDPOINT environment variable
 * - Supports apps launched before network is connected (Asio retry logic)
 * - No TLS support (ws:// only, suitable for container-to-systemhost communication)
 *
 * Thread safety:
 * - Callback registration is protected by mutex
 * - Connection state uses atomic boolean
 * - Callbacks run on Asio's io_service thread and must not call transport APIs
 */
class WebSocketTransport : public IFireboltTextTransport {
public:
    /**
     * @brief Constructs a WebSocketTransport
     *
     * Reads the FIREBOLT_ENDPOINT environment variable for the WebSocket URL.
     * If the environment variable is not set, the transport will report a
     * configuration error when open() is called.
     */
    WebSocketTransport();

    /**
     * @brief Destroys the WebSocketTransport
     *
     * Disconnects and cleans up WebSocket++ and Asio resources if connected.
     */
    ~WebSocketTransport() override;

    // Disable copy and move
    WebSocketTransport(const WebSocketTransport&) = delete;
    WebSocketTransport& operator=(const WebSocketTransport&) = delete;
    WebSocketTransport(WebSocketTransport&&) = delete;
    WebSocketTransport& operator=(WebSocketTransport&&) = delete;

    /**
     * @brief Opens the WebSocket connection
     *
     * Establishes a WebSocket connection to the endpoint specified in
     * FIREBOLT_ENDPOINT environment variable using WebSocket++ and Asio.
     * This blocks until the connection is established or fails.
     *
     * @return true if the connection was successfully opened
     * @return false if the connection failed to open or FIREBOLT_ENDPOINT is not set
     */
    bool open() override;

    /**
     * @brief Checks if the WebSocket is currently connected
     *
     * @return true if the WebSocket has an active connection
     * @return false if the WebSocket is disconnected
     */
    bool isConnected() override;

    /**
     * @brief Registers a callback for connection status changes
     *
     * The callback is invoked when the WebSocket connection status changes.
     * It runs on Asio's io_service thread and must not call other transport APIs.
     *
     * @param callback Function to call with (connected, errorInfo) parameters:
     *                 - connected: true if connected, false if disconnected
     *                 - errorInfo: error description if disconnected, empty string if connected
     */
    void onConnectionStatusChanged(
        std::function<void(bool connected, const std::string& errorInfo)> callback) override;

    /**
     * @brief Sends a text message through the WebSocket
     *
     * Sends the message as a WebSocket text frame through WebSocket++.
     *
     * @param message The text message to send
     * @return true if the message was sent successfully
     * @return false if the message could not be sent (e.g., not connected)
     */
    bool send(const std::string& message) override;

    /**
     * @brief Registers a callback for received messages
     *
     * The callback is invoked when a text frame is received from the WebSocket.
     * It runs on Asio's io_service thread and must not call other transport APIs.
     *
     * @param callback Function to call with the received message as a string
     */
    void onMessageReceived(
        std::function<void(const std::string& message)> callback) override;

    /**
     * @brief Disconnects and closes the WebSocket connection
     *
     * Terminates the WebSocket connection and releases WebSocket++ and Asio resources.
     * The connection event handler is automatically unregistered during disconnect.
     */
    void disconnect() override;

private:
    // WebSocket++ client type
    typedef websocketpp::client<websocketpp::config::asio_client> client_t;

    // Asio io_context for networking
    std::shared_ptr<asio::io_context> m_io_service;
    std::unique_ptr<asio::io_context::work> m_work;

    // Thread for io_service
    std::thread m_io_thread;

    // WebSocket++ client
    std::unique_ptr<client_t> m_client;
    client_t::connection_ptr m_connection;

    // Configuration
    std::string m_endpoint;
    std::string m_lastError;

    // Connection state (atomic for thread safety)
    std::atomic<bool> m_connected;

    // Mutex protecting callback assignment
    mutable std::mutex m_callbackMutex;

    // Mutex and condition variable for connection synchronization
    mutable std::mutex m_connectionMutex;
    std::condition_variable m_connectionCV;
    bool m_connectionCompleted = false;
    bool m_connectionResult = false;
    std::string m_connectionError;

    // Callbacks
    std::function<void(bool, const std::string&)> m_connectionCallback;
    std::function<void(const std::string&)> m_messageCallback;
};

} // namespace transport
} // namespace firebolt

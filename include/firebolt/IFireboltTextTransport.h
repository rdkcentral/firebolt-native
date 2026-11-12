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

#include <functional>
#include <string>

namespace firebolt {

/**
 * @brief Pure header-only interface for text-based transport implementations
 *
 * This interface defines the contract for transport implementations that handle
 * text-based protocols (e.g., WebSocket with text frames, JSON-RPC). Transport
 * implementations handle the actual communication protocol and are injected into
 * Firebolt clients at runtime.
 *
 * Usage example:
 *   auto transport = std::make_shared<WebSocketTransport>();
 *   CoreClient client(transport);
 *   client.doSomething();
 *
 * Partners can implement custom transports by inheriting from this interface.
 * Configuration (URL, authentication, etc.) is implementation-specific and
 * handled by the transport itself, not exposed through this interface.
 */
class IFireboltTextTransport {
public:
    virtual ~IFireboltTextTransport() = default;

    /**
     * @brief Opens the transport connection
     *
     * Establishes the connection using pre-configured settings (URL, authentication,
     * etc.) that were provided during transport construction. The transport must be
     * configured before calling this method.
     *
     * @return true if the connection was successfully opened
     * @return false if the connection failed to open
     */
    virtual bool open() = 0;

    /**
     * @brief Checks if the transport is currently connected
     *
     * @return true if the transport has an active connection
     * @return false if the transport is disconnected
     */
    virtual bool isConnected() = 0;

    /**
     * @brief Registers a callback for connection status changes
     *
     * The callback is invoked when the transport's connection status changes.
     * It runs on an internal thread and must not call other transport APIs.
     *
     * @param callback Function to call with (connected, errorInfo) parameters:
     *                 - connected: true if connected, false if disconnected
     *                 - errorInfo: error description if disconnected, empty string if connected
     */
    virtual void onConnectionStatusChanged(
        std::function<void(bool connected, const std::string& errorInfo)> callback) = 0;

    /**
     * @brief Sends a text message through the transport
     *
     * Sends the message as a text string through the transport. The message
     * format depends on the protocol (e.g., JSON-RPC string for WebSocket).
     *
     * @param message The text message to send
     * @return true if the message was sent successfully
     * @return false if the message could not be sent (e.g., not connected)
     */
    virtual bool send(const std::string& message) = 0;

    /**
     * @brief Registers a callback for received messages
     *
     * The callback is invoked when a message is received from the transport.
     * It runs on an internal thread and must not call other transport APIs.
     *
     * @param callback Function to call with the received message as a string
     */
    virtual void onMessageReceived(
        std::function<void(const std::string& message)> callback) = 0;

    /**
     * @brief Disconnects and closes the transport connection
     *
     * Terminates the connection and releases any resources associated with it.
     * The connection event handler is automatically unregistered during disconnect.
     */
    virtual void disconnect() = 0;
};

} // namespace firebolt

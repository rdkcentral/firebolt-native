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

#include <memory>
#include <string>

namespace firebolt {

/**
 * @brief Pure header-only interface for Firebolt transport implementations
 *
 * This interface defines the contract for transport implementations that can be
 * injected into Firebolt clients at runtime. Transport implementations handle the
 * actual communication protocol (e.g., WebSocket, Unix Domain Socket, etc.).
 *
 * Usage example:
 *   auto transport = std::make_shared<ExampleTransport>();
 *   CoreClient client(transport);
 *   client.doSomething();
 *
 * Partners can implement custom transports by inheriting from this interface.
 */
class IFireboltTransport {
public:
    virtual ~IFireboltTransport() = default;

    /**
     * @brief Send a message through the transport
     * @param message The message to send
     */
    virtual void send(const std::string& message) = 0;

    /**
     * @brief Receive a message from the transport
     * @return The received message
     */
    virtual std::string receive() = 0;
};

} // namespace firebolt

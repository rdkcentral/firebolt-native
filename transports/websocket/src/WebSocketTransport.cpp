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

// Define ASIO_STANDALONE before including asio.hpp (required for standalone Asio)
#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif

#include "firebolt/transport/WebSocketTransport.h"

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <asio.hpp>
#include <cstdlib>
#include <iostream>

namespace firebolt {
namespace transport {

typedef websocketpp::client<websocketpp::config::asio_client> client_t;

WebSocketTransport::WebSocketTransport()
    : m_io_service(std::make_shared<asio::io_context>())
    , m_client(std::make_unique<client_t>())
    , m_connected(false) {
    // Initialize WebSocket++ with Asio
    m_client->init_asio(m_io_service.get());

    // Read FIREBOLT_ENDPOINT from environment variable
    const char* endpoint = std::getenv("FIREBOLT_ENDPOINT");
    if (endpoint != nullptr) {
        m_endpoint = endpoint;
    } else {
        m_lastError = "FIREBOLT_ENDPOINT environment variable not set";
    }
}

WebSocketTransport::~WebSocketTransport() {
    disconnect();
}

bool WebSocketTransport::open() {
    if (m_endpoint.empty()) {
        m_lastError = "FIREBOLT_ENDPOINT environment variable not set";
        return false;
    }

    try {
        // Set up handlers
        m_client->set_open_handler([this](websocketpp::connection_hdl hdl) {
            m_connected = true;
            std::lock_guard<std::mutex> lock(m_callbackMutex);
            if (m_connectionCallback) {
                m_connectionCallback(true, "");
            }
        });

        m_client->set_close_handler([this](websocketpp::connection_hdl hdl) {
            m_connected = false;
            std::lock_guard<std::mutex> lock(m_callbackMutex);
            if (m_connectionCallback) {
                m_connectionCallback(false, "Connection closed");
            }
        });

        m_client->set_fail_handler([this](websocketpp::connection_hdl hdl) {
            m_connected = false;
            std::lock_guard<std::mutex> lock(m_callbackMutex);
            if (m_connectionCallback) {
                m_connectionCallback(false, m_lastError);
            }
        });

        m_client->set_message_handler([this](websocketpp::connection_hdl hdl, client_t::message_ptr msg) {
            if (msg->get_opcode() == websocketpp::frame::opcode::text) {
                std::string payload = msg->get_payload();
                std::lock_guard<std::mutex> lock(m_callbackMutex);
                if (m_messageCallback) {
                    m_messageCallback(payload);
                }
            }
        });

        // Create connection
        websocketpp::lib::error_code ec;
        client_t::connection_ptr con = m_client->get_connection(m_endpoint, ec);

        if (ec) {
            m_lastError = "Failed to create connection: " + ec.message();
            return false;
        }

        m_connection = con;

        // Connect (blocking)
        m_client->connect(con);

        // Start io_context in background thread
        m_work = std::make_unique<asio::io_context::work>(*m_io_service);
        m_io_thread = std::thread([this]() {
            m_io_service->run();
        });

        return true;
    } catch (const std::exception& e) {
        m_lastError = std::string("Exception during connect: ") + e.what();
        return false;
    }
}

bool WebSocketTransport::isConnected() {
    return m_connected.load();
}

void WebSocketTransport::onConnectionStatusChanged(
    std::function<void(bool connected, const std::string& errorInfo)> callback) {
    std::lock_guard<std::mutex> lock(m_callbackMutex);
    m_connectionCallback = std::move(callback);
}

bool WebSocketTransport::send(const std::string& message) {
    if (!m_connected.load() || !m_connection) {
        return false;
    }

    try {
        m_client->send(m_connection, message, websocketpp::frame::opcode::text);
        return true;
    } catch (const std::exception& e) {
        m_lastError = std::string("Send failed: ") + e.what();
        return false;
    }
}

void WebSocketTransport::onMessageReceived(
    std::function<void(const std::string& message)> callback) {
    std::lock_guard<std::mutex> lock(m_callbackMutex);
    m_messageCallback = std::move(callback);
}

void WebSocketTransport::disconnect() {
    if (m_connection) {
        try {
            m_client->close(m_connection, websocketpp::close::status::normal, "Closing");
        } catch (...) {
            // Ignore close errors during disconnect
        }
        m_connection.reset();
    }

    // Stop io_service
    if (m_work) {
        m_work.reset();
    }
    
    if (m_io_thread.joinable()) {
        m_io_thread.join();
    }

    bool wasConnected = m_connected.exchange(false);

    if (wasConnected) {
        std::lock_guard<std::mutex> lock(m_callbackMutex);
        if (m_connectionCallback) {
            m_connectionCallback(false, "");
        }
    }
}

} // namespace transport
} // namespace firebolt

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

// Use standalone Asio
#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif
#ifndef WEBSOCKETPP_STANDALONE_ASIO
#define WEBSOCKETPP_STANDALONE_ASIO
#endif

#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>
#include <iostream>
#include <csignal>
#include <thread>

typedef websocketpp::server<websocketpp::config::asio> server;

static server* g_server = nullptr;
static std::thread* g_server_thread = nullptr;

void signalHandler(int signal) {
    if (g_server) {
        g_server->stop();
    }
}

void onWebSocketConnected(websocketpp::connection_hdl hdl) {
    std::cout << "WebSocket client connected" << std::endl;
}

void onMessage(websocketpp::connection_hdl hdl, server::message_ptr msg) {
    if (msg->get_opcode() == websocketpp::frame::opcode::text) {
        std::string payload = msg->get_payload();
        std::cout << "Received message: " << payload << std::endl;

        // Echo the message back
        g_server->send(hdl, payload, websocketpp::frame::opcode::text);
    }
}

void startServer(int port) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    g_server = new server();

    g_server->set_access_channels(websocketpp::log::alevel::all);
    g_server->clear_access_channels(websocketpp::log::alevel::all);

    g_server->set_open_handler(&onWebSocketConnected);
    g_server->set_message_handler(&onMessage);

    g_server->init_asio();
    g_server->set_reuse_addr(true);
    g_server->listen(port);
    g_server->start_accept();

    std::cout << "Mock WebSocket server started on port " << port << std::endl;
    std::cout << "WebSocket endpoint: ws://localhost:" << port << "/" << std::endl;

    g_server->run();
}

void stopServer() {
    if (g_server) {
        g_server->stop();
        delete g_server;
        g_server = nullptr;
    }

    if (g_server_thread) {
        g_server_thread->join();
        delete g_server_thread;
        g_server_thread = nullptr;
    }

    std::cout << "Mock WebSocket server stopped" << std::endl;
}

int main(int argc, char** argv) {
    int port = 8765;  // Default port

    if (argc > 1) {
        port = std::atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            std::cerr << "Invalid port number: " << argv[1] << std::endl;
            return 1;
        }
    }

    startServer(port);
    stopServer();

    return 0;
}

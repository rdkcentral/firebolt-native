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

#include "firebolt/core/FireboltClient.h"
#include "firebolt/common/IOperationState.h"
#include "firebolt/common/ISubscriptionState.h"
#include "firebolt/common/Logging.h"
#include <nlohmann/json.hpp>

namespace firebolt {
namespace core {

void MessageCallbackWrapper::OnMessageReceived(const std::string& message) {
    // Safe to access m_client: if it was destroyed, m_isShutdown will prevent access to stale state
    if (m_client) {
        m_client->OnTransportMessage(message);
    }
}

FireboltClient::FireboltClient(std::shared_ptr<IFireboltTextTransport> transport)
    : m_transport(transport)
    , m_nextRequestId(1)
    , m_isShutdown(false) {
    
    // Register transport callback with lifetime-safe wrapper
    // Create a shared_ptr to a wrapper object that handles the callback safely
    auto callbackWrapper = std::make_shared<MessageCallbackWrapper>(this);
    m_transport->onMessageReceived([callbackWrapper](const std::string& message) {
        callbackWrapper->OnMessageReceived(message);
    });
}

FireboltClient::~FireboltClient() {
    shutdown();
}

void FireboltClient::shutdown() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_isShutdown = true;
    }
    
    // Disconnect the transport to stop receiving messages and unregister callbacks
    m_transport->disconnect();
}

void FireboltClient::OnTransportMessage(const std::string& message) {
    try {
        auto json = nlohmann::json::parse(message);
        
        // Check if it's a response (has id and result/error)
        if (json.contains("id")) {
            uint64_t id = json["id"];
            
            std::shared_ptr<IOperationState> operation;
            std::shared_ptr<ISubscriptionState> subscription;
            {
                std::lock_guard<std::mutex> lock(m_mutex);

                // If we're shutting down, discard the message to avoid use-after-free
                if (m_isShutdown) {
                    return;
                }

                auto reqIt = m_pendingRequests.find(id);
                if (reqIt != m_pendingRequests.end()) {
                    operation = reqIt->second;
                    m_pendingRequests.erase(reqIt);
                }
                auto subIt = m_subscriptions.find(id);
                if (subIt != m_subscriptions.end()) {
                    subscription = subIt->second;
                }
            }
            if (operation) {
                operation->Complete(message);
            }
            if (subscription) {
                subscription->DeliverEvent(message);
            }
        }
    } catch (const std::exception& e) {
        // Log error but don't crash
    }
}

std::string FireboltClient::BuildJsonRpcRequest(uint64_t requestId, const std::string& method, const std::string& paramsJson) {
    nlohmann::json request;
    request["id"] = requestId;
    request["method"] = method;
    request["params"] = nlohmann::json::parse(paramsJson);
    return request.dump();
}

} // namespace core
} // namespace firebolt

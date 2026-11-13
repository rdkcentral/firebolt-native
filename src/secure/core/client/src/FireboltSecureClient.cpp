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

#include "firebolt/secure/core/FireboltSecureClient.h"
#include "firebolt/secure/core/EnvelopeWrapper.h"
#include "firebolt/common/IOperationState.h"
#include "firebolt/common/ISubscriptionState.h"

namespace firebolt {
namespace secure {
namespace core {

FireboltSecureClient::FireboltSecureClient(std::shared_ptr<IFireboltBufferTransport> transport)
    : m_transport(transport)
    , m_nextRequestId(1)
    , m_isShutdown(false) {

    m_transport->onMessageReceived([this](const std::vector<uint8_t>& data) {
        OnTransportMessage(data);
    });
}

FireboltSecureClient::~FireboltSecureClient() {
    shutdown();
}

void FireboltSecureClient::shutdown() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_isShutdown = true;
    }
    
    // Disconnect the transport to stop receiving messages and unregister callbacks
    m_transport->disconnect();
}

void FireboltSecureClient::OnTransportMessage(const std::vector<uint8_t>& data) {
    uint64_t requestId = 0;
    std::string jsonRpc;
    if (!EnvelopeWrapper::UnwrapResponse(data, requestId, jsonRpc)) {
        return;
    }

    // Look up state under the lock but invoke callbacks after releasing it,
    // so callbacks may safely issue new requests.
    std::shared_ptr<IOperationState> operation;
    std::shared_ptr<ISubscriptionState> subscription;
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        // If we're shutting down, discard the message to avoid use-after-free
        if (m_isShutdown) {
            return;
        }

        auto reqIt = m_pendingRequests.find(requestId);
        if (reqIt != m_pendingRequests.end()) {
            operation = reqIt->second;
            m_pendingRequests.erase(reqIt);
        }

        auto subIt = m_subscriptions.find(requestId);
        if (subIt != m_subscriptions.end()) {
            subscription = subIt->second;
        }
    }

    if (operation) {
        operation->Complete(jsonRpc);
    }
    if (subscription) {
        subscription->DeliverEvent(jsonRpc);
    }
}

std::vector<uint8_t> FireboltSecureClient::BuildRequest(uint64_t requestId, const std::string& method, const std::string& paramsJson) {
    return EnvelopeWrapper::WrapRequest(requestId, method, paramsJson);
}

} // namespace core
} // namespace secure
} // namespace firebolt

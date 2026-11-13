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

#include "firebolt/IFireboltBufferTransport.h"
#include "firebolt/common/FireboltObject.h"
#include "firebolt/common/FireboltOperation.h"
#include "firebolt/common/FireboltSubscription.h"
#include "firebolt/common/FireboltModel.h"
#include "firebolt/common/IOperationState.h"
#include "firebolt/common/ISubscriptionState.h"
#include "firebolt/common/StateFactory.h"
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace firebolt {
namespace secure {
namespace core {

using common::FireboltObject;
using common::FireboltOperation;
using common::FireboltSubscription;
using common::IOperationState;
using common::ISubscriptionState;

// Forward declaration for generated interfaces
template<typename T>
using EventCallback = std::function<void(const T&)>;

/**
 * @brief FireboltSecureClient for JSON-RPC wrapped in Protobuf over a binary buffer transport
 *
 * Exposes the same Send<T>(), Subscribe<T>() and GetInterface<T>() API as the core
 * FireboltClient. JSON-RPC messages are wrapped in a Protobuf envelope before being
 * sent over IFireboltBufferTransport and unwrapped before being delivered to callers.
 */
class FireboltSecureClient {
public:
    explicit FireboltSecureClient(std::shared_ptr<IFireboltBufferTransport> transport);
    ~FireboltSecureClient();

    // Disable copy
    FireboltSecureClient(const FireboltSecureClient&) = delete;
    FireboltSecureClient& operator=(const FireboltSecureClient&) = delete;

    /**
     * @brief Unregisters the receiver and waits for in-flight callbacks to finish
     *
     * Must be called before the client is destroyed if the transport may outlive it.
     * Prevents use-after-free bugs when the transport delivers callbacks after
     * the client is destroyed.
     */
    void shutdown();

    /**
     * @brief Sends a JSON-RPC request wrapped in a Protobuf envelope
     * @tparam T Response model type
     * @param method JSON-RPC method name
     * @param params Request parameters as FireboltObject
     * @return FireboltOperation<T> for the request
     */
    template<typename T>
    FireboltOperation<T> Send(const std::string& method, const FireboltObject& params);

    /**
     * @brief Subscribes to an event delivered through the Protobuf envelope
     * @tparam T Event model type
     * @param event JSON-RPC event name
     * @param listener Callback for event notifications
     * @return FireboltSubscription for the event
     */
    template<typename T>
    FireboltSubscription Subscribe(const std::string& event, EventCallback<T> listener);

    /**
     * @brief Gets an interface implementation
     * @tparam T Interface type
     * @return Reference to the interface
     */
    template<typename T>
    T& GetInterface();

private:
    void OnTransportMessage(const std::vector<uint8_t>& data);
    std::vector<uint8_t> BuildRequest(uint64_t requestId, const std::string& method, const std::string& paramsJson);

    std::shared_ptr<IFireboltBufferTransport> m_transport;
    uint64_t m_nextRequestId;
    std::unordered_map<uint64_t, std::shared_ptr<IOperationState>> m_pendingRequests;
    std::unordered_map<uint64_t, std::shared_ptr<ISubscriptionState>> m_subscriptions;
    std::mutex m_mutex;
    bool m_isShutdown = false;
};

// Template implementations
template<typename T>
FireboltOperation<T> FireboltSecureClient::Send(const std::string& method, const FireboltObject& params) {
    uint64_t requestId;
    std::shared_ptr<IOperationState> state;

    // Allocate ID and create state under the lock to prevent ID reuse
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        requestId = m_nextRequestId++;
        state = common::StateFactory::CreateOperationState(requestId);
        m_pendingRequests[requestId] = state;
    }

    // Wrap JSON-RPC request in Protobuf envelope and send via transport (outside lock to allow concurrent operations)
    std::string paramsJson = params.toString();
    m_transport->send(BuildRequest(requestId, method, paramsJson));

    // Return wrapper - state ownership shared between map and operation handle
    return FireboltOperation<T>(state);
}

template<typename T>
FireboltSubscription FireboltSecureClient::Subscribe(const std::string& event, EventCallback<T> listener) {
    uint64_t subscriptionId;
    std::shared_ptr<ISubscriptionState> state;

    // Allocate ID and create state under the lock to prevent ID reuse
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        subscriptionId = m_nextRequestId++;
        state = common::StateFactory::CreateSubscriptionState(subscriptionId);
        m_subscriptions[subscriptionId] = state;
    }

    // Register event callback that removes subscription from map when unsubscribed
    state->OnEvent([listener](const std::string& eventJson) {
        auto parsed = common::ParseJsonRpcResult<T>(eventJson);
        if (parsed.IsSuccess()) {
            listener(parsed.Value());
        }
    });

    // Send subscription request wrapped in Protobuf envelope (outside lock to allow concurrent operations)
    m_transport->send(BuildRequest(subscriptionId, event, "{}"));

    // Return wrapper - state ownership shared between map and subscription handle
    return FireboltSubscription(state);
}

template<typename T>
T& FireboltSecureClient::GetInterface() {
    // TODO: Implement interface registry
    static T instance(*this);
    return instance;
}

} // namespace core
} // namespace secure
} // namespace firebolt

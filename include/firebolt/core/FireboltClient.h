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

#include "firebolt/IFireboltTextTransport.h"
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
#include <mutex>

namespace firebolt {
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
 * @brief FireboltClient for text-based JSON-RPC over WebSocket
 *
 * Manages JSON-RPC requests and events over IFireboltTextTransport.
 * Provides Send<T>() for method calls and Subscribe<T>() for event registration.
 */
class FireboltClient {
public:
    explicit FireboltClient(std::shared_ptr<IFireboltTextTransport> transport);
    ~FireboltClient();

    // Disable copy
    FireboltClient(const FireboltClient&) = delete;
    FireboltClient& operator=(const FireboltClient&) = delete;

    /**
     * @brief Sends a JSON-RPC request
     * @tparam T Response model type
     * @param method JSON-RPC method name
     * @param params Request parameters as FireboltObject
     * @return FireboltOperation<T> for the request
     */
    template<typename T>
    FireboltOperation<T> Send(const std::string& method, const FireboltObject& params);

    /**
     * @brief Subscribes to an event
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
    void OnTransportMessage(const std::string& message);
    std::string BuildJsonRpcRequest(uint64_t requestId, const std::string& method, const std::string& paramsJson);

    std::shared_ptr<IFireboltTextTransport> m_transport;
    uint64_t m_nextRequestId;
    std::unordered_map<uint64_t, IOperationState*> m_pendingRequests;
    std::unordered_map<uint64_t, ISubscriptionState*> m_subscriptions;
    std::unordered_map<uint64_t, EventCallback<std::string>> m_eventCallbacks;
    std::mutex m_mutex;
};

// Template implementations
template<typename T>
FireboltOperation<T> FireboltClient::Send(const std::string& method, const FireboltObject& params) {
    uint64_t requestId = m_nextRequestId++;

    // Create operation state (owned by runtime)
    auto* state = common::StateFactory::CreateOperationState(requestId);

    // Register state with correlation map
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pendingRequests[requestId] = state;
    }

    // Build and send JSON-RPC request via transport
    std::string paramsJson = params.toString();
    std::string requestJson = BuildJsonRpcRequest(requestId, method, paramsJson);
    m_transport->send(requestJson);

    // Return wrapper - state ownership stays with runtime
    return FireboltOperation<T>(state);
}

template<typename T>
FireboltSubscription FireboltClient::Subscribe(const std::string& event, EventCallback<T> listener) {
    uint64_t subscriptionId = m_nextRequestId++;

    // Create subscription state (owned by runtime)
    auto* state = common::StateFactory::CreateSubscriptionState(subscriptionId);

    // Register state with correlation map
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_subscriptions[subscriptionId] = state;
    }

    // Register event callback
    state->OnEvent([listener](const std::string& eventJson) {
        T model;
        model.fromString(eventJson);
        listener(model);
    });

    // Send subscription request
    std::string requestJson = BuildJsonRpcRequest(subscriptionId, event, "{}");
    m_transport->send(requestJson);

    // Return wrapper - state ownership stays with runtime
    return FireboltSubscription(state);
}

template<typename T>
T& FireboltClient::GetInterface() {
    // TODO: Implement interface registry
    static T instance(*this);
    return instance;
}

} // namespace core
} // namespace firebolt

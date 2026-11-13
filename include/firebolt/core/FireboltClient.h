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
#include <typeindex>

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

// Forward declaration for wrapper
class FireboltClient;

/**
 * @brief Lifetime-safe wrapper for transport message callbacks
 *
 * Holds a raw pointer to the client and safely handles the case where
 * the client is destroyed before a callback is invoked. The wrapper itself
 * is held by shared_ptr in the callback, so it outlives the client if needed.
 */
class MessageCallbackWrapper {
public:
    explicit MessageCallbackWrapper(FireboltClient* client)
        : m_client(client) {
    }
    
    void OnMessageReceived(const std::string& message);

private:
    FireboltClient* m_client;  // Raw pointer; safe because we check m_isShutdown
};

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
     * @brief Unregisters the receiver and waits for in-flight callbacks to finish
     *
     * Must be called before the client is destroyed if the transport may outlive it.
     * Prevents use-after-free bugs when the transport delivers callbacks after
     * the client is destroyed.
     */
    void shutdown();

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
    friend class MessageCallbackWrapper;
    
    void OnTransportMessage(const std::string& message);
    std::string BuildJsonRpcRequest(uint64_t requestId, const std::string& method, const std::string& paramsJson);

    std::shared_ptr<IFireboltTextTransport> m_transport;
    uint64_t m_nextRequestId;
    std::unordered_map<uint64_t, std::shared_ptr<IOperationState>> m_pendingRequests;
    std::unordered_map<uint64_t, std::shared_ptr<ISubscriptionState>> m_subscriptions;
    std::unordered_map<uint64_t, EventCallback<std::string>> m_eventCallbacks;
    
    // Per-client interface registry using type_index as key and shared_ptr for lifetime management
    // Maps type_index to a shared_ptr that deletes the interface instance
    std::unordered_map<std::type_index, std::shared_ptr<void>> m_interfaceRegistry;
    
    std::mutex m_mutex;
    bool m_isShutdown = false;
};

// Template implementations
template<typename T>
FireboltOperation<T> FireboltClient::Send(const std::string& method, const FireboltObject& params) {
    uint64_t requestId;
    std::shared_ptr<IOperationState> state;

    // Allocate ID and create state under the lock to prevent ID reuse
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        requestId = m_nextRequestId++;
        state = common::StateFactory::CreateOperationState(requestId);
        m_pendingRequests[requestId] = state;
    }

    // Open transport if disconnected
    if (!m_transport->isConnected()) {
        if (!m_transport->open()) {
            // Connection failed - complete operation with error and remove from map
            std::string errorJson = R"({"error":{"message":"Failed to connect transport"}})";
            state->Complete(errorJson);
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_pendingRequests.erase(requestId);
            }
            return FireboltOperation<T>(state);
        }
    }

    // Build and send JSON-RPC request via transport (outside lock to allow concurrent operations)
    std::string paramsJson = params.toString();
    std::string requestJson = BuildJsonRpcRequest(requestId, method, paramsJson);
    if (!m_transport->send(requestJson)) {
        // Send failed - complete operation with error and remove from map
        std::string errorJson = R"({"error":{"message":"Failed to send request"}})";
        state->Complete(errorJson);
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_pendingRequests.erase(requestId);
        }
    }

    // Return wrapper - state ownership shared between map and operation handle
    return FireboltOperation<T>(state);
}

template<typename T>
FireboltSubscription FireboltClient::Subscribe(const std::string& event, EventCallback<T> listener) {
    uint64_t subscriptionId;
    std::shared_ptr<ISubscriptionState> state;

    // Allocate ID and create state under the lock to prevent ID reuse
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        subscriptionId = m_nextRequestId++;
        state = common::StateFactory::CreateSubscriptionState(subscriptionId);
        m_subscriptions[subscriptionId] = state;
    }

    // Register event callback using ParseJsonRpcEvent to handle both "result" and "params" fields
    state->OnEvent([listener](const std::string& eventJson) {
        auto parsed = common::ParseJsonRpcEvent<T>(eventJson);
        if (parsed.IsSuccess()) {
            listener(parsed.Value());
        }
    });

    // Open transport if disconnected
    if (!m_transport->isConnected()) {
        if (!m_transport->open()) {
            // Connection failed - deactivate subscription and remove from map
            state->Unsubscribe();
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_subscriptions.erase(subscriptionId);
            }
            return FireboltSubscription(state);
        }
    }

    // Send subscription request (outside lock to allow concurrent operations)
    std::string requestJson = BuildJsonRpcRequest(subscriptionId, event, "{}");
    if (!m_transport->send(requestJson)) {
        // Send failed - deactivate subscription and remove from map
        state->Unsubscribe();
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_subscriptions.erase(subscriptionId);
        }
    }

    // Return wrapper - state ownership shared between map and subscription handle
    return FireboltSubscription(state);
}

template<typename T>
T& FireboltClient::GetInterface() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::type_index typeIdx(typeid(T));
    auto it = m_interfaceRegistry.find(typeIdx);
    
    if (it != m_interfaceRegistry.end()) {
        // Return existing interface for this client
        return *static_cast<T*>(it->second.get());
    }
    
    // Create new interface instance for this client and cache it with type-specific deleter
    T* instance = new T(*this);
    
    // Use a deleter that captures the correct type
    auto deleter = [](void* ptr) {
        delete static_cast<T*>(ptr);
    };
    
    m_interfaceRegistry[typeIdx] = std::shared_ptr<void>(instance, deleter);
    return *instance;
}

} // namespace core
} // namespace firebolt

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
namespace common {

/**
 * @brief Opaque interface for subscription state management
 *
 * This interface is implemented by the runtime library (libfirebolt-common.so)
 * and used by header-only wrappers (FireboltSubscription). It manages event
 * registration, listener callbacks, unsubscription, and automatic cleanup.
 *
 * Responsibilities:
 * - Event subscription tracking
 * - Event unsubscription
 * - Thread synchronization
 */
class ISubscriptionState {
public:
    virtual ~ISubscriptionState() = default;

    /**
     * @brief Registers a listener callback for events
     * @param callback Function to call when an event is received
     */
    virtual void OnEvent(std::function<void(const std::string&)> callback) = 0;

    /**
     * @brief Unsubscribes from the event
     * @return true if unsubscription succeeded, false otherwise
     */
    virtual bool Unsubscribe() = 0;

    /**
     * @brief Checks if the subscription is active
     * @return true if active, false if unsubscribed
     */
    virtual bool IsActive() const = 0;

    /**
     * @brief Gets the subscription ID for correlation
     * @return Subscription ID
     */
    virtual uint64_t GetSubscriptionId() const = 0;

    /**
     * @brief Called by client when an event is received
     * @param event JSON event string
     */
    virtual void DeliverEvent(const std::string& event) = 0;
};

} // namespace common
} // namespace firebolt

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

#include "FireboltResult.h"
#include "ISubscriptionState.h"

namespace firebolt {
namespace common {

/**
 * @brief Header-only wrapper for an active Firebolt event registration
 *
 * Represents a long-lived event registration. Manages subscription lifetime
 * and supports explicit unsubscription and automatic cleanup on destruction.
 */
class FireboltSubscription {
public:
    explicit FireboltSubscription(ISubscriptionState* state)
        : m_state(state) {
    }

    ~FireboltSubscription() {
        // Automatically unsubscribe on destruction
        if (m_state) {
            m_state->Unsubscribe();
        }
    }

    // Disable copy
    FireboltSubscription(const FireboltSubscription&) = delete;
    FireboltSubscription& operator=(const FireboltSubscription&) = delete;

    // Enable move
    FireboltSubscription(FireboltSubscription&&) noexcept = default;
    FireboltSubscription& operator=(FireboltSubscription&&) noexcept = default;

    /**
     * @brief Checks if the subscription is valid
     * @return true if valid, false if invalid
     */
    bool IsValid() const {
        return m_state != nullptr;
    }

    /**
     * @brief Checks if the subscription is active
     * @return true if active, false if unsubscribed
     */
    bool IsActive() const {
        return m_state ? m_state->IsActive() : false;
    }

    /**
     * @brief Unsubscribes from the event
     * @return true if unsubscription succeeded, false otherwise
     */
    bool Unsubscribe() {
        return m_state ? m_state->Unsubscribe() : false;
    }

private:
    ISubscriptionState* m_state;
};

} // namespace common
} // namespace firebolt

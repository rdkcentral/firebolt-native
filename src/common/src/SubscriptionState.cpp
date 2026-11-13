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

#include "firebolt/common/SubscriptionState.h"

#include <unordered_map>

namespace firebolt {
namespace common {

namespace {
thread_local std::unordered_map<const SubscriptionState*, uint64_t> activeDeliveries;
}

SubscriptionState::SubscriptionState(uint64_t subscriptionId)
    : m_subscriptionId(subscriptionId)
    , m_active(true)
    , m_inflightDeliveries(0) {
}

SubscriptionState::~SubscriptionState() {
    Unsubscribe();
}

void SubscriptionState::OnEvent(std::function<void(const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_callback = callback;
}

bool SubscriptionState::Unsubscribe() {
    std::unique_lock<std::mutex> lock(m_mutex);
    const bool wasActive = m_active.exchange(false);
    const auto currentThreadDeliveries = activeDeliveries.find(this);
    const uint64_t ownDeliveries = currentThreadDeliveries == activeDeliveries.end()
        ? 0
        : currentThreadDeliveries->second;

    // Wait for other dispatches, but do not deadlock when called by a listener.
    m_deliveryDone.wait(lock, [this, ownDeliveries]() {
        return m_inflightDeliveries.load() <= ownDeliveries;
    });
    m_callback = nullptr;
    return wasActive;
}

bool SubscriptionState::IsActive() const {
    return m_active.load();
}

uint64_t SubscriptionState::GetSubscriptionId() const {
    return m_subscriptionId;
}

void SubscriptionState::DeliverEvent(const std::string& event) {
    std::function<void(const std::string&)> callback;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_active.load()) {
            return;
        }
        // Register the dispatch under the same lock used to deactivate the
        // subscription, so Unsubscribe cannot miss a callback about to run.
        ++m_inflightDeliveries;
        callback = m_callback;
    }

    // Decrement under the same mutex used by Unsubscribe's wait predicate,
    // including when the listener throws.
    const auto finishDelivery = [this]() {
        bool noInflightDeliveries = false;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            noInflightDeliveries = (--m_inflightDeliveries == 0);
        }
        m_deliveryDone.notify_all();
    };

    // Invoke callback outside lock so listener can safely call Unsubscribe().
    try {
        if (callback) {
            ++activeDeliveries[this];
            callback(event);
            auto delivery = activeDeliveries.find(this);
            if (--delivery->second == 0) {
                activeDeliveries.erase(delivery);
            }
        }
    } catch (...) {
        if (callback) {
            auto delivery = activeDeliveries.find(this);
            if (delivery != activeDeliveries.end() && --delivery->second == 0) {
                activeDeliveries.erase(delivery);
            }
        }
        finishDelivery();
        throw;
    }
    finishDelivery();
}

} // namespace common
} // namespace firebolt

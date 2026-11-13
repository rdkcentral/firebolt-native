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

namespace firebolt {
namespace common {

SubscriptionState::SubscriptionState(uint64_t subscriptionId)
    : m_subscriptionId(subscriptionId)
    , m_active(true) {
}

SubscriptionState::~SubscriptionState() {
    Unsubscribe();
}

void SubscriptionState::OnEvent(std::function<void(const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_callback = callback;
}

bool SubscriptionState::Unsubscribe() {
    if (!m_active.load()) {
        return false;
    }
    m_active = false;
    std::lock_guard<std::mutex> lock(m_mutex);
    m_callback = nullptr;
    return true;
}

bool SubscriptionState::IsActive() const {
    return m_active.load();
}

uint64_t SubscriptionState::GetSubscriptionId() const {
    return m_subscriptionId;
}

void SubscriptionState::DeliverEvent(const std::string& event) {
    if (!m_active.load()) {
        return;
    }
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_callback) {
        m_callback(event);
    }
}

} // namespace common
} // namespace firebolt

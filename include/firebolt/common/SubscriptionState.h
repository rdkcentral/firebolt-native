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

#include "firebolt/common/ISubscriptionState.h"
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <string>

namespace firebolt {
namespace common {

class SubscriptionState : public ISubscriptionState {
public:
    explicit SubscriptionState(uint64_t subscriptionId);
    ~SubscriptionState() override;

    void OnEvent(std::function<void(const std::string&)> callback) override;
    bool Unsubscribe() override;
    bool IsActive() const override;
    uint64_t GetSubscriptionId() const override;
    void DeliverEvent(const std::string& event) override;

private:
    uint64_t m_subscriptionId;
    std::atomic<bool> m_active;
    std::atomic<int> m_inflightDeliveries;  // Track in-flight DeliverEvent calls
    std::function<void(const std::string&)> m_callback;
    std::condition_variable m_deliveryDone;  // Signals when all deliveries complete
    mutable std::mutex m_mutex;
};

} // namespace common
} // namespace firebolt

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

#include "firebolt/common/IOperationState.h"
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <string>

namespace firebolt {
namespace common {

class OperationState : public IOperationState {
public:
    explicit OperationState(uint64_t requestId);
    ~OperationState() override = default;

    void OnComplete(std::function<void(const std::string&)> callback) override;
    bool IsCompleted() const override;
    bool Cancel() override;
    uint64_t GetRequestId() const override;
    std::string Get(std::chrono::milliseconds timeout) override;
    void Complete(const std::string& response) override;

private:
    uint64_t m_requestId;
    std::atomic<bool> m_completed;
    std::atomic<bool> m_cancelled;
    std::function<void(const std::string&)> m_callback;
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    std::string m_response;
};

} // namespace common
} // namespace firebolt

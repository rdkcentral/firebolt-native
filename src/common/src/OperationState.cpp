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

#include "firebolt/common/OperationState.h"

namespace firebolt {
namespace common {

OperationState::OperationState(uint64_t requestId)
    : m_requestId(requestId)
    , m_completed(false)
    , m_cancelled(false) {
}

void OperationState::OnComplete(std::function<void(const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_callback = callback;
}

bool OperationState::IsCompleted() const {
    return m_completed.load();
}

bool OperationState::Cancel() {
    if (m_completed.load()) {
        return false;
    }
    m_cancelled = true;
    m_completed = true;
    m_cv.notify_all();
    return true;
}

uint64_t OperationState::GetRequestId() const {
    return m_requestId;
}

std::string OperationState::Get(std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_cv.wait_for(lock, timeout, [this] { return m_completed.load(); })) {
        return m_response;
    }
    return "";
}

void OperationState::Complete(const std::string& response) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_cancelled.load()) {
            return;
        }
        m_response = response;
        m_completed = true;
    }
    m_cv.notify_all();

    if (m_callback) {
        m_callback(response);
    }
}

} // namespace common
} // namespace firebolt

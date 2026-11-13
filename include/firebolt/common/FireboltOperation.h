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
#include <chrono>
#include "FireboltResult.h"
#include "FireboltModel.h"
#include "IOperationState.h"
#include "JsonRpcResult.h"

namespace firebolt {
namespace common {

/**
 * @brief Callback type for operation results
 * @tparam T Type of the result value (model type)
 */
template<typename T>
using OperationCallback = std::function<void(const FireboltResult<T>&)>;

/**
 * @brief Header-only wrapper for a pending Firebolt request
 *
 * Represents a pending request that will eventually complete with a typed
 * response model. Provides both callback-based async consumption and blocking
 * sync consumption with timeout.
 *
 * @tparam T Type of the response model (e.g., LifecycleState, DeviceInfo)
 */
template<typename T>
class FireboltOperation {
public:
    explicit FireboltOperation(std::shared_ptr<IOperationState> state)
        : m_state(state) {
    }

    ~FireboltOperation() = default;

    // Disable copy
    FireboltOperation(const FireboltOperation&) = delete;
    FireboltOperation& operator=(const FireboltOperation&) = delete;

    // Enable move
    FireboltOperation(FireboltOperation&&) noexcept = default;
    FireboltOperation& operator=(FireboltOperation&&) noexcept = default;

    /**
     * @brief Checks if the operation is valid
     * @return true if valid, false if invalid
     */
    bool IsValid() const {
        return m_state != nullptr;
    }

    /**
     * @brief Checks if the operation is completed
     * @return true if completed, false if pending
     */
    bool IsCompleted() const {
        return m_state ? m_state->IsCompleted() : false;
    }

    /**
     * @brief Cancels the operation
     * @return true if cancellation succeeded, false otherwise
     */
    bool Cancel() {
        return m_state ? m_state->Cancel() : false;
    }

    /**
     * @brief Registers a callback for async consumption
     * @param callback Function to call when operation completes with model T
     */
    void Then(OperationCallback<T> callback) {
        if (!m_state) {
            return;
        }

        m_state->OnComplete([callback](const std::string& responseJson) {
            callback(ParseJsonRpcResult<T>(responseJson));
        });
    }

    /**
     * @brief Blocks until operation completes or timeout expires
     * @param timeout Maximum time to wait
     * @return Result containing model T
     */
    FireboltResult<T> Get(std::chrono::milliseconds timeout) {
        if (!m_state) {
            return FireboltResult<T>::Failure("Invalid operation state");
        }

        // Block on internal state until completion or timeout
        std::string responseJson = m_state->Get(timeout);

        if (responseJson.empty()) {
            return FireboltResult<T>::Failure("Operation timed out or was cancelled");
        }

        return ParseJsonRpcResult<T>(responseJson);
    }

private:
    std::shared_ptr<IOperationState> m_state;
};

} // namespace common
} // namespace firebolt

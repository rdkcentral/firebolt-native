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
#include <cstdint>
#include <chrono>

namespace firebolt {
namespace common {

/**
 * @brief Opaque interface for operation state management
 *
 * This interface is implemented by the runtime library (libfirebolt-common.so)
 * and used by header-only wrappers (FireboltOperation<T>). It manages request
 * lifecycle, correlation, callback dispatching, and synchronization.
 *
 * Responsibilities:
 * - Request lifecycle management
 * - JSON-RPC correlation
 * - Callback dispatching
 * - Request cancellation
 * - Request completion tracking
 * - Thread synchronization
 * - Transport interaction
 */
class IOperationState {
public:
    virtual ~IOperationState() = default;

    /**
     * @brief Registers a callback to be called when the operation completes
     * @param callback Function to call with the response JSON string
     */
    virtual void OnComplete(std::function<void(const std::string&)> callback) = 0;

    /**
     * @brief Checks if the operation is completed
     * @return true if completed, false if pending
     */
    virtual bool IsCompleted() const = 0;

    /**
     * @brief Cancels the operation
     * @return true if cancellation succeeded, false otherwise
     */
    virtual bool Cancel() = 0;

    /**
     * @brief Gets the request ID for correlation
     * @return Request ID
     */
    virtual uint64_t GetRequestId() const = 0;

    /**
     * @brief Blocks until operation completes or timeout expires
     * @param timeout Maximum time to wait
     * @return Response JSON string, empty if timeout or cancelled
     */
    virtual std::string Get(std::chrono::milliseconds timeout) = 0;

    /**
     * @brief Called by client when response is received
     * @param response JSON response string
     */
    virtual void Complete(const std::string& response) = 0;
};

} // namespace common
} // namespace firebolt

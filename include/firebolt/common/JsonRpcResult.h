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

#include <string>

#include "FireboltObject.h"
#include "FireboltResult.h"

namespace firebolt {
namespace common {

/**
 * @brief Converts a full JSON-RPC response or event message into a typed result
 * @tparam T Model type read from the "result" member
 * @param jsonRpc Full JSON-RPC message string
 * @return Success with the model read from "result", or Failure from "error"
 */
template<typename T>
FireboltResult<T> ParseJsonRpcResult(const std::string& jsonRpc) {
    try {
        FireboltObject envelope;
        envelope.fromString(jsonRpc);

        if (envelope.has("error")) {
            FireboltObject error = envelope.getObject("error");
            return FireboltResult<T>::Failure(
                error.has("message") ? error.get<std::string>("message") : "Unknown error");
        }

        if (!envelope.has("result")) {
            return FireboltResult<T>::Failure("Response has no result");
        }

        T model;
        model.fromObject(envelope.getObject("result"));
        return FireboltResult<T>::Success(model);
    } catch (const std::exception& error) {
        return FireboltResult<T>::Failure(error.what());
    }
}

} // namespace common
} // namespace firebolt

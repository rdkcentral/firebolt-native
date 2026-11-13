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

#include <cstdint>
#include <string>
#include <vector>

namespace firebolt {
namespace secure {
namespace core {

/**
 * @brief Converts between JSON-RPC strings and the Protobuf RPC envelope
 *
 * Envelope fields:
 * - RpcRequest: request_id (uint64), service (RPC_SERVICE_JSON_RPC),
 *   schema_version, and payload (serialized CoreRequest with id, method, params)
 * - RpcResponse: request_id (uint64), service, schema_version, status (RPC_STATUS_OK
 *   on success), and payload (serialized CoreResponse holding the JSON-RPC result or error)
 *
 * Responses and events arrive as RpcResponse. Unwrapping rebuilds the JSON-RPC
 * message so the secure path hands the same JSON shape to models as the core
 * path does.
 */
class EnvelopeWrapper {
public:
    /**
     * @brief Wraps a JSON-RPC request in an RpcRequest envelope
     * @param requestId Request correlation ID
     * @param method JSON-RPC method name
     * @param paramsJson Parameters as a JSON string
     * @return Serialized RpcRequest bytes
     */
    static std::vector<uint8_t> WrapRequest(uint64_t requestId, const std::string& method, const std::string& paramsJson);

    /**
     * @brief Unwraps an RpcResponse envelope into a JSON-RPC message
     *
     * A non-OK RPC status is converted into a JSON-RPC error so the waiting
     * caller is completed instead of hanging.
     *
     * @param data Serialized RpcResponse bytes
     * @param requestId Envelope request ID used for correlation
     * @param jsonRpc Reconstructed JSON-RPC message
     * @return true if the envelope was parsed, false otherwise
     */
    static bool UnwrapResponse(const std::vector<uint8_t>& data, uint64_t& requestId, std::string& jsonRpc);

private:
    EnvelopeWrapper() = delete;
};

} // namespace core
} // namespace secure
} // namespace firebolt

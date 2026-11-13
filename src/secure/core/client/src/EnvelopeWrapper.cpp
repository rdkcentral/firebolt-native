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

#include "firebolt/secure/core/EnvelopeWrapper.h"
#include "firebolt/proto/rpc.pb.h"
#include "firebolt/proto/core.pb.h"
#include <nlohmann/json.hpp>

namespace firebolt {
namespace secure {
namespace core {

namespace {

constexpr uint32_t kJsonRpcSchemaVersion = 1;
constexpr int kRpcErrorCode = -32000;

// CoreResponse carries JSON fragments as strings; fall back to a plain string if a fragment is not valid JSON.
nlohmann::json ParseJsonFragment(const std::string& fragment) {
    auto parsed = nlohmann::json::parse(fragment, nullptr, false);
    return parsed.is_discarded() ? nlohmann::json(fragment) : parsed;
}

std::vector<uint8_t> ToBytes(const std::string& serialized) {
    return std::vector<uint8_t>(serialized.begin(), serialized.end());
}

} // namespace

std::vector<uint8_t> EnvelopeWrapper::WrapRequest(uint64_t requestId, const std::string& method, const std::string& paramsJson) {
    // CoreRequest.id is int32 on the wire; request IDs are sequential and stay well below INT32_MAX.
    CoreRequest coreRequest;
    coreRequest.set_id(static_cast<int32_t>(requestId));
    coreRequest.set_method(method);
    coreRequest.set_params(paramsJson);

    RpcRequest rpcRequest;
    rpcRequest.set_request_id(requestId);
    rpcRequest.set_service(RPC_SERVICE_JSON_RPC);
    rpcRequest.set_schema_version(kJsonRpcSchemaVersion);
    rpcRequest.set_payload(coreRequest.SerializeAsString());

    return ToBytes(rpcRequest.SerializeAsString());
}

bool EnvelopeWrapper::UnwrapResponse(const std::vector<uint8_t>& data, uint64_t& requestId, std::string& jsonRpc) {
    RpcResponse rpcResponse;
    if (!rpcResponse.ParseFromArray(data.data(), static_cast<int>(data.size()))) {
        return false;
    }

    requestId = rpcResponse.request_id();

    if (rpcResponse.status() != RPC_STATUS_OK) {
        nlohmann::json error;
        error["id"] = requestId;
        error["error"]["code"] = kRpcErrorCode;
        error["error"]["message"] = "RPC status " + std::to_string(static_cast<int>(rpcResponse.status()));
        jsonRpc = error.dump();
        return true;
    }

    CoreResponse coreResponse;
    if (!coreResponse.ParseFromString(rpcResponse.payload())) {
        return false;
    }

    nlohmann::json message;
    message["id"] = coreResponse.id();
    if (!coreResponse.method().empty()) {
        message["method"] = coreResponse.method();
    }
    if (!coreResponse.result().empty()) {
        message["result"] = ParseJsonFragment(coreResponse.result());
    }
    if (!coreResponse.error().empty()) {
        message["error"] = ParseJsonFragment(coreResponse.error());
    }
    if (!coreResponse.params().empty()) {
        message["params"] = ParseJsonFragment(coreResponse.params());
    }

    jsonRpc = message.dump();
    return true;
}

} // namespace core
} // namespace secure
} // namespace firebolt

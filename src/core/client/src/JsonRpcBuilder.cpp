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

#include "firebolt/common/FireboltObject.h"
#include "firebolt/common/Logging.h"
#include <nlohmann/json.hpp>
#include <string>

namespace firebolt {
namespace core {

/**
 * @brief Helper class for constructing JSON-RPC requests
 */
class JsonRpcBuilder {
public:
    /**
     * @brief Builds a JSON-RPC request string
     * @param requestId Request ID
     * @param method Method name
     * @param params Parameters as JSON string
     * @return JSON-RPC request string
     */
    static std::string BuildRequest(uint64_t requestId, const std::string& method, const std::string& params);

private:
    JsonRpcBuilder() = delete;
};

std::string JsonRpcBuilder::BuildRequest(uint64_t requestId, const std::string& method, const std::string& params) {
    nlohmann::json request;
    request["id"] = requestId;
    request["method"] = method;
    request["params"] = nlohmann::json::parse(params);
    return request.dump();
}

} // namespace core
} // namespace firebolt

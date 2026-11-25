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
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <firebolt/core/FireboltClient.h>
#include <firebolt/core/Shared.h>
#include <firebolt/common/FireboltModel.h>
#include <firebolt/common/FireboltObject.h>
#include <firebolt/common/FireboltOperation.h>
#include <firebolt/common/FireboltSubscription.h>

namespace firebolt {
namespace core {
namespace discovery {

using common::FireboltObject;
using common::FireboltOperation;
using common::FireboltSubscription;
using common::FireboltModel;

// Primitive type definition
using FireboltString = std::string;

// Import AgePolicy from shared module
using shared::AgePolicy;



class IDiscovery {
public:
    virtual ~IDiscovery() = default;

    virtual FireboltOperation<std::string> Watched(std::string entityId, std::optional<double> progress = std::nullopt, std::optional<bool> completed = std::nullopt, std::optional<std::string> watchedOn = std::nullopt, std::optional<AgePolicy> agePolicy = std::nullopt) = 0;

};

class Discovery : public IDiscovery {
public:
    explicit Discovery(core::FireboltClient& client)
        : m_client(client)
    {{
    }}

    FireboltOperation<std::string> Watched(std::string entityId, std::optional<double> progress = std::nullopt, std::optional<bool> completed = std::nullopt, std::optional<std::string> watchedOn = std::nullopt, std::optional<AgePolicy> agePolicy = std::nullopt) override
    {{
        return m_client.Send<std::string>("discovery.watched", FireboltObject{});
    }}

private:
    core::FireboltClient& m_client;
};

inline IDiscovery& DiscoveryInterface(core::FireboltClient& client)
{
    return client.GetInterface<Discovery>();
}

} // namespace discovery
} // namespace core
} // namespace firebolt

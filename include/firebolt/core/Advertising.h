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
#include <firebolt/common/FireboltModel.h>
#include <firebolt/common/FireboltObject.h>
#include <firebolt/common/FireboltOperation.h>
#include <firebolt/common/FireboltSubscription.h>

namespace firebolt {
namespace core {
namespace advertising {

using common::FireboltObject;
using common::FireboltOperation;
using common::FireboltSubscription;
using common::FireboltModel;

// Primitive type definition
using FireboltString = std::string;

struct AdvertisingId {
    std::string ifa;
    std::string ifa_type;
    std::string lmt;
};

enum class IfaType {
    Dpid,  // Device-provided ID
    Sspid,  // SSP (Supply-Side Platform) provided ID
    Sessionid,  // Session/synthetic ID
};

enum class Lmt {
    _0,  // Limit ad tracking disabled
    _1,  // Limit ad tracking enabled
};


class IAdvertising {
public:
    virtual ~IAdvertising() = default;

    virtual FireboltOperation<AdvertisingId> Advertisingid() = 0;

};

class Advertising : public IAdvertising {
public:
    explicit Advertising(core::FireboltClient& client)
        : m_client(client)
    {{
    }}

    FireboltOperation<AdvertisingId> Advertisingid() override
    {{
        return m_client.Send<AdvertisingId>("advertising.advertisingId", FireboltObject{});
    }}

private:
    core::FireboltClient& m_client;
};

inline IAdvertising& AdvertisingInterface(core::FireboltClient& client)
{
    return client.GetInterface<Advertising>();
}

} // namespace advertising
} // namespace core
} // namespace firebolt

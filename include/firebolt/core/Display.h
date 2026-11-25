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
namespace display {

using common::FireboltObject;
using common::FireboltOperation;
using common::FireboltSubscription;
using common::FireboltModel;

// Primitive type definition
using FireboltString = std::string;

enum class ColorimetryValue {
    Bt709,  // BT.709 colorimetry
    Bt2020,  // BT.2020 colorimetry
};

enum class VideoResolution {
    _720p50,  // 720p at 50Hz
    _720p60,  // 720p at 60Hz
    _1080p50,  // 1080p at 50Hz
    _1080p60,  // 1080p at 60Hz
    _2160p50,  // 2160p at 50Hz
    _2160p60,  // 2160p at 60Hz
};


class IDisplay {
public:
    virtual ~IDisplay() = default;

};

class Display : public IDisplay {
public:
    explicit Display(core::FireboltClient& client)
        : m_client(client)
    {{
    }}

private:
    core::FireboltClient& m_client;
};

inline IDisplay& DisplayInterface(core::FireboltClient& client)
{
    return client.GetInterface<Display>();
}

} // namespace display
} // namespace core
} // namespace firebolt

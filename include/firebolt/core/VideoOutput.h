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
namespace videooutput {

using common::FireboltObject;
using common::FireboltOperation;
using common::FireboltSubscription;
using common::FireboltModel;

// Primitive type definition
using FireboltString = std::string;

struct VideoResolution {
    uint64_t width;
    uint64_t height;
};

enum class HdcpType {
    Hdcp1_4,  // HDCP 1.4 negotiated
    Hdcp2_2,  // HDCP 2.2 negotiated
    None,  // No encrypted connection established
    Direct,  // Direct connection (TV device)
};

struct ResolutionChangedEvent : public FireboltModel<ResolutionChangedEvent> {
    std::string resolution;

    void write(FireboltObject& object) const;
    void read(const FireboltObject& object);
};

struct HdcpChangedEvent : public FireboltModel<HdcpChangedEvent> {
    std::string hdcp;

    void write(FireboltObject& object) const;
    void read(const FireboltObject& object);
};


class IVideoOutput {
public:
    virtual ~IVideoOutput() = default;

    virtual FireboltOperation<VideoResolution> GetResolution() = 0;

    virtual FireboltOperation<HdcpType> GetHdcp() = 0;

virtual FireboltSubscription OnResolutionChanged(std::function<void(const ResolutionChangedEvent&)> listener) = 0;

virtual FireboltSubscription OnHdcpChanged(std::function<void(const HdcpChangedEvent&)> listener) = 0;

};

class VideoOutput : public IVideoOutput {
public:
    explicit VideoOutput(core::FireboltClient& client)
        : m_client(client)
    {{
    }}

    FireboltOperation<VideoResolution> GetResolution() override
    {{
        return m_client.Send<VideoResolution>("videooutput.resolution", FireboltObject{});
    }}

    FireboltOperation<HdcpType> GetHdcp() override
    {{
        return m_client.Send<HdcpType>("videooutput.hdcp", FireboltObject{});
    }}

    FireboltSubscription OnResolutionChanged(std::function<void(const ResolutionChangedEvent&)> listener) override
    {{
        return m_client.Subscribe<ResolutionChangedEvent>("videooutput.onResolutionChanged", std::move(listener));
    }}

    FireboltSubscription OnHdcpChanged(std::function<void(const HdcpChangedEvent&)> listener) override
    {{
        return m_client.Subscribe<HdcpChangedEvent>("videooutput.onHdcpChanged", std::move(listener));
    }}

private:
    core::FireboltClient& m_client;
};

inline IVideoOutput& VideoOutputInterface(core::FireboltClient& client)
{
    return client.GetInterface<VideoOutput>();
}

void ResolutionChangedEvent::write(FireboltObject& object) const
{
    // TODO: Implement serialization
}

void ResolutionChangedEvent::read(const FireboltObject& object)
{
    // TODO: Implement deserialization
}

void HdcpChangedEvent::write(FireboltObject& object) const
{
    // TODO: Implement serialization
}

void HdcpChangedEvent::read(const FireboltObject& object)
{
    // TODO: Implement deserialization
}

} // namespace videooutput
} // namespace core
} // namespace firebolt

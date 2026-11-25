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
namespace device {

using common::FireboltObject;
using common::FireboltOperation;
using common::FireboltSubscription;
using common::FireboltModel;

// Primitive type definition
using FireboltString = std::string;

enum class DeviceClass {
    Ott,  // No tuner/demod, no integrated display
    Stb,  // With tuner/demod, no integrated display
    Tv,  // Possibly tuner/demod, with integrated display
};

struct HdrCapabilities {
    bool hdr10;
    bool hdr10Plus;
    bool dolbyVision;
    bool hlg;
};

struct NameChangedEvent : public FireboltModel<NameChangedEvent> {
    std::string name;

    void write(FireboltObject& object) const;
    void read(const FireboltObject& object);
};


class IDevice {
public:
    virtual ~IDevice() = default;

    virtual FireboltOperation<HdrCapabilities> GetHdr() = 0;

    virtual FireboltOperation<bool> GetDolbyatmosexperienceavailable() = 0;

    virtual FireboltOperation<std::string> GetName() = 0;

    virtual FireboltOperation<std::string> Uid() = 0;

    virtual FireboltOperation<DeviceClass> Deviceclass() = 0;

    virtual FireboltOperation<std::string> Uptime() = 0;

    virtual FireboltOperation<std::string> Brandname() = 0;

    virtual FireboltOperation<std::string> Modelid() = 0;

    virtual FireboltOperation<std::string> Osname() = 0;

    virtual FireboltOperation<std::string> Osversion() = 0;

    virtual FireboltOperation<std::string> Firmware() = 0;

virtual FireboltSubscription OnNameChanged(std::function<void(const NameChangedEvent&)> listener) = 0;

};

class Device : public IDevice {
public:
    explicit Device(core::FireboltClient& client)
        : m_client(client)
    {{
    }}

    FireboltOperation<HdrCapabilities> GetHdr() override
    {{
        return m_client.Send<HdrCapabilities>("device.hdr", FireboltObject{});
    }}

    FireboltOperation<bool> GetDolbyatmosexperienceavailable() override
    {{
        return m_client.Send<bool>("device.dolbyAtmosExperienceAvailable", FireboltObject{});
    }}

    FireboltOperation<std::string> GetName() override
    {{
        return m_client.Send<std::string>("device.name", FireboltObject{});
    }}

    FireboltOperation<std::string> Uid() override
    {{
        return m_client.Send<std::string>("device.uid", FireboltObject{});
    }}

    FireboltOperation<DeviceClass> Deviceclass() override
    {{
        return m_client.Send<DeviceClass>("device.deviceClass", FireboltObject{});
    }}

    FireboltOperation<std::string> Uptime() override
    {{
        return m_client.Send<std::string>("device.uptime", FireboltObject{});
    }}

    FireboltOperation<std::string> Brandname() override
    {{
        return m_client.Send<std::string>("device.brandName", FireboltObject{});
    }}

    FireboltOperation<std::string> Modelid() override
    {{
        return m_client.Send<std::string>("device.modelId", FireboltObject{});
    }}

    FireboltOperation<std::string> Osname() override
    {{
        return m_client.Send<std::string>("device.osName", FireboltObject{});
    }}

    FireboltOperation<std::string> Osversion() override
    {{
        return m_client.Send<std::string>("device.osVersion", FireboltObject{});
    }}

    FireboltOperation<std::string> Firmware() override
    {{
        return m_client.Send<std::string>("device.firmware", FireboltObject{});
    }}

    FireboltSubscription OnNameChanged(std::function<void(const NameChangedEvent&)> listener) override
    {{
        return m_client.Subscribe<NameChangedEvent>("device.onNameChanged", std::move(listener));
    }}

private:
    core::FireboltClient& m_client;
};

inline IDevice& DeviceInterface(core::FireboltClient& client)
{
    return client.GetInterface<Device>();
}

void NameChangedEvent::write(FireboltObject& object) const
{
    // TODO: Implement serialization
}

void NameChangedEvent::read(const FireboltObject& object)
{
    // TODO: Implement deserialization
}

} // namespace device
} // namespace core
} // namespace firebolt

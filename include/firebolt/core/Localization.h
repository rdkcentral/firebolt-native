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
namespace localization {

using common::FireboltObject;
using common::FireboltOperation;
using common::FireboltSubscription;
using common::FireboltModel;

// Primitive type definition
using FireboltString = std::string;

struct TimeZoneChangedEvent : public FireboltModel<TimeZoneChangedEvent> {
    std::string timezone;

    void write(FireboltObject& object) const;
    void read(const FireboltObject& object);
};



class ILocalization {
public:
    virtual ~ILocalization() = default;

    virtual FireboltOperation<std::string> GetCountry() = 0;

    virtual FireboltOperation<std::vector<std::string>> GetPreferredaudiolanguages() = 0;

    virtual FireboltOperation<std::string> GetPresentationlanguage() = 0;

    virtual FireboltOperation<std::string> GetTimezone() = 0;

virtual FireboltSubscription OnTimeZoneChanged(std::function<void(const TimeZoneChangedEvent&)> listener) = 0;

};

class Localization : public ILocalization {
public:
    explicit Localization(core::FireboltClient& client)
        : m_client(client)
    {{
    }}

    FireboltOperation<std::string> GetCountry() override
    {{
        return m_client.Send<std::string>("localization.country", FireboltObject{});
    }}

    FireboltOperation<std::vector<std::string>> GetPreferredaudiolanguages() override
    {{
        return m_client.Send<std::vector<std::string>>("localization.preferredAudioLanguages", FireboltObject{});
    }}

    FireboltOperation<std::string> GetPresentationlanguage() override
    {{
        return m_client.Send<std::string>("localization.presentationLanguage", FireboltObject{});
    }}

    FireboltOperation<std::string> GetTimezone() override
    {{
        return m_client.Send<std::string>("localization.timeZone", FireboltObject{});
    }}

    FireboltSubscription OnTimeZoneChanged(std::function<void(const TimeZoneChangedEvent&)> listener) override
    {{
        return m_client.Subscribe<TimeZoneChangedEvent>("localization.onTimeZoneChanged", std::move(listener));
    }}

private:
    core::FireboltClient& m_client;
};

inline ILocalization& LocalizationInterface(core::FireboltClient& client)
{
    return client.GetInterface<Localization>();
}

void TimeZoneChangedEvent::write(FireboltObject& object) const
{
    // TODO: Implement serialization
}

void TimeZoneChangedEvent::read(const FireboltObject& object)
{
    // TODO: Implement deserialization
}

} // namespace localization
} // namespace core
} // namespace firebolt

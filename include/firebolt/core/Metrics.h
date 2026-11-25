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
namespace metrics {

using common::FireboltObject;
using common::FireboltOperation;
using common::FireboltSubscription;
using common::FireboltModel;

// Primitive type definition
using FireboltString = std::string;

// Import AgePolicy from shared module
using shared::AgePolicy;

enum class ErrorType {
    Network,  // Network-related error
    Media,  // Media playback error
    Restriction,  // Content restriction error
    Entitlement,  // Content entitlement/auth error
    Other,  // Other or uncategorized error
};


class IMetrics {
public:
    virtual ~IMetrics() = default;

    virtual FireboltOperation<void> Ready() = 0;

    virtual FireboltOperation<void> Signin() = 0;

    virtual FireboltOperation<void> Signout() = 0;

    virtual FireboltOperation<void> Startcontent(std::string entityId, AgePolicy agePolicy) = 0;

    virtual FireboltOperation<void> Stopcontent(std::string entityId, AgePolicy agePolicy) = 0;

    virtual FireboltOperation<std::string> Page(std::string pageId, std::optional<AgePolicy> agePolicy = std::nullopt) = 0;

    virtual FireboltOperation<std::string> Error(ErrorType type, std::string code, std::string description, bool visible, std::optional<std::string> parameters = std::nullopt, std::optional<AgePolicy> agePolicy = std::nullopt) = 0;

    virtual FireboltOperation<void> Medialoadstart(std::string entityId, std::optional<AgePolicy> agePolicy = std::nullopt) = 0;

    virtual FireboltOperation<void> Mediaplay(std::string entityId, std::optional<AgePolicy> agePolicy = std::nullopt) = 0;

    virtual FireboltOperation<void> Mediaplaying(std::string entityId, std::optional<AgePolicy> agePolicy = std::nullopt) = 0;

    virtual FireboltOperation<void> Mediapause(std::string entityId, std::optional<AgePolicy> agePolicy = std::nullopt) = 0;

    virtual FireboltOperation<void> Mediawaiting(std::string entityId, std::optional<AgePolicy> agePolicy = std::nullopt) = 0;

    virtual FireboltOperation<void> Mediaseeking(std::string entityId, std::string target, std::optional<AgePolicy> agePolicy = std::nullopt) = 0;

    virtual FireboltOperation<void> Mediaseeked(std::string entityId, std::string position, std::optional<AgePolicy> agePolicy = std::nullopt) = 0;

    virtual FireboltOperation<void> Mediaratechanged(std::string entityId, std::string rate, std::optional<AgePolicy> agePolicy = std::nullopt) = 0;

    virtual FireboltOperation<void> Mediarenditionchanged(std::string entityId, std::string bitrate, std::string width, std::string height, std::optional<std::string> profile = std::nullopt, std::optional<AgePolicy> agePolicy = std::nullopt) = 0;

    virtual FireboltOperation<void> Mediaended(std::string entityId, std::optional<AgePolicy> agePolicy = std::nullopt) = 0;

    virtual FireboltOperation<std::string> Event(std::string schema, std::string data, std::optional<AgePolicy> agePolicy = std::nullopt) = 0;

    virtual FireboltOperation<std::string> Appinfo(std::string build) = 0;

};

class Metrics : public IMetrics {
public:
    explicit Metrics(core::FireboltClient& client)
        : m_client(client)
    {{
    }}

    FireboltOperation<void> Ready() override
    {{
        return m_client.Send<void>("metrics.ready", FireboltObject{});
    }}

    FireboltOperation<void> Signin() override
    {{
        return m_client.Send<void>("metrics.signIn", FireboltObject{});
    }}

    FireboltOperation<void> Signout() override
    {{
        return m_client.Send<void>("metrics.signOut", FireboltObject{});
    }}

    FireboltOperation<void> Startcontent(std::string entityId, AgePolicy agePolicy) override
    {{
        return m_client.Send<void>("metrics.startContent", FireboltObject{});
    }}

    FireboltOperation<void> Stopcontent(std::string entityId, AgePolicy agePolicy) override
    {{
        return m_client.Send<void>("metrics.stopContent", FireboltObject{});
    }}

    FireboltOperation<std::string> Page(std::string pageId, std::optional<AgePolicy> agePolicy = std::nullopt) override
    {{
        return m_client.Send<std::string>("metrics.page", FireboltObject{});
    }}

    FireboltOperation<std::string> Error(ErrorType type, std::string code, std::string description, bool visible, std::optional<std::string> parameters = std::nullopt, std::optional<AgePolicy> agePolicy = std::nullopt) override
    {{
        return m_client.Send<std::string>("metrics.error", FireboltObject{});
    }}

    FireboltOperation<void> Medialoadstart(std::string entityId, std::optional<AgePolicy> agePolicy = std::nullopt) override
    {{
        return m_client.Send<void>("metrics.mediaLoadStart", FireboltObject{});
    }}

    FireboltOperation<void> Mediaplay(std::string entityId, std::optional<AgePolicy> agePolicy = std::nullopt) override
    {{
        return m_client.Send<void>("metrics.mediaPlay", FireboltObject{});
    }}

    FireboltOperation<void> Mediaplaying(std::string entityId, std::optional<AgePolicy> agePolicy = std::nullopt) override
    {{
        return m_client.Send<void>("metrics.mediaPlaying", FireboltObject{});
    }}

    FireboltOperation<void> Mediapause(std::string entityId, std::optional<AgePolicy> agePolicy = std::nullopt) override
    {{
        return m_client.Send<void>("metrics.mediaPause", FireboltObject{});
    }}

    FireboltOperation<void> Mediawaiting(std::string entityId, std::optional<AgePolicy> agePolicy = std::nullopt) override
    {{
        return m_client.Send<void>("metrics.mediaWaiting", FireboltObject{});
    }}

    FireboltOperation<void> Mediaseeking(std::string entityId, std::string target, std::optional<AgePolicy> agePolicy = std::nullopt) override
    {{
        return m_client.Send<void>("metrics.mediaSeeking", FireboltObject{});
    }}

    FireboltOperation<void> Mediaseeked(std::string entityId, std::string position, std::optional<AgePolicy> agePolicy = std::nullopt) override
    {{
        return m_client.Send<void>("metrics.mediaSeeked", FireboltObject{});
    }}

    FireboltOperation<void> Mediaratechanged(std::string entityId, std::string rate, std::optional<AgePolicy> agePolicy = std::nullopt) override
    {{
        return m_client.Send<void>("metrics.mediaRateChanged", FireboltObject{});
    }}

    FireboltOperation<void> Mediarenditionchanged(std::string entityId, std::string bitrate, std::string width, std::string height, std::optional<std::string> profile = std::nullopt, std::optional<AgePolicy> agePolicy = std::nullopt) override
    {{
        return m_client.Send<void>("metrics.mediaRenditionChanged", FireboltObject{});
    }}

    FireboltOperation<void> Mediaended(std::string entityId, std::optional<AgePolicy> agePolicy = std::nullopt) override
    {{
        return m_client.Send<void>("metrics.mediaEnded", FireboltObject{});
    }}

    FireboltOperation<std::string> Event(std::string schema, std::string data, std::optional<AgePolicy> agePolicy = std::nullopt) override
    {{
        return m_client.Send<std::string>("metrics.event", FireboltObject{});
    }}

    FireboltOperation<std::string> Appinfo(std::string build) override
    {{
        return m_client.Send<std::string>("metrics.appInfo", FireboltObject{});
    }}

private:
    core::FireboltClient& m_client;
};

inline IMetrics& MetricsInterface(core::FireboltClient& client)
{
    return client.GetInterface<Metrics>();
}

} // namespace metrics
} // namespace core
} // namespace firebolt

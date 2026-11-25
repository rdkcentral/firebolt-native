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
namespace lifecycle2 {

using common::FireboltObject;
using common::FireboltOperation;
using common::FireboltSubscription;
using common::FireboltModel;

// Primitive type definition
using FireboltString = std::string;

enum class LifecycleState {
    Initializing,  // App is starting up; first observable state
    Paused,  // Foreground but not interactive
    Active,  // Foreground and interactive
    Suspended,  // Background
    Hibernated,  // Deep background with reduced resources
    Terminating,  // Being shut down; terminal state
};

struct StateChangedEvent : public FireboltModel<StateChangedEvent> {
    LifecycleState oldState;
    LifecycleState newState;

    void write(FireboltObject& object) const;
    void read(const FireboltObject& object);
};


class ILifecycle2 {
public:
    virtual ~ILifecycle2() = default;

virtual FireboltSubscription OnStateChanged(std::function<void(const StateChangedEvent&)> listener) = 0;

};

class Lifecycle2 : public ILifecycle2 {
public:
    explicit Lifecycle2(core::FireboltClient& client)
        : m_client(client)
    {{
    }}

    FireboltSubscription OnStateChanged(std::function<void(const StateChangedEvent&)> listener) override
    {{
        return m_client.Subscribe<StateChangedEvent>("lifecycle2.onStateChanged", std::move(listener));
    }}

private:
    core::FireboltClient& m_client;
};

inline ILifecycle2& Lifecycle2Interface(core::FireboltClient& client)
{
    return client.GetInterface<Lifecycle2>();
}

void StateChangedEvent::write(FireboltObject& object) const
{
    // TODO: Implement serialization
}

void StateChangedEvent::read(const FireboltObject& object)
{
    // TODO: Implement deserialization
}

} // namespace lifecycle2
} // namespace core
} // namespace firebolt

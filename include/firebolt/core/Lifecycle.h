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
#include <string>
#include <utility>

#include <firebolt/core/FireboltClient.h>
#include <firebolt/common/FireboltModel.h>
#include <firebolt/common/FireboltObject.h>
#include <firebolt/common/FireboltOperation.h>
#include <firebolt/common/FireboltSubscription.h>

namespace firebolt
{
namespace lifecycle
{

using common::FireboltObject;
using common::FireboltOperation;
using common::FireboltSubscription;
using common::FireboltModel;

// Primitive type definition
using FireboltString = std::string;

class LifecycleState final
    : public FireboltModel<LifecycleState>
{
public:
    FireboltString state;

    void write(FireboltObject& object) const
    {
        object.set("state", state);
    }

    void read(const FireboltObject& object)
    {
        state = object.get<std::string>("state");
    }
};

class ILifecycle
{
public:
    virtual ~ILifecycle() = default;

    virtual FireboltOperation<LifecycleState>
    GetLifecycleState() = 0;

    virtual FireboltSubscription
    OnStateChanged(
        std::function<void(const LifecycleState&)> listener) = 0;
};

class Lifecycle final : public ILifecycle
{
public:
    explicit Lifecycle(core::FireboltClient& client)
        : m_client(client)
    {
    }

    FireboltOperation<LifecycleState>
    GetLifecycleState() override
    {
        return m_client.Send<LifecycleState>(
            "lifecycle.state",
            FireboltObject{});
    }

    FireboltSubscription
    OnStateChanged(
        std::function<void(const LifecycleState&)> listener) override
    {
        return m_client.Subscribe<LifecycleState>(
            "lifecycle.onStateChanged",
            std::move(listener));
    }

private:
    core::FireboltClient& m_client;
};

} // namespace lifecycle
} // namespace firebolt

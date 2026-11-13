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

#include "firebolt/common/IOperationState.h"
#include "firebolt/common/ISubscriptionState.h"
#include <memory>

namespace firebolt {
namespace common {

/**
 * @brief Factory functions for creating state instances
 *
 * Returns shared_ptr to ensure proper lifetime management. Both operation and
 * subscription states are owned by both the client map and the handle wrapper.
 * When the client removes an entry from its map, the handle wrapper (FireboltOperation
 * or FireboltSubscription) retains ownership until destroyed.
 */
class StateFactory {
public:
    static std::shared_ptr<IOperationState> CreateOperationState(uint64_t requestId);
    static std::shared_ptr<ISubscriptionState> CreateSubscriptionState(uint64_t subscriptionId);
};

} // namespace common
} // namespace firebolt

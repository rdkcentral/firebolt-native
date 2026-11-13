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

#include "firebolt/common/StateFactory.h"
#include "firebolt/common/OperationState.h"
#include "firebolt/common/SubscriptionState.h"

namespace firebolt {
namespace common {

IOperationState* StateFactory::CreateOperationState(uint64_t requestId) {
    return new OperationState(requestId);
}

ISubscriptionState* StateFactory::CreateSubscriptionState(uint64_t subscriptionId) {
    return new SubscriptionState(subscriptionId);
}

} // namespace common
} // namespace firebolt

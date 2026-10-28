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

#include "firebolt/IFireboltTransport.h"

#include <queue>
#include <string>

namespace firebolt {
namespace transport {

/**
 * @brief Simple in-memory transport implementation for testing and demonstration
 *
 * This transport uses an in-memory queue to simulate message passing.
 * It's intended as a simple example for partners to understand the transport pattern.
 */
class ExampleTransport : public IFireboltTransport {
public:
    ExampleTransport() = default;
    ~ExampleTransport() override = default;

    void send(const std::string& message) override;
    std::string receive() override;

private:
    std::queue<std::string> m_messageQueue;
};

} // namespace transport
} // namespace firebolt

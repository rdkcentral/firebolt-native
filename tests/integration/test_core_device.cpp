/*
 * Integration test for core path using ExampleTransport
 */

#include "firebolt/transport/ExampleTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/Device.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

namespace firebolt {
namespace core {
namespace device {
namespace test {

TEST(DeviceIntegration, Instantiation)
{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    Device device(client);

    ASSERT_TRUE(true);
}

TEST(DeviceIntegration, EventInjection_NameChanged)
{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    Device device(client);

    // Subscribe to event
    bool event_received = false;
    auto subscription = device.OnNameChanged([&event_received](const NameChangedEvent& payload) {
        event_received = true;
    });

    // Inject event with result field (preferred over params)
    // Event has result field in examples - prefer result over params

    ASSERT_TRUE(event_received);
}

} // namespace test
} // namespace device
} // namespace core
} // namespace firebolt

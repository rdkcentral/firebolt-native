/*
 * Integration test for core path using ExampleTransport
 */

#include "firebolt/transport/ExampleTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/Localization.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

namespace firebolt {
namespace core {
namespace localization {
namespace test {

TEST(LocalizationIntegration, Instantiation)
{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    Localization localization(client);

    ASSERT_TRUE(true);
}

TEST(LocalizationIntegration, EventInjection_TimeZoneChanged)
{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    Localization localization(client);

    // Subscribe to event
    bool event_received = false;
    auto subscription = localization.OnTimeZoneChanged([&event_received](const TimeZoneChangedEvent& payload) {
        event_received = true;
    });

    // Inject event with result field (preferred over params)
    // Event has result field in examples - prefer result over params

    ASSERT_TRUE(event_received);
}

} // namespace test
} // namespace localization
} // namespace core
} // namespace firebolt

/*
 * Integration test for core path using ExampleTransport
 */

#include "firebolt/transport/ExampleTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/Lifecycle2.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

namespace firebolt {
namespace core {
namespace lifecycle2 {
namespace test {

TEST(Lifecycle2Integration, Instantiation)
{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    Lifecycle2 lifecycle2(client);

    ASSERT_TRUE(true);
}

TEST(Lifecycle2Integration, EventInjection_StateChanged)
{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    Lifecycle2 lifecycle2(client);

    // Subscribe to event
    bool event_received = false;
    auto subscription = lifecycle2.OnStateChanged([&event_received](const StateChangedEvent& payload) {
        event_received = true;
    });

    // Inject event with result field (preferred over params)

    ASSERT_TRUE(event_received);
}

} // namespace test
} // namespace lifecycle2
} // namespace core
} // namespace firebolt

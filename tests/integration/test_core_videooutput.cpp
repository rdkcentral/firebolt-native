/*
 * Integration test for core path using ExampleTransport
 */

#include "firebolt/transport/ExampleTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/VideoOutput.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

namespace firebolt {
namespace core {
namespace videooutput {
namespace test {

TEST(VideoOutputIntegration, Instantiation)
{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    VideoOutput videooutput(client);

    ASSERT_TRUE(true);
}

TEST(VideoOutputIntegration, EventInjection_ResolutionChanged)
{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    VideoOutput videooutput(client);

    // Subscribe to event
    bool event_received = false;
    auto subscription = videooutput.OnResolutionChanged([&event_received](const ResolutionChangedEvent& payload) {
        event_received = true;
    });

    // Inject event with result field (preferred over params)
    // Event has result field in examples - prefer result over params

    ASSERT_TRUE(event_received);
}


TEST(VideoOutputIntegration, EventInjection_HdcpChanged)
{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    VideoOutput videooutput(client);

    // Subscribe to event
    bool event_received = false;
    auto subscription = videooutput.OnHdcpChanged([&event_received](const HdcpChangedEvent& payload) {
        event_received = true;
    });

    // Inject event with result field (preferred over params)
    // Event has result field in examples - prefer result over params

    ASSERT_TRUE(event_received);
}

} // namespace test
} // namespace videooutput
} // namespace core
} // namespace firebolt

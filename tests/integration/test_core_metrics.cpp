/*
 * Integration test for core path using ExampleTransport
 */

#include "firebolt/transport/ExampleTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/Metrics.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

namespace firebolt {
namespace core {
namespace metrics {
namespace test {

TEST(MetricsIntegration, Instantiation)
{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    Metrics metrics(client);

    ASSERT_TRUE(true);
}
} // namespace test
} // namespace metrics
} // namespace core
} // namespace firebolt

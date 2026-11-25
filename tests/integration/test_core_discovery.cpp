/*
 * Integration test for core path using ExampleTransport
 */

#include "firebolt/transport/ExampleTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/Discovery.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

namespace firebolt {
namespace core {
namespace discovery {
namespace test {

TEST(DiscoveryIntegration, Instantiation)
{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    Discovery discovery(client);

    ASSERT_TRUE(true);
}
} // namespace test
} // namespace discovery
} // namespace core
} // namespace firebolt

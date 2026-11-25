/*
 * Integration test for core path using ExampleTransport
 */

#include "firebolt/transport/ExampleTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/Network.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

namespace firebolt {
namespace core {
namespace network {
namespace test {

TEST(NetworkIntegration, Instantiation)
{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    Network network(client);

    ASSERT_TRUE(true);
}
} // namespace test
} // namespace network
} // namespace core
} // namespace firebolt

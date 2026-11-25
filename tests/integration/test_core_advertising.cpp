/*
 * Integration test for core path using ExampleTransport
 */

#include "firebolt/transport/ExampleTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/Advertising.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

namespace firebolt {
namespace core {
namespace advertising {
namespace test {

TEST(AdvertisingIntegration, Instantiation)
{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    Advertising advertising(client);

    ASSERT_TRUE(true);
}
} // namespace test
} // namespace advertising
} // namespace core
} // namespace firebolt

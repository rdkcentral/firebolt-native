/*
 * Integration test for core path using ExampleTransport
 */

#include "firebolt/transport/ExampleTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/Shared.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

namespace firebolt {
namespace core {
namespace shared {
namespace test {

TEST(SharedIntegration, Instantiation)
{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    Shared shared(client);

    ASSERT_TRUE(true);
}
} // namespace test
} // namespace shared
} // namespace core
} // namespace firebolt

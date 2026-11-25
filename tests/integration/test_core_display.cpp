/*
 * Integration test for core path using ExampleTransport
 */

#include "firebolt/transport/ExampleTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/Display.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

namespace firebolt {
namespace core {
namespace display {
namespace test {

TEST(DisplayIntegration, Instantiation)
{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    Display display(client);

    ASSERT_TRUE(true);
}
} // namespace test
} // namespace display
} // namespace core
} // namespace firebolt

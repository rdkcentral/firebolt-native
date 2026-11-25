/*
 * Integration test for core path using ExampleTransport
 */

#include "firebolt/transport/ExampleTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/Accessibility.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

namespace firebolt {
namespace core {
namespace accessibility {
namespace test {

TEST(AccessibilityIntegration, Instantiation)
{
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    Accessibility accessibility(client);

    ASSERT_TRUE(true);
}
} // namespace test
} // namespace accessibility
} // namespace core
} // namespace firebolt

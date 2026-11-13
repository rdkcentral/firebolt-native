/*
 * Integration test for core path using ExampleTransport
 */

#include "firebolt/transport/ExampleTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/Lifecycle.h"
#include <gtest/gtest.h>

namespace firebolt {
namespace core {
namespace test {

TEST(CoreClientIntegration, LifecycleStateRequest) {
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    lifecycle::Lifecycle lifecycle(client);

    auto operation = lifecycle.GetLifecycleState();

    ASSERT_TRUE(operation.IsValid());
}

TEST(CoreClientIntegration, LifecycleStateSubscription) {
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    lifecycle::Lifecycle lifecycle(client);

    bool eventReceived = false;
    auto subscription = lifecycle.OnStateChanged([&eventReceived](const lifecycle::LifecycleState& state) {
        (void)state; // Suppress unused warning
        eventReceived = true;
    });

    ASSERT_TRUE(subscription.IsActive());
}

} // namespace test
} // namespace core
} // namespace firebolt

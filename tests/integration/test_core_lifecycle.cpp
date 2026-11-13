/*
 * Integration test for core path using ExampleTransport
 */

#include "firebolt/transport/ExampleTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/Lifecycle.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

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

TEST(CoreClientIntegration, LifecycleEventWithResultField) {
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    lifecycle::Lifecycle lifecycle(client);

    bool eventReceived = false;
    std::string receivedState;
    auto subscription = lifecycle.OnStateChanged([&eventReceived, &receivedState](const lifecycle::LifecycleState& state) {
        eventReceived = true;
        receivedState = state.state;
    });

    ASSERT_TRUE(subscription.IsActive());

    // Inject an event message with "result" field (notification format)
    std::string eventMessage = R"({"id":1,"result":{"state":"active"}})";
    transport->InjectMessage(eventMessage);

    // Give some time for the message to be processed
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_TRUE(eventReceived);
    EXPECT_EQ(receivedState, "active");
}

TEST(CoreClientIntegration, LifecycleEventWithParamsField) {
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    lifecycle::Lifecycle lifecycle(client);

    bool eventReceived = false;
    std::string receivedState;
    auto subscription = lifecycle.OnStateChanged([&eventReceived, &receivedState](const lifecycle::LifecycleState& state) {
        eventReceived = true;
        receivedState = state.state;
    });

    ASSERT_TRUE(subscription.IsActive());

    // Inject an event message with "params" field (event format)
    // This tests the fix for params-based events
    std::string eventMessage = R"({"id":1,"params":{"state":"idle"}})";
    transport->InjectMessage(eventMessage);

    // Give some time for the message to be processed
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_TRUE(eventReceived);
    EXPECT_EQ(receivedState, "idle");
}

TEST(CoreClientIntegration, LifecycleEventPreferResultOverParams) {
    auto transport = std::make_shared<transport::ExampleTransport>();
    FireboltClient client(transport);

    lifecycle::Lifecycle lifecycle(client);

    bool eventReceived = false;
    std::string receivedState;
    auto subscription = lifecycle.OnStateChanged([&eventReceived, &receivedState](const lifecycle::LifecycleState& state) {
        eventReceived = true;
        receivedState = state.state;
    });

    ASSERT_TRUE(subscription.IsActive());

    // Inject an event message with both "result" and "params" fields
    // Should prefer "result" field
    std::string eventMessage = R"({"id":1,"result":{"state":"active"},"params":{"state":"idle"}})";
    transport->InjectMessage(eventMessage);

    // Give some time for the message to be processed
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_TRUE(eventReceived);
    EXPECT_EQ(receivedState, "active");
}


} // namespace test
} // namespace core
} // namespace firebolt

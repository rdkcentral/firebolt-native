/*
 * Integration tests for the WebSocket transport lifecycle.
 * Uses a mock WebSocket device to test the full client flow.
 */

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "firebolt/IFireboltTextTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/Lifecycle.h"

namespace {

const char* kLifecycleState = "FOREGROUND";
const char* kStateChanged = "BACKGROUND";

// Mock WebSocket device for testing: replies to requests and emits events
class MockWebSocketDevice : public firebolt::IFireboltTextTransport {
public:
    bool open() override { return true; }
    bool isConnected() override { return true; }
    void onConnectionStatusChanged(std::function<void(bool, const std::string&)>) override {}
    bool send(const std::string& message) override {
        auto request = nlohmann::json::parse(message);
        const auto id = request["id"].get<uint64_t>();
        const auto method = request["method"].get<std::string>();
        if (method == "lifecycle.state") {
            receiver(nlohmann::json{{"id", id}, {"result", {{"state", kLifecycleState}}}}.dump());
        } else if (method == "lifecycle.onStateChanged") {
            subscriptionId = id;
        }
        return true;
    }
    void onMessageReceived(std::function<void(const std::string&)> callback) override {
        receiver = std::move(callback);
    }
    void disconnect() override {}

    void emitStateChanged() {
        receiver(nlohmann::json{{"id", subscriptionId}, {"result", {{"state", kStateChanged}}}}.dump());
    }

    std::function<void(const std::string&)> receiver;
    uint64_t subscriptionId = 0;
};

} // namespace

TEST(WebSocketLifecycleIntegration, GetLifecycleStateThroughMockDevice) {
    auto device = std::make_shared<MockWebSocketDevice>();
    firebolt::core::FireboltClient client(device);
    firebolt::core::lifecycle::Lifecycle lifecycle(client);

    auto result = lifecycle.GetLifecycleState().Get(std::chrono::milliseconds(100));

    ASSERT_TRUE(result.IsSuccess());
    EXPECT_EQ(result.Value().state, kLifecycleState);
}

TEST(WebSocketLifecycleIntegration, OnStateChangedDeliversEventsThroughMockDevice) {
    auto device = std::make_shared<MockWebSocketDevice>();
    firebolt::core::FireboltClient client(device);
    firebolt::core::lifecycle::Lifecycle lifecycle(client);

    std::vector<std::string> events;
    auto subscription = lifecycle.OnStateChanged([&events](const auto& state) {
        events.push_back(state.state);
    });
    device->emitStateChanged();

    ASSERT_EQ(events.size(), 1u);
    EXPECT_EQ(events[0], kStateChanged);
}

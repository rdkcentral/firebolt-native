/*
 * Integration tests for the secure (buffer transport) lifecycle path and parity with the core path.
 * Each device simulator answers lifecycle requests and emits state events the way a platform would.
 */

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "firebolt/IFireboltBufferTransport.h"
#include "firebolt/IFireboltTextTransport.h"
#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/Lifecycle.h"
#include "firebolt/proto/core.pb.h"
#include "firebolt/proto/rpc.pb.h"
#include "firebolt/secure/core/FireboltSecureClient.h"
#include "firebolt/secure/core/Lifecycle.h"

namespace {

const char* kLifecycleState = "FOREGROUND";
const char* kStateChanged = "BACKGROUND";

// Text device for the core path: replies to requests and emits events with the subscription id.
class TextDevice : public firebolt::IFireboltTextTransport {
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

// Buffer device for the secure path: decodes the RpcRequest envelope and answers with RpcResponse.
class BufferDevice : public firebolt::IFireboltBufferTransport {
public:
    bool open() override { return true; }
    bool isConnected() override { return true; }
    void onConnectionStatusChanged(std::function<void(bool, const std::string&)>) override {}
    bool send(const std::vector<uint8_t>& data) override {
        RpcRequest envelope;
        envelope.ParseFromArray(data.data(), static_cast<int>(data.size()));
        CoreRequest core;
        core.ParseFromString(envelope.payload());
        if (core.method() == "lifecycle.state") {
            CoreResponse response;
            response.set_id(core.id());
            response.set_result(nlohmann::json{{"state", kLifecycleState}}.dump());
            deliver(envelope.request_id(), response);
        } else if (core.method() == "lifecycle.onStateChanged") {
            subscriptionRequestId = envelope.request_id();
        }
        return true;
    }
    void onMessageReceived(std::function<void(const std::vector<uint8_t>&)> callback) override {
        receiver = std::move(callback);
    }
    void disconnect() override {}

    void emitStateChanged() {
        CoreResponse event;
        event.set_id(subscriptionRequestId);
        event.set_result(nlohmann::json{{"state", kStateChanged}}.dump());
        deliver(subscriptionRequestId, event);
    }

    std::function<void(const std::vector<uint8_t>&)> receiver;
    uint64_t subscriptionRequestId = 0;

private:
    void deliver(uint64_t requestId, const CoreResponse& core) {
        std::string corePayload;
        core.SerializeToString(&corePayload);
        RpcResponse response;
        response.set_request_id(requestId);
        response.set_service(RPC_SERVICE_JSON_RPC);
        response.set_schema_version(1);
        response.set_status(RPC_STATUS_OK);
        response.set_payload(corePayload);
        std::string bytes;
        response.SerializeToString(&bytes);
        receiver(std::vector<uint8_t>(bytes.begin(), bytes.end()));
    }
};

struct Observed {
    std::string state;
    std::vector<std::string> events;
};

// Runs the same application flow on any lifecycle implementation: Get the state, then receive one event.
template<typename LifecycleT, typename DeviceT>
Observed RunLifecycleFlow(LifecycleT& lifecycle, DeviceT& device) {
    Observed observed;
    auto result = lifecycle.GetLifecycleState().Get(std::chrono::milliseconds(100));
    if (result.IsSuccess()) {
        observed.state = result.Value().state;
    }

    auto subscription = lifecycle.OnStateChanged([&observed](const auto& state) {
        observed.events.push_back(state.state);
    });
    device.emitStateChanged();
    return observed;
}

} // namespace

TEST(SecureLifecycleIntegration, GetLifecycleStateThroughBufferDevice) {
    auto device = std::make_shared<BufferDevice>();
    firebolt::secure::core::FireboltSecureClient client(device);
    firebolt::secure::core::lifecycle::Lifecycle lifecycle(client);

    auto result = lifecycle.GetLifecycleState().Get(std::chrono::milliseconds(100));

    ASSERT_TRUE(result.IsSuccess());
    EXPECT_EQ(result.Value().state, kLifecycleState);
}

TEST(SecureLifecycleIntegration, OnStateChangedDeliversEventsThroughEnvelope) {
    auto device = std::make_shared<BufferDevice>();
    firebolt::secure::core::FireboltSecureClient client(device);
    firebolt::secure::core::lifecycle::Lifecycle lifecycle(client);

    std::vector<std::string> events;
    auto subscription = lifecycle.OnStateChanged([&events](const auto& state) {
        events.push_back(state.state);
    });
    device->emitStateChanged();

    ASSERT_EQ(events.size(), 1u);
    EXPECT_EQ(events[0], kStateChanged);
}

TEST(LifecycleParity, CoreAndSecurePathsProduceSameResults) {
    auto textDevice = std::make_shared<TextDevice>();
    firebolt::core::FireboltClient coreClient(textDevice);
    firebolt::core::lifecycle::Lifecycle coreLifecycle(coreClient);
    Observed core = RunLifecycleFlow(coreLifecycle, *textDevice);

    auto bufferDevice = std::make_shared<BufferDevice>();
    firebolt::secure::core::FireboltSecureClient secureClient(bufferDevice);
    firebolt::secure::core::lifecycle::Lifecycle secureLifecycle(secureClient);
    Observed secure = RunLifecycleFlow(secureLifecycle, *bufferDevice);

    EXPECT_EQ(core.state, kLifecycleState);
    EXPECT_EQ(core.state, secure.state);
    EXPECT_EQ(core.events, secure.events);
    EXPECT_EQ(core.events, std::vector<std::string>{kStateChanged});
}

// Buffer device variant that sends events with "params" field instead of "result"
class BufferDeviceWithParams : public firebolt::IFireboltBufferTransport {
public:
    bool open() override { return true; }
    bool isConnected() override { return true; }
    void onConnectionStatusChanged(std::function<void(bool, const std::string&)>) override {}
    bool send(const std::vector<uint8_t>& data) override {
        RpcRequest envelope;
        envelope.ParseFromArray(data.data(), static_cast<int>(data.size()));
        CoreRequest core;
        core.ParseFromString(envelope.payload());
        if (core.method() == "lifecycle.state") {
            CoreResponse response;
            response.set_id(core.id());
            response.set_result(nlohmann::json{{"state", kLifecycleState}}.dump());
            deliver(envelope.request_id(), response);
        } else if (core.method() == "lifecycle.onStateChanged") {
            subscriptionRequestId = envelope.request_id();
        }
        return true;
    }
    void onMessageReceived(std::function<void(const std::vector<uint8_t>&)> callback) override {
        receiver = std::move(callback);
    }
    void disconnect() override {}

    void emitStateChanged() {
        CoreResponse event;
        event.set_id(subscriptionRequestId);
        event.set_params(nlohmann::json{{"state", kStateChanged}}.dump());
        deliver(subscriptionRequestId, event);
    }

    std::function<void(const std::vector<uint8_t>&)> receiver;
    uint64_t subscriptionRequestId = 0;

private:
    void deliver(uint64_t requestId, const CoreResponse& core) {
        std::string corePayload;
        core.SerializeToString(&corePayload);
        RpcResponse response;
        response.set_request_id(requestId);
        response.set_service(RPC_SERVICE_JSON_RPC);
        response.set_schema_version(1);
        response.set_status(RPC_STATUS_OK);
        response.set_payload(corePayload);
        std::string bytes;
        response.SerializeToString(&bytes);
        receiver(std::vector<uint8_t>(bytes.begin(), bytes.end()));
    }
};

TEST(SecureLifecycleIntegration, OnStateChangedDeliversParamsBasedEvents) {
    auto device = std::make_shared<BufferDeviceWithParams>();
    firebolt::secure::core::FireboltSecureClient client(device);
    firebolt::secure::core::lifecycle::Lifecycle lifecycle(client);

    std::vector<std::string> events;
    auto subscription = lifecycle.OnStateChanged([&events](const auto& state) {
        events.push_back(state.state);
    });
    device->emitStateChanged();

    ASSERT_EQ(events.size(), 1u);
    EXPECT_EQ(events[0], kStateChanged);
}


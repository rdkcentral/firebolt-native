/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2026 RDK Management.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "firebolt/secure/core/EnvelopeWrapper.h"
#include "firebolt/secure/core/FireboltSecureClient.h"
#include "firebolt/secure/core/Lifecycle.h"
#include "firebolt/proto/rpc.pb.h"
#include "firebolt/proto/core.pb.h"

using firebolt::secure::core::EnvelopeWrapper;
using firebolt::secure::core::FireboltSecureClient;

namespace {

class MockBufferTransport : public firebolt::IFireboltBufferTransport {
public:
    bool open() override { return true; }
    bool isConnected() override { return true; }
    void onConnectionStatusChanged(
        std::function<void(bool, const std::string&)>) override {}
    bool send(const std::vector<uint8_t>& data) override {
        sent.push_back(data);
        return true;
    }
    void onMessageReceived(std::function<void(const std::vector<uint8_t>&)> callback) override {
        receiver = std::move(callback);
    }
    void disconnect() override {}

    std::vector<std::vector<uint8_t>> sent;
    std::function<void(const std::vector<uint8_t>&)> receiver;
};

std::vector<uint8_t> MakeResponse(uint64_t requestId, RpcStatus status, const std::string& coreJson) {
    // Helper builds the RpcResponse envelope the device side would send back
    RpcResponse response;
    response.set_request_id(requestId);
    response.set_service(RPC_SERVICE_JSON_RPC);
    response.set_schema_version(1);
    response.set_status(status);
    response.set_payload(coreJson);
    std::string bytes;
    response.SerializeToString(&bytes);
    return std::vector<uint8_t>(bytes.begin(), bytes.end());
}

} // namespace

TEST(EnvelopeWrapperTest, WrapRequestEncodesJsonRpcEnvelope) {
    auto bytes = EnvelopeWrapper::WrapRequest(7, "device.name", "{\"x\":1}");

    RpcRequest envelope;
    ASSERT_TRUE(envelope.ParseFromArray(bytes.data(), static_cast<int>(bytes.size())));
    EXPECT_EQ(envelope.request_id(), 7u);
    EXPECT_EQ(envelope.service(), RPC_SERVICE_JSON_RPC);

    CoreRequest core;
    ASSERT_TRUE(core.ParseFromString(envelope.payload()));
    EXPECT_EQ(core.id(), 7);
    EXPECT_EQ(core.method(), "device.name");
    EXPECT_EQ(core.params(), "{\"x\":1}");
}

TEST(EnvelopeWrapperTest, UnwrapOkResponseRebuildsJsonRpc) {
    CoreResponse core;
    core.set_id(7);
    core.set_result("\"Living Room\"");
    std::string corePayload;
    core.SerializeToString(&corePayload);

    uint64_t requestId = 0;
    std::string jsonRpc;
    ASSERT_TRUE(EnvelopeWrapper::UnwrapResponse(
        MakeResponse(7, RPC_STATUS_OK, corePayload), requestId, jsonRpc));

    EXPECT_EQ(requestId, 7u);
    auto json = nlohmann::json::parse(jsonRpc);
    EXPECT_EQ(json["id"], 7);
    EXPECT_EQ(json["result"], "Living Room");
}

TEST(EnvelopeWrapperTest, NonOkStatusBecomesJsonRpcError) {
    uint64_t requestId = 0;
    std::string jsonRpc;
    ASSERT_TRUE(EnvelopeWrapper::UnwrapResponse(
        MakeResponse(9, RPC_STATUS_INTERNAL, ""), requestId, jsonRpc));

    EXPECT_EQ(requestId, 9u);
    auto json = nlohmann::json::parse(jsonRpc);
    EXPECT_EQ(json["id"], 9);
    EXPECT_EQ(json["error"]["code"], -32000);
}

TEST(EnvelopeWrapperTest, MalformedBytesAreRejected) {
    std::vector<uint8_t> garbage = {0xFF, 0xFF, 0xFF, 0xFF};
    uint64_t requestId = 0;
    std::string jsonRpc;
    EXPECT_FALSE(EnvelopeWrapper::UnwrapResponse(garbage, requestId, jsonRpc));
}

TEST(FireboltSecureClientTest, SendWritesEnvelopeAndDeliversResponse) {
    auto transport = std::make_shared<MockBufferTransport>();
    FireboltSecureClient client(transport);
    ASSERT_TRUE(transport->receiver);

    auto operation = client.Send<std::string>("device.name", firebolt::common::FireboltObject());
    ASSERT_EQ(transport->sent.size(), 1u);

    RpcRequest envelope;
    ASSERT_TRUE(envelope.ParseFromArray(transport->sent[0].data(),
                                        static_cast<int>(transport->sent[0].size())));
    CoreRequest core;
    ASSERT_TRUE(core.ParseFromString(envelope.payload()));
    EXPECT_EQ(core.method(), "device.name");

    CoreResponse response;
    response.set_id(core.id());
    response.set_result("\"Living Room\"");
    std::string corePayload;
    response.SerializeToString(&corePayload);
    transport->receiver(MakeResponse(envelope.request_id(), RPC_STATUS_OK, corePayload));

    EXPECT_TRUE(operation.IsCompleted());
}

namespace {

std::string SentMethod(const MockBufferTransport& transport, size_t index) {
    RpcRequest envelope;
    envelope.ParseFromArray(transport.sent[index].data(), static_cast<int>(transport.sent[index].size()));
    CoreRequest core;
    core.ParseFromString(envelope.payload());
    return core.method();
}

} // namespace

TEST(LifecycleSecureTest, LifecycleStateWriteReadRoundTrips) {
    firebolt::secure::core::lifecycle::LifecycleState source;
    source.state = "FOREGROUND";
    firebolt::common::FireboltObject object;
    source.write(object);

    firebolt::secure::core::lifecycle::LifecycleState target;
    target.read(object);
    EXPECT_EQ(target.state, "FOREGROUND");
}

TEST(LifecycleSecureTest, InterfaceSendsAndSubscribesWithCoreMethodNames) {
    auto transport = std::make_shared<MockBufferTransport>();
    FireboltSecureClient client(transport);

    auto& lifecycle = firebolt::secure::core::lifecycle::LifecycleInterface(client);
    auto operation = lifecycle.GetLifecycleState();
    auto subscription = lifecycle.OnStateChanged([](const firebolt::secure::core::lifecycle::LifecycleState&) {});

    ASSERT_EQ(transport->sent.size(), 2u);
    EXPECT_EQ(SentMethod(*transport, 0), "lifecycle.state");
    EXPECT_EQ(SentMethod(*transport, 1), "lifecycle.onStateChanged");
    EXPECT_TRUE(operation.IsValid());
    EXPECT_TRUE(subscription.IsActive());
}

TEST(LifecycleSecureTest, GetLifecycleStateDecodesResultState) {
    auto transport = std::make_shared<MockBufferTransport>();
    FireboltSecureClient client(transport);

    auto& lifecycle = firebolt::secure::core::lifecycle::LifecycleInterface(client);
    auto operation = lifecycle.GetLifecycleState();
    ASSERT_EQ(transport->sent.size(), 1u);

    RpcRequest envelope;
    ASSERT_TRUE(envelope.ParseFromArray(transport->sent[0].data(),
                                        static_cast<int>(transport->sent[0].size())));
    CoreRequest core;
    ASSERT_TRUE(core.ParseFromString(envelope.payload()));

    CoreResponse response;
    response.set_id(core.id());
    response.set_result("{\"state\":\"FOREGROUND\"}");
    std::string corePayload;
    response.SerializeToString(&corePayload);
    transport->receiver(MakeResponse(envelope.request_id(), RPC_STATUS_OK, corePayload));

    auto result = operation.Get(std::chrono::milliseconds(100));
    ASSERT_TRUE(result.IsSuccess());
    EXPECT_EQ(result.Value().state, "FOREGROUND");
}

TEST(LifecycleSecureTest, GetLifecycleStateReturnsFailureForNonOkStatus) {
    auto transport = std::make_shared<MockBufferTransport>();
    FireboltSecureClient client(transport);

    auto& lifecycle = firebolt::secure::core::lifecycle::LifecycleInterface(client);
    auto operation = lifecycle.GetLifecycleState();
    ASSERT_EQ(transport->sent.size(), 1u);

    RpcRequest envelope;
    ASSERT_TRUE(envelope.ParseFromArray(transport->sent[0].data(),
                                        static_cast<int>(transport->sent[0].size())));
    transport->receiver(MakeResponse(envelope.request_id(), RPC_STATUS_INTERNAL, ""));

    auto result = operation.Get(std::chrono::milliseconds(100));
    EXPECT_TRUE(result.IsFailure());
}


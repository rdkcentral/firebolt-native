# Secure Path

The secure path sends Protobuf envelopes over a buffer transport (`IFireboltBufferTransport`).
`FireboltSecureClient` wraps each JSON-RPC request in an `RpcRequest` envelope and unwraps
each `RpcResponse` (for responses and events) back into JSON-RPC. The generated
`secure::core::lifecycle::Lifecycle` interface has the same API as the core path, so
application code is the same apart from the client and transport.

## Usage

```cpp
#include <chrono>
#include <iostream>

#include "firebolt/IFireboltBufferTransport.h"
#include "firebolt/secure/core/FireboltSecureClient.h"
#include "firebolt/secure/core/Lifecycle.h"

int main() {
    // Application-provided transport that moves bytes to the device (for example over a UDS socket).
    std::shared_ptr<firebolt::IFireboltBufferTransport> transport = MakeBufferTransport();
    firebolt::secure::core::FireboltSecureClient client(transport);
    firebolt::secure::core::lifecycle::Lifecycle lifecycle(client);

    auto state = lifecycle.GetLifecycleState().Get(std::chrono::milliseconds(500));
    if (state.IsSuccess()) {
        std::cout << "state: " << state.Value().state << "\n";
    }

    auto subscription = lifecycle.OnStateChanged(
        [](const firebolt::secure::core::lifecycle::LifecycleState& event) {
            std::cout << "changed: " << event.state << "\n";
        });

    return 0;
}
```

`MakeBufferTransport()` stands for your own `IFireboltBufferTransport` implementation.

## Linking

Link against `firebolt-secure-client`, `firebolt-common`, and `firebolt-proto`. Add `include/`
to the include path.

## Envelope Structure

See the [`EnvelopeWrapper`](../../../../src/secure/core/client/include/firebolt/secure/core/EnvelopeWrapper.h)
header for the envelope format.

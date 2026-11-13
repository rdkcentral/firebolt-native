# Core Path

The core path sends JSON-RPC messages over a text transport (`IFireboltTextTransport`).
`FireboltClient` correlates responses and events by JSON-RPC `id`, and the generated
`lifecycle::Lifecycle` interface exposes the typed Lifecycle API on top of it.

## Usage

```cpp
#include <chrono>
#include <iostream>

#include "firebolt/core/FireboltClient.h"
#include "firebolt/core/Lifecycle.h"
#include "firebolt/transport/ExampleTransport.h"

int main() {
    auto transport = std::make_shared<firebolt::transport::ExampleTransport>();
    firebolt::core::FireboltClient client(transport);
    firebolt::core::lifecycle::Lifecycle lifecycle(client);

    auto state = lifecycle.GetLifecycleState().Get(std::chrono::milliseconds(500));
    if (state.IsSuccess()) {
        std::cout << "state: " << state.Value().state << "\n";
    }

    auto subscription = lifecycle.OnStateChanged(
        [](const firebolt::core::lifecycle::LifecycleState& event) {
            std::cout << "changed: " << event.state << "\n";
        });

    return 0;
}
```

`ExampleTransport` echoes each sent message back to the client, so the request above
returns a failure result. Use a transport connected to a real device to receive lifecycle
values.

## Linking

Link against `firebolt-core-client` and `firebolt-common`. The example transport is
`firebolt-example-transport`. Add `include/` to the include path.

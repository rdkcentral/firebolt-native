---
module: Network
version: "9.0"
platform: both
stability: stable
description: |
  Provides access to network connectivity status.
  Apps can query network state and subscribe to connectivity changes.

properties:
  - name: connected
    description: |
      Returns the network connectivity status: true if internet-accessible,
      false otherwise.
    since: "8.0.0"
    result:
      type: bool
    examples:
      - description: Device is connected to internet
        result: true
---

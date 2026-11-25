---
module: Shared
version: "9.0"
stability: stable
description: |
  Cross-module type definitions shared across all Firebolt 9 API modules.

  These types are referenced by the OpenRPC derivation process and consumed
  by all language generators. They are not authored per-module — they live
  here and are referenced via the "shared:" prefix in other spec files.

  Types in this module are NEVER exposed directly to API consumers in generated
  headers. They are transport/contract types used internally by the pipeline:
  - ListenResponse is stripped from subscribe method results by the AST builder.
  - FireboltError is referenced by the C++ generator's FireboltResult<T> type.

types:

  - name: ListenResponse
    kind: object
    description: |
      Returned synchronously when a client subscribes to or unsubscribes from
      a Firebolt event method. This is the JSON-RPC response to the subscribe
      call itself (not the asynchronous event payload).

      The AST builder strips ListenResponse from all subscribe method result
      types. It does not appear in the Canonical AST or any generated headers.

      Wire example (subscribe confirmation):
        { "listening": true, "event": "Lifecycle2.onStateChanged" }

      Wire example (unsubscribe confirmation):
        { "listening": false, "event": "Lifecycle2.onStateChanged" }
    properties:
      - name: listening
        type: bool
        required: true
        description: |
          True if the client is now subscribed to the event.
          False if the client has just unsubscribed.
      - name: event
        type: string
        required: false
        description: |
          The fully-qualified event method name that was subscribed to or
          unsubscribed from. Format: "<Module>.<eventName>".
          Example: "Lifecycle2.onStateChanged"

  - name: FireboltError
    kind: object
    description: |
      Error carrier for failed Firebolt API calls. Matches the JSON-RPC 2.0
      error object structure to ensure wire compatibility.

      In C++ generated headers, this type is exposed as Firebolt::FireboltError
      and is the error type carried by FireboltResult<T>. It is not directly
      exposed in TypeScript, ReScript, Kotlin/JS, or Python headers — those
      languages surface errors via Promise rejection or exceptions.

      Error codes are defined by the Firebolt 9 error taxonomy:

        Code 1 — Generic / Unknown method
          The called method is not known to the version of Firebolt running
          on this device. Indicates a version mismatch.

        Code 2 — Generic / Method not permitted
          The app does not have permission to call this method on this device
          or in the current context.

        Code 3 — Generic / Generic failure
          An unclassified failure occurred.

        Code 4 — Generic / System failure
          Memory allocation failure or transport-level failure. The request
          could not be processed regardless of its content.

        Code 5 — Specific / Not implemented
          This is a non-mandatory method that is not implemented on this
          specific device. Example: SecureStorage.get on a non-Comcast device.

      Note: Error code 6 ("App state invalid") is defined in the Firebolt 9
      specification but has not been approved. Do not use code 6 in specs
      or implementations until it is marked stable.
    properties:
      - name: code
        type: unsigned
        required: true
        description: |
          Numeric Firebolt error code. Valid values: 1, 2, 3, 4, 5.
          See type description for the meaning of each code.
      - name: message
        type: string
        required: true
        description: |
          Human-readable description of the error, suitable for logging.
          Not intended for display to end users.

  - name: AgePolicy
    kind: enum
    description: |
      Age-rating classification applied by the app to content being reported.
      Used across multiple modules for consistent content maturity classification.
    values:
      - id: "app:adult"
        description: Adult content classification
      - id: "app:child"
        description: Child content classification
      - id: "app:teen"
        description: Teen content classification
---

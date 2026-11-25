---
module: Lifecycle2
version: "9.0"
platform: native
stability: stable
description: |
  Manages the lifecycle state of an app as controlled by the platform.
  Apps must subscribe to onStateChanged before the platform will transition
  them out of the initializing state.

  Note: This module is named "Lifecycle2" intentionally. The name "Lifecycle"
  is reserved by Firebolt versions prior to 9.

types:
  - name: LifecycleState
    kind: enum
    description: |
      Lifecycle state of an app managed by the platform.

      Valid transitions:
        initializing → paused | suspended
        paused → active | suspended
        active → paused
        suspended → paused | hibernated
        hibernated → suspended
        any → terminating
    values:
      - id: initializing
        description: App is starting up; first observable state
      - id: paused
        description: Foreground but not interactive
      - id: active
        description: Foreground and interactive
      - id: suspended
        description: Background
      - id: hibernated
        description: Deep background with reduced resources
      - id: terminating
        description: Being shut down; terminal state

  - name: StateChangedEvent
    kind: object
    description: |
      Payload for a lifecycle state transition notification.
      Always carries exactly one state transition.
    properties:
      - name: oldState
        $ref: "#/types/LifecycleState"
        required: true
        description: The state the app transitioned from
      - name: newState
        $ref: "#/types/LifecycleState"
        required: true
        description: The state the app transitioned to

events:
  - name: onStateChanged
    description: |
      Subscribe to lifecycle state change notifications.
      The app/runtime remains in initializing until this subscribe call is made.
      Each notification carries exactly one state transition.
    since: "8.0.0"
    payload:
      type:
        $ref: "#/types/StateChangedEvent"
    examples:
      - description: App becoming active from paused
        payload:
          oldState: paused
          newState: active
---

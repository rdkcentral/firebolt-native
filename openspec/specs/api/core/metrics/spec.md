---
module: Metrics
version: "9.0"
platform: both
stability: stable
description: |
  Provides event logging to the platform for metrics and analytics.
  Apps send telemetry events describing media playback, errors, and lifecycle.

actions:
  - name: ready
    description: Signals that the app is ready to display content and accept user input.
    since: "9.0.0"
    params: []
    result: null

  - name: signIn
    description: Log a sign in event. Platform will automatically detect the id.
    since: "9.0.0"
    platform: native
    params: []
    result: null

  - name: signOut
    description: Log a sign out event. Platform will automatically detect the id.
    since: "9.0.0"
    platform: native
    params: []
    result: null

  - name: startContent
    description: Signals the app is starting to show video or audio content with entity identification and age policy context.
    since: "9.0.0"
    params:
      - name: entityId
        type: string
        required: true
        description: Entity identifier for the media asset
      - name: agePolicy
        type:
          $ref: "shared:AgePolicy"
        required: true
        description: Age policy for the content
    result: null

  - name: stopContent
    description: Signals the app has stopped showing video or audio content with entity identification and age policy context.
    since: "9.0.0"
    params:
      - name: entityId
        type: string
        required: true
        description: Entity identifier for the media asset
      - name: agePolicy
        type:
          $ref: "shared:AgePolicy"
        required: true
        description: Age policy for the content
    result: null

  - name: page
    description: |
      Signals that the app has transitioned to a new page with page identification and optional age policy context.
    since: "9.0.0"
    params:
      - name: pageId
        type: string
        required: true
        description: Identifier for the page the app is now on
      - name: agePolicy
        type:
          $ref: "shared:AgePolicy"
        required: false
        description: Age policy for content classification

  - name: error
    description: Logs an error that occurred within the app with structured error information including type, code, description, visibility, optional parameters, and optional age policy.
    since: "9.0.0"
    params:
      - name: type
        type:
          $ref: "#/types/ErrorType"
        required: true
        description: Type of error
      - name: code
        type: string
        required: true
        description: Error code
      - name: description
        type: string
        required: true
        description: Human-readable error description
      - name: visible
        type: bool
        required: true
        description: Whether error should be visible to user
      - name: parameters
        type: object
        required: false
        description: Additional error parameters as key-value pairs
      - name: agePolicy
        type:
          $ref: "shared:AgePolicy"
        required: false
        description: Age policy for content context

  - name: mediaLoadStart
    description: Called when setting the URL of a media asset to play, in order to infer load time.
    since: "9.0.0"
    params:
      - name: entityId
        type: string
        required: true
        description: Entity identifier for the media asset
      - name: agePolicy
        type:
          $ref: "shared:AgePolicy"
        required: false
        description: Age policy for the content
    result: null

  - name: mediaPlay
    description: Called when media playback should start due to autoplay, user-initiated play, or unpausing.
    since: "9.0.0"
    params:
      - name: entityId
        type: string
        required: true
        description: Entity identifier for the media asset
      - name: agePolicy
        type:
          $ref: "shared:AgePolicy"
        required: false
        description: Age policy for the content
    result: null

  - name: mediaPlaying
    description: |
      Called when media playback actually starts due to autoplay, user-initiated play, unpausing, or recovering from a buffering interruption.
    since: "9.0.0"
    params:
      - name: entityId
        type: string
        required: true
        description: Entity identifier for the media asset
      - name: agePolicy
        type:
          $ref: "shared:AgePolicy"
        required: false
        description: Age policy for the content
    result: null

  - name: mediaPause
    description: Signals that media playback has been paused.
    since: "9.0.0"
    params:
      - name: entityId
        type: string
        required: true
        description: Entity identifier for the media asset
      - name: agePolicy
        type:
          $ref: "shared:AgePolicy"
        required: false
        description: Age policy for the content
    result: null

  - name: mediaWaiting
    description: Signals that media playback is waiting (buffering) for data.
    since: "9.0.0"
    params:
      - name: entityId
        type: string
        required: true
        description: Entity identifier for the media asset
      - name: agePolicy
        type:
          $ref: "shared:AgePolicy"
        required: false
        description: Age policy for the content
    result: null

  - name: mediaSeeking
    description: Signals that the user is seeking to a new playback position with entity identification, target position, and optional age policy.
    since: "9.0.0"
    params:
      - name: entityId
        type: string
        required: true
        description: Entity identifier for the media asset
      - name: target
        type: number
        required: true
        description: Target playback position in seconds
      - name: agePolicy
        type:
          $ref: "shared:AgePolicy"
        required: false
        description: Age policy for content classification
    result: null

  - name: mediaSeeked
    description: Signals that the user has completed seeking to a new position with entity identification, final position, and optional age policy.
    since: "9.0.0"
    params:
      - name: entityId
        type: string
        required: true
        description: Entity identifier for the media asset
      - name: position
        type: number
        required: true
        description: Final playback position in seconds
      - name: agePolicy
        type:
          $ref: "shared:AgePolicy"
        required: false
        description: Age policy for content classification
    result: null

  - name: mediaRateChanged
    description: Signals that media playback rate has changed with entity identification, rate value, and optional age policy.
    since: "9.0.0"
    params:
      - name: entityId
        type: string
        required: true
        description: Entity identifier for the media asset
      - name: rate
        type: number
        required: true
        description: Playback rate multiplier (e.g., 2.0 for 2x speed)
      - name: agePolicy
        type:
          $ref: "shared:AgePolicy"
        required: false
        description: Age policy for content classification
    result: null

  - name: mediaRenditionChanged
    description: Signals that media rendition (quality) has changed with entity identification, bitrate, dimensions, optional profile, and optional age policy.
    since: "9.0.0"
    params:
      - name: entityId
        type: string
        required: true
        description: Entity identifier for the media asset
      - name: bitrate
        type: number
        required: true
        description: Bitrate in bits per second
      - name: width
        type: number
        required: true
        description: Video width in pixels
      - name: height
        type: number
        required: true
        description: Video height in pixels
      - name: profile
        type: string
        required: false
        description: Quality profile identifier
      - name: agePolicy
        type:
          $ref: "shared:AgePolicy"
        required: false
        description: Age policy for content classification
    result: null

  - name: mediaEnded
    description: Signals that media playback has ended.
    since: "9.0.0"
    params:
      - name: entityId
        type: string
        required: true
        description: Entity identifier for the media asset
      - name: agePolicy
        type:
          $ref: "shared:AgePolicy"
        required: false
        description: Age policy for the content
    result: null

  - name: event
    description: |
      Generic event telemetry allowing apps to log custom metrics and analytics events with schema URI, data string, and optional age policy.
    since: "9.0.0"
    params:
      - name: schema
        type: string
        required: true
        description: Schema URI for the event data format
      - name: data
        type: string
        required: true
        description: Event data as a string
      - name: agePolicy
        type:
          $ref: "shared:AgePolicy"
        required: false
        description: Age policy for content classification

  - name: appInfo
    description: |
      Sends app-specific metrics to the platform with a build identifier.
    since: "9.0.0"
    params:
      - name: build
        type: object
        required: true
        properties:
          - name: build
            type: string
            required: true
            description: Build identifier for the app
        description: Build identifier for the app

types:
  - name: ErrorType
    kind: enum
    description: Classification of error type.
    values:
      - id: "network"
        description: Network-related error
      - id: "media"
        description: Media playback error
      - id: "restriction"
        description: Content restriction error
      - id: "entitlement"
        description: Content entitlement/auth error
      - id: "other"
        description: Other or uncategorized error
---

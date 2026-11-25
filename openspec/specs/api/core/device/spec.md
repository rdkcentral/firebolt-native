---
module: Device
version: "9.0"
platform: both
stability: stable
description: |
  Provides access to device information and capabilities.
  Apps can query device identifiers, class, and display capabilities.

actions:
  - name: uid
    description: Returns a persistent unique UUID for the current app and device.
    since: "8.0.0"
    result:
      type: string
      description: UUID that resets when app or device is reset
    examples:
      - description: Persistent app+device UUID
        result: "550e8400-e29b-41d4-a716-446655440000"

  - name: deviceClass
    description: Returns the class of the device.
    since: "9.0.0"
    result:
      $ref: "#/types/DeviceClass"
    examples:
      - description: Set-top box
        result: "stb"

  - name: uptime
    description: Returns the number of seconds since most recent device boot, including any time spent during deep sleep.
    since: "9.0.0"
    platform: native
    params: []
    result:
      type: number
    examples:
      - description: Device uptime
        result: 86400

  - name: brandName
    description: Returns the brand name under which the device was marketed to consumers. Typically also shown on the TV bezel, device label or remote either "" (if not initialized) 1 or more characters.
    since: "9.0.0"
    params: []
    result:
      type: string
    examples:
      - description: Device brand
        result: "Acme"

  - name: modelId
    description: Returns the model identifier assigned to the device hardware. Typically also shown on the device label or UI.
    since: "9.0.0"
    params: []
    result:
      type: string
    examples:
      - description: Device model
        result: "ABC123"

  - name: osName
    description: Returns the operating system name as defined by the operator.
    since: "9.0.0"
    params: []
    result:
      type: string
    examples:
      - description: Operating system name
        result: "FireboltOS"

  - name: osVersion
    description: Returns the operating system version as defined by the operator.
    since: "9.0.0"
    params: []
    result:
      type: string
    examples:
      - description: Operating system version
        result: "9.0.0"

  - name: firmware
    description: Returns a string that identifies the firmware image of the device.
    since: "9.0.0"
    params: []
    result:
      type: string
    examples:
      - description: Firmware version
        result: "1.2.3"

properties:
  - name: hdr
    description: |
      Returns the HDR standards supported by the attached TV or integrated display.
    since: "8.0.0"
    result:
      $ref: "#/types/HdrCapabilities"
    examples:
      - description: HDR10 and Dolby Vision supported
        result:
          hdr10: true
          hdr10Plus: false
          dolbyVision: true
          hlg: false

  - name: dolbyAtmosExperienceAvailable
    description: |
      Returns whether the user would get a Dolby Atmos experience
      if a Dolby Atmos track were to be played at this time.
    since: "9.0.0"
    result:
      type: bool
    examples:
      - description: Dolby Atmos available
        result: true

  - name: name
    description: Returns the device friendly name. Used by network services (DIAL, Miracast, AirPlay) so that other devices can more easily identify this device during device discovery.
    since: "9.0.0"
    result:
      type: string
    examples:
      - description: Device friendly name
        result: "Living Room TV"

events:
  - name: onNameChanged
    description: Event for when Device.name changed.
    since: "9.0.0"
    params: []
    result:
      type: string
    examples:
      - description: Device name changed
        result: "Bedroom TV"

types:
  - name: DeviceClass
    kind: enum
    description: The class of the device (string enum).
    values:
      - id: "ott"
        description: No tuner/demod, no integrated display
      - id: "stb"
        description: With tuner/demod, no integrated display
      - id: "tv"
        description: Possibly tuner/demod, with integrated display

  - name: HdrCapabilities
    kind: object
    description: HDR standards supported by the device.
    properties:
      - name: hdr10
        type: bool
        required: true
        description: HDR10 support
      - name: hdr10Plus
        type: bool
        required: true
        description: HDR10+ support
      - name: dolbyVision
        type: bool
        required: true
        description: Dolby Vision support
      - name: hlg
        type: bool
        required: true
        description: HLG support
---

---
module: VideoOutput
version: "9.0"
platform: both
stability: stable
description: |
  Provides access to video output configuration.
  Apps can query and subscribe to video output resolution changes.

properties:
  - name: resolution
    description: |
      Returns the width and height of the video signal on the video output, in pixels.
      Typically used by a streaming app to determine the highest resolution of video to select.
      OTT/STB device: returns the video resolution over HDMI.
      TV device: returns the resolution of the panel.
    since: "8.0.0"
    result:
      $ref: "#/types/VideoResolution"
    examples:
      - description: Device output resolution is 4K
        result:
          width: 3840
          height: 2160

  - name: hdcp
    description: |
      Returns the current state of output protection on the video output.
      OTT/STB device: returns the negotiated HDCP version on the video output, or none if an encrypted connection has not been made between the OTT/STB device and any attached TV.
      TV device: returns direct.
    since: "9.0.0"
    result:
      $ref: "#/types/HdcpType"
    examples:
      - description: HDCP 2.2 negotiated
        result: "hdcp2.2"

events:
  - name: onResolutionChanged
    description: Event for when VideoOutput.resolution changed.
    since: "8.0.0"
    params: []
    result:
      $ref: "#/types/VideoResolution"
    examples:
      - description: Resolution changed to 1080p
        result:
          width: 1920
          height: 1080

  - name: onHdcpChanged
    description: |
      Event for when VideoOutput.hdcp changed.
      Returns the current state of output protection on the video output.
      OTT/STB device: returns the negotiated HDCP version on the video output, or none if an encrypted connection has not been made between the OTT/STB device and any attached TV.
      TV device: returns direct.
    since: "9.0.0"
    params: []
    result:
      $ref: "#/types/HdcpType"
    examples:
      - description: HDCP state changed
        result: "hdcp1.4"

types:
  - name: VideoResolution
    description: |
      Video resolution output in pixels (object with width and height).
      Allowed width/height combinations: (720, 480), (720, 576), (1280, 720), (1920, 1080), (3840, 2160).
    properties:
      - name: width
        type: unsigned
        description: The width of the video resolution in pixels
      - name: height
        type: unsigned
        description: The height of the video resolution in pixels

  - name: HdcpType
    kind: enum
    description: HDCP output protection state
    values:
      - id: "hdcp1.4"
        description: HDCP 1.4 negotiated
      - id: "hdcp2.2"
        description: HDCP 2.2 negotiated
      - id: "none"
        description: No encrypted connection established
      - id: "direct"
        description: Direct connection (TV device)
---

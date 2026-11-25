---
module: Display
version: "9.0"
platform: both
stability: stable
description: |
  Provides access to display capabilities.
  Some APIs are web-only, some are native-only, some are both.

actions:
  - name: colorimetry
    description: |
      Returns an unordered list of colorimetry values supported by the attached TV or integral display.
      Returns an empty list if no TV is attached.
    since: "9.0.0"
    platform: web
    result:
      type: array
      items:
        $ref: "#/types/ColorimetryValue"
    examples:
      - description: Display supports BT.709 and BT.2020
        result:
          - "bt709"
          - "bt2020"

  - name: videoResolutions
    description: |
      Returns an unordered list of HD video resolutions and frame rates supported by the attached TV or integral display.
      Returns an empty list if no TV is attached.
    since: "9.0.0"
    platform: web
    result:
      type: array
      items:
        $ref: "#/types/VideoResolution"
    examples:
      - description: Display supports 1080p60 and 2160p60
        result:
          - "1080p60"
          - "2160p60"

types:
  - name: ColorimetryValue
    kind: enum
    description: Colorimetry standard.
    values:
      - id: "bt709"
        description: BT.709 colorimetry
      - id: "bt2020"
        description: BT.2020 colorimetry

  - name: VideoResolution
    kind: enum
    description: HD video resolution and frame rate.
    values:
      - id: "720p50"
        description: 720p at 50Hz
      - id: "720p60"
        description: 720p at 60Hz
      - id: "1080p50"
        description: 1080p at 50Hz
      - id: "1080p60"
        description: 1080p at 60Hz
      - id: "2160p50"
        description: 2160p at 50Hz
      - id: "2160p60"
        description: 2160p at 60Hz
---

---
module: Advertising
version: "9.0"
platform: both
stability: stable
description: |
  Provides access to advertising identifiers.
  Apps can query the device's IFA and related metadata.

actions:
  - name: advertisingId
    description: Returns the IFA (Identifier For Advertising) and related metadata.
    since: "8.0.0"
    result:
      $ref: "#/types/AdvertisingId"
    examples:
      - description: IFA with DPID and LMT enabled
        result:
          ifa: "550e8400-e29b-41d4-a716-446655440000"
          ifa_type: "dpid"
          lmt: "1"

types:
  - name: AdvertisingId
    description: Advertising identifiers and metadata.
    properties:
      - name: ifa
        type: string
        description: The Identifier For Advertising as a UUID
      - name: ifa_type
        type:
          $ref: "#/types/IfaType"
      - name: lmt
        type:
          $ref: "#/types/Lmt"

  - name: IfaType
    kind: enum
    description: The source of the IFA.
    values:
      - id: "dpid"
        description: Device-provided ID
      - id: "sspid"
        description: SSP (Supply-Side Platform) provided ID
      - id: "sessionid"
        description: Session/synthetic ID

  - name: Lmt
    kind: enum
    description: Limit Ad Tracking setting.
    values:
      - id: "0"
        description: Limit ad tracking disabled
      - id: "1"
        description: Limit ad tracking enabled
---

---
module: Localization
version: "9.0"
platform: both
stability: stable
description: |
  Provides access to locale and regional settings managed by the platform.
  Apps subscribe to events to receive real-time updates when settings change.

properties:
  - name: country
    description: |
      Returns the country setting.
      The payload is an ISO 3166-1 alpha-2 country code (e.g. "US", "GB", "DE").
      Corresponds to the Country field in the Devices table.
    since: "8.0.0"
    result:
      type: string
      minLength: 2
      maxLength: 2
      pattern: "^[A-Z]{2}$"
      description: ISO 3166-1 alpha-2 country code
    examples:
      - description: Country is United States
        result: "US"

  - name: preferredAudioLanguages
    description: |
      Returns the list of preferred audio languages.
      A list of zero or more languages in order of decreasing preference.
      Each code is an ISO 639-2/B language code.
    since: "8.0.0"
    result:
      type: array
      items: string
      description: ISO 639-2/B language codes
    examples:
      - description: English and Spanish preferences
        result:
          - "eng"
          - "spa"

  - name: presentationLanguage
    description: |
      The presentation language of the device, e.g. en-US
    since: "8.0.0"
    result:
      type: string
      description: BCP 47 locale tag
    examples:
      - description: US English
        result: "en-US"

  - name: timeZone
    description: Returns the IANA time zone format.
    since: "9.0.0"
    platform: native
    result:
      type: string
      description: IANA time zone format (e.g., "America/New_York", "Europe/London")
    examples:
      - description: New York time zone
        result: "America/New_York"

events:
  - name: onTimeZoneChanged
    description: Event for when Localization.timeZone changed.
    since: "9.0.0"
    platform: native
    params: []
    result:
      type: string
    examples:
      - description: Time zone changed
        result: "Europe/London"
---

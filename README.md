# MirrorClock-Firmware
> [!NOTE]  
> Im currently waiting for permision from Bastelgarage.ch to upload.
## Overview
MirrorClock-Firmware is a firmware designed for the "23-er Platine" from Bastelgarage.ch. This firmware comes in two versions: a **standard** version and an **MQTT** version. It addresses efficiency issues and resolves errors, such as the random restarts observed in the original firmware on some ESP8266 devices.

## Versions
### Standard Version
This version includes all the features present in the original firmware, but with improved code structure and efficiency.

### MQTT Version
The MQTT version builds upon the standard version and introduces additional capabilities for seamless integration with MQTT (Message Queuing Telemetry Transport) protocols. Some key features of the MQTT version include:

- **Sensor Reading:** Retrieve data from all sensors over MQTT.
- **Brightness Control:** Set the brightness of the device remotely via MQTT.
- **Color Configuration:** Adjust the color settings remotely using MQTT.
- **Power Management:** Turn the device on and off remotely through MQTT.
- **Note:** The MQTT version does not include Infrared (IR) support.

## Development Status
Please note that MirrorClock-Firmware is currently in development, and not all features have been fully implemented. The firmware may exhibit some bugs at this stage. However, we are actively working on refining the codebase and addressing any issues.

## Planned Completion
We aim to finalize and release a stable version of MirrorClock-Firmware by February 2024. Your feedback and contributions are welcome as we work towards delivering a reliable and feature-rich firmware for the "23-er Platine."

## Getting Started
To use MirrorClock-Firmware, follow the installation instructions provided in the respective version folders. Additionally, check the documentation for any updates or changes.

## Contribution
If you encounter issues or have suggestions for improvement, please feel free to open an issue or submit a pull request. Your contributions will play a crucial role in enhancing the overall quality of MirrorClock-Firmware.

Thank you for your interest in MirrorClock-Firmware!

# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a PlatformIO project for the **LilyGO T-A7670G R2** development board (Model Q425) featuring:
- ESP32-WROVER-E microcontroller with 4MB Flash and 8MB PSRAM
- A7670G 4G LTE CAT1 modem
- L76K external GPS module (not built-in to modem)
- Serial communication at 115200 baud (modem) and 9600 baud (GPS)

## Build and Development Commands

### Building and Uploading
```bash
# Build the project
pio run

# Upload to board
pio run --target upload

# Monitor serial output (115200 baud)
pio device monitor

# Build, upload, and monitor in one command
pio run --target upload && pio device monitor

# Clean build files
pio run --target clean
```

### Library Management
The project uses local copies of libraries in the `lib/` directory:
- **TinyGSM**: Forked version specifically for LilyGO boards - do not replace with standard library
- **TinyGPSPlus**: GPS NMEA parsing
- **StreamDebugger**: Serial debugging utilities
- **SensorLib**: Sensor support (includes BMA423, BMM150, etc.)

Note: `platformio.ini` references standard libraries but the local `lib/` versions take precedence.

## Hardware Architecture

### Critical Pin Definitions
All pin assignments are defined in [src/main.cpp](src/main.cpp):

**Modem (A7670G)**:
- TX: GPIO 26, RX: GPIO 27
- Power: GPIO 4, Reset: GPIO 5
- DTR: GPIO 25, RI: GPIO 33

**GPS Module (L76K External)**:
- TX: GPIO 21, RX: GPIO 22
- PPS: GPIO 23, Wakeup: GPIO 19

**Power Management**:
- BOARD_POWER_ON_PIN (GPIO 12) must be HIGH for battery operation

**SD Card** (if needed):
- MISO: GPIO 2, MOSI: GPIO 15, SCLK: GPIO 14, CS: GPIO 13

### Serial Communication
- `Serial`: USB debug output (115200 baud)
- `Serial1` (SerialAT): Modem AT commands (115200 baud)
- `Serial2` (SerialGPS): GPS NMEA data (9600 baud)

## Important Implementation Notes

1. **GPS Module**: The A7670G modem does NOT have built-in GPS. This board uses an external L76K GPS module connected via Serial2. Do not attempt to use AT+GPS commands with the modem.

2. **Power Sequence**: The modem requires a specific power-on sequence (LOW→HIGH→LOW pulse on GPIO 4) implemented in [src/main.cpp:100-105](src/main.cpp#L100-L105).

3. **TinyGSM Library**: Uses a forked version for LilyGO compatibility. When working with modem functionality, maintain compatibility with the local fork.

4. **GPS Data Flow**: GPS data is read from Serial2, parsed by TinyGPSPlus, and displayed when valid sentences are decoded. GPS acquisition requires outdoor location and may take 5-10 minutes for initial fix.

5. **AT Command Passthrough**: The main loop includes AT command passthrough between USB serial and modem serial for debugging (lines 146-151).

## Development Environment

- Platform: ESP32 (espressif32@6.11.0)
- Framework: Arduino
- Build flags include PSRAM support and cache issue fix
- Monitor includes ESP32 exception decoder filter

## Common Tasks

### Adding Modem Functionality
Refer to TinyGSM examples in `lib/TinyGSM/examples/` for HTTP, MQTT, and network operations. The modem uses the A7670 client variant.

### Debugging GPS Issues
Uncomment line 133 in [src/main.cpp](src/main.cpp#L133) to view raw NMEA sentences for GPS debugging.

### Working with Sensors
SensorLib examples in `lib/SensorLib/examples/` demonstrate usage of supported sensors (accelerometer, magnetometer, etc.).
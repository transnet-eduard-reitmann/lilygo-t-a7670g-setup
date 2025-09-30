# LilyGO T-A7670G R2 GPS Development Setup

Development environment for the LilyGO T-A7670G R2 with GPS (Model Q425) using PlatformIO.

## Hardware Specifications

- **Board**: LilyGO T-A7670G R2 (Model Q425)
- **Microcontroller**: ESP32-WROVER-E
- **Flash**: 4MB
- **PSRAM**: 8MB
- **Modem**: A7670G (4G LTE CAT1)
- **GPS**: L76K External GPS Module
- **Product Page**: [LilyGO Store](https://lilygo.cc/products/t-sim-a7670e?variant=43043706077365)

## Prerequisites

1. **Install USB Drivers**
   - Download and install CH9102 USB bridge driver from [WCH-IC website](http://www.wch-ic.com/search?q=CH9102&t=downloads)
   - Required for USB-C communication with the board

2. **Development Environment**
   - [Visual Studio Code](https://code.visualstudio.com/)
   - [Python](https://www.python.org/downloads/) (required by PlatformIO)
   - [PlatformIO IDE extension](https://platformio.org/install/ide?install=vscode)

## Hardware Setup

1. **Before Power On**:
   - Insert your SIM card into the SIM card slot
   - Connect GPS antenna to GPS port
   - Connect LTE antenna to LTE port

2. **Connection**:
   - Connect board to computer via USB-C cable
   - Board will power on automatically

## Pin Configuration

### Modem Pins
- TX: GPIO 26
- RX: GPIO 27
- Power: GPIO 4
- Reset: GPIO 5
- DTR: GPIO 25
- RI: GPIO 33

### GPS Module Pins (L76K)
- TX: GPIO 21
- RX: GPIO 22
- PPS: GPIO 23
- Wakeup: GPIO 19

### Other Pins
- Battery ADC: GPIO 35
- I2C SDA: GPIO 21
- I2C SCL: GPIO 22

## Getting Started

### 1. Open Project in PlatformIO

```bash
cd lilygo-t-a7670g-setup
code .
```

### 2. Build the Project

In VS Code:
- Press `Ctrl+Shift+P` (Windows/Linux) or `Cmd+Shift+P` (Mac)
- Type "PlatformIO: Build" and press Enter

Or use the PlatformIO toolbar button.

### 3. Upload to Board

In VS Code:
- Press `Ctrl+Shift+P` (Windows/Linux) or `Cmd+Shift+P` (Mac)
- Type "PlatformIO: Upload" and press Enter

Or use the PlatformIO toolbar button.

### 4. Monitor Serial Output

In VS Code:
- Press `Ctrl+Shift+P` (Windows/Linux) or `Cmd+Shift+P` (Mac)
- Type "PlatformIO: Monitor" and press Enter

Or use the PlatformIO toolbar button.

**Monitor Settings**: 115200 baud

## Project Structure

```
lilygo-t-a7670g-setup/
├── src/
│   └── main.cpp          # Main application code
├── lib/
│   ├── TinyGSM/          # TinyGSM library (forked for LilyGO)
│   ├── TinyGPSPlus/      # GPS parsing library
│   ├── StreamDebugger/   # Debug utilities
│   └── SensorLib/        # Sensor support library
├── include/              # Header files
├── platformio.ini        # PlatformIO configuration
└── README.md            # This file
```

## Features

The included example demonstrates:
- GPS functionality with L76K external GPS module
- Serial communication with A7670G modem
- GPS data parsing (location, date, time)
- AT command passthrough

## Important Notes

### GPS Functionality
- **A7670G uses EXTERNAL GPS** (L76K module), not built-in GPS
- GPS signal acquisition may take several minutes outdoors
- Ensure GPS antenna is connected and positioned with clear sky view
- Initial fix requires open sky visibility

### Modem Compatibility
- This project uses a **forked version of TinyGSM** specifically for LilyGO boards
- Do not replace with standard TinyGSM library

### Power Considerations
- When using battery power, BOARD_POWER_ON_PIN (GPIO 12) must be HIGH
- Board includes power management for optimal battery life

## Troubleshooting

### GPS Not Working
- Verify GPS antenna is properly connected
- Ensure you're outdoors with clear sky view
- Wait at least 5-10 minutes for initial GPS fix
- Check serial output for GPS NMEA sentences

### Upload Fails
- Ensure CH9102 driver is installed
- Check USB cable supports data transfer (not charge-only)
- Try different USB port
- Press and hold BOOT button during upload if needed

### Modem Not Responding
- Verify SIM card is inserted
- Check antenna connections
- Review serial monitor for AT command responses
- Verify modem power sequence completed

## Additional Examples

The LilyGO repository includes numerous examples:
- Network connectivity (HTTP, MQTT)
- SMS functionality
- GPS tracking applications
- Sensor integration

See: [LilyGO Modem Series Examples](https://github.com/Xinyuan-LilyGO/LilyGo-Modem-Series/tree/main/examples)

## Resources

- [Official GitHub Repository](https://github.com/Xinyuan-LilyGO/LilyGo-Modem-Series/)
- [Product Page](https://lilygo.cc/products/t-sim-a7670e?variant=43043706077365)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [TinyGSM Documentation](https://github.com/vshymanskyy/TinyGSM)

## Licence

This project uses libraries and examples from LilyGO licensed under MIT Licence.

## Support

For issues and questions:
- Check existing [GitHub Issues](https://github.com/Xinyuan-LilyGO/LilyGo-Modem-Series/issues)
- Review LilyGO documentation
- Verify modem firmware is up to date
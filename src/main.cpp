/**
 * @file      main.cpp
 * @author    LilyGO T-A7670G R2 GPS Setup
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2025-09-30
 * @note      This sketch is for T-A7670G R2 with L76K GPS Module (Model Q425)
 *            A7670G uses external GPS, not built-in GPS functionality
 */

// TinyGSM modem type defined in platformio.ini build_flags

#include <Arduino.h>
#include <TinyGPS++.h>
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

// Serial definitions
#ifndef SerialAT
#define SerialAT Serial1
#endif

#ifndef SerialGPS
#define SerialGPS Serial2
#endif

// Pin definitions for T-A7670G
#define BOARD_MODEM_DTR_PIN                 25
#define BOARD_MODEM_TX_PIN                  26
#define BOARD_MODEM_RX_PIN                  27
#define BOARD_MODEM_PWR_PIN                 4
#define BOARD_ADC_PIN                       35
#define BOARD_POWER_ON_PIN                  12
#define BOARD_MODEM_RI_PIN                  33
#define BOARD_RST_PIN                       5

// SD Card pins (if needed)
#define BOARD_SDCARD_MISO                   2
#define BOARD_SDCARD_MOSI                   15
#define BOARD_SDCARD_SCLK                   14
#define BOARD_SDCARD_CS                     13

// GPS Module pins (L76K)
#define BOARD_GPS_TX_PIN                    21
#define BOARD_GPS_RX_PIN                    22
#define BOARD_GPS_PPS_PIN                   23
#define BOARD_GPS_WAKEUP_PIN                19

// Button pin
#define BOARD_BOOT_PIN                      0

// Network credentials - Vodacom South Africa
const char apn[] = "internet";  // Vodacom SA APN
const char gprsUser[] = "";     // No username required
const char gprsPass[] = "";     // No password required

// SIM card PIN (set to NULL or "" if no PIN required)
const char simPIN[] = "0000";

// TinyGSM and HTTP client
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
HttpClient http(client, "httpbin.org", 80);

// GPS
TinyGPSPlus gps;

// Timing and state variables
unsigned long lastGpsDisplay = 0;
unsigned long lastButtonCheck = 0;
bool lastButtonState = HIGH;
bool buttonPressed = false;

void displayInfo()
{
    Serial.print(F("GPS: "));

    // Location
    if (gps.location.isValid()) {
        Serial.print(gps.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(gps.location.lng(), 6);
    } else {
        Serial.print(F("NO_FIX"));
    }
    Serial.print(F(" | "));

    // Altitude
    Serial.print(F("Alt:"));
    if (gps.altitude.isValid()) {
        Serial.print(gps.altitude.meters(), 1);
        Serial.print(F("m"));
    } else {
        Serial.print(F("N/A"));
    }
    Serial.print(F(" | "));

    // Speed
    Serial.print(F("Spd:"));
    if (gps.speed.isValid()) {
        Serial.print(gps.speed.kmph(), 1);
        Serial.print(F("km/h"));
    } else {
        Serial.print(F("N/A"));
    }
    Serial.print(F(" | "));

    // Course
    Serial.print(F("Crs:"));
    if (gps.course.isValid()) {
        Serial.print(gps.course.deg(), 0);
        Serial.print(F("\xC2\xB0"));
    } else {
        Serial.print(F("N/A"));
    }
    Serial.print(F(" | "));

    // Date/Time
    if (gps.date.isValid() && gps.time.isValid()) {
        Serial.print(gps.date.year());
        Serial.print(F("-"));
        if (gps.date.month() < 10) Serial.print(F("0"));
        Serial.print(gps.date.month());
        Serial.print(F("-"));
        if (gps.date.day() < 10) Serial.print(F("0"));
        Serial.print(gps.date.day());
        Serial.print(F(" "));
        if (gps.time.hour() < 10) Serial.print(F("0"));
        Serial.print(gps.time.hour());
        Serial.print(F(":"));
        if (gps.time.minute() < 10) Serial.print(F("0"));
        Serial.print(gps.time.minute());
        Serial.print(F(":"));
        if (gps.time.second() < 10) Serial.print(F("0"));
        Serial.print(gps.time.second());
    } else {
        Serial.print(F("NO_TIME"));
    }
    Serial.print(F(" | "));

    // Satellites
    Serial.print(F("Sats:"));
    if (gps.satellites.isValid()) {
        Serial.print(gps.satellites.value());
    } else {
        Serial.print(F("0"));
    }
    Serial.print(F(" | "));

    // HDOP
    Serial.print(F("HDOP:"));
    if (gps.hdop.isValid()) {
        Serial.print(gps.hdop.hdop(), 1);
    } else {
        Serial.print(F("N/A"));
    }

    Serial.println();
}

void initModem()
{
    Serial.println(F("Initializing modem..."));

    // Restart modem
    Serial.print(F("Restarting modem... "));
    if (!modem.restart()) {
        Serial.println(F("FAILED"));
        Serial.println(F("Trying to continue anyway..."));
    } else {
        Serial.println(F("OK"));
    }

    // Wait for modem to be ready
    delay(3000);

    // Get modem info
    Serial.print(F("Modem: "));
    String modemInfo = modem.getModemInfo();
    Serial.println(modemInfo);
}

void testInternetConnectivity()
{
    Serial.println();
    Serial.println(F("========================================"));
    Serial.println(F("Starting Internet Connectivity Test..."));
    Serial.println(F("========================================"));

    // Initialize modem if not already done
    if (!modem.isNetworkConnected()) {
        initModem();
    }

    // Check SIM card status
    Serial.print(F("SIM card: "));
    int simStatus = modem.getSimStatus();

    if (simStatus == 0) {
        Serial.println(F("ERROR - No SIM card detected!"));
        Serial.println(F("========================================"));
        return;
    }

    // Unlock SIM with PIN if required
    if (simStatus == 2) {
        Serial.print(F("Locked, unlocking with PIN... "));
        if (modem.simUnlock(simPIN)) {
            Serial.println(F("OK"));
        } else {
            Serial.println(F("FAILED - Check PIN code"));
            Serial.println(F("========================================"));
            return;
        }
    } else if (simStatus == 3) {
        Serial.println(F("ERROR - SIM PUK required!"));
        Serial.println(F("========================================"));
        return;
    } else {
        Serial.println(F("OK (Ready)"));
    }

    // Check firmware version
    Serial.print(F("Firmware: "));
    modem.sendAT("+SIMCOMATI");
    modem.waitResponse(1000L);
    Serial.println();

    // Wait for network
    Serial.print(F("Network: Waiting for registration... "));
    if (!modem.waitForNetwork(30000L)) {
        Serial.println(F("FAILED"));
        Serial.println(F("========================================"));
        return;
    }
    Serial.println(F("OK"));

    // Check signal quality
    Serial.print(F("Signal: "));
    int16_t sq = modem.getSignalQuality();
    Serial.print(sq);
    Serial.println(F(" (0-31, higher is better)"));

    // Connect to GPRS
    Serial.print(F("GPRS: Connecting to \""));
    Serial.print(apn);
    Serial.print(F("\"... "));
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        Serial.println(F("FAILED"));
        Serial.println(F("Note: Update APN in code for your carrier"));
        Serial.println(F("========================================"));
        return;
    }
    Serial.println(F("OK"));

    // Check IP address
    Serial.print(F("IP Address: "));
    IPAddress local = modem.localIP();
    Serial.println(local);

    // HTTP GET test
    Serial.println();
    Serial.println(F("HTTP GET Test: http://httpbin.org/get"));
    Serial.print(F("Connecting... "));

    int err = http.get("/get");
    if (err != 0) {
        Serial.print(F("FAILED (error: "));
        Serial.print(err);
        Serial.println(F(")"));
    } else {
        int status = http.responseStatusCode();
        Serial.print(F("Status: "));
        Serial.println(status);

        if (status == 200) {
            Serial.println(F("SUCCESS - Internet is working!"));

            // Read first 500 chars of response
            String body = http.responseBody();
            if (body.length() > 500) body = body.substring(0, 500) + "...";
            Serial.println(F("Response:"));
            Serial.println(body);
        } else {
            Serial.println(F("Unexpected status code"));
        }
    }

    http.stop();

    // Disconnect GPRS
    Serial.println();
    Serial.print(F("Disconnecting GPRS... "));
    modem.gprsDisconnect();
    Serial.println(F("Done"));

    Serial.println(F("========================================"));
    Serial.println();
}

void setup()
{
    // Power on the board
    pinMode(BOARD_POWER_ON_PIN, OUTPUT);
    digitalWrite(BOARD_POWER_ON_PIN, HIGH);

    // Reset modem
    pinMode(BOARD_RST_PIN, OUTPUT);
    digitalWrite(BOARD_RST_PIN, LOW);

    // Power on modem
    pinMode(BOARD_MODEM_PWR_PIN, OUTPUT);
    digitalWrite(BOARD_MODEM_PWR_PIN, LOW);
    delay(100);
    digitalWrite(BOARD_MODEM_PWR_PIN, HIGH);
    delay(1000);
    digitalWrite(BOARD_MODEM_PWR_PIN, LOW);

    // Initialize button
    pinMode(BOARD_BOOT_PIN, INPUT_PULLUP);

    // Initialise serial ports
    Serial.begin(115200);
    SerialAT.begin(115200, SERIAL_8N1, BOARD_MODEM_RX_PIN, BOARD_MODEM_TX_PIN);
    SerialGPS.begin(9600, SERIAL_8N1, BOARD_GPS_RX_PIN, BOARD_GPS_TX_PIN);

    delay(2000);

    Serial.println("==============================================");
    Serial.println("LilyGO T-A7670G R2 with GPS (Model Q425)");
    Serial.println("==============================================");
    Serial.println();
    Serial.println("Board: T-A7670G R2");
    Serial.println("Modem: A7670G (4G LTE CAT1)");
    Serial.println("GPS: L76K External GPS Module");
    Serial.println();
    Serial.println("Press BOOT button to test internet connectivity");
    Serial.println();
    Serial.println("AT Commands Passthrough: Active");
    Serial.println("  - Send 'AT+SIMCOMATI' to check firmware version");
    Serial.println("  - Send 'AT+CGMR' for modem revision");
    Serial.println("  - Send 'AT+CPIN?' to check SIM PIN status");
    Serial.println("  - Send 'AT+CPIN=\"0000\"' to unlock SIM (if locked)");
    Serial.println();
    Serial.println("Waiting for GPS signal...");
    Serial.println("(This may take several minutes outdoors)");
    Serial.println();

    // Initialize modem and unlock SIM
    Serial.println("==============================================");
    Serial.println("Initializing Modem...");
    Serial.println("==============================================");

    // Wait for modem to be ready after power-on sequence
    Serial.println("Waiting for modem to boot (5 seconds)...");
    delay(5000);

    // Test modem communication
    Serial.print("Testing AT communication... ");
    int retry = 0;
    while (retry < 5) {
        SerialAT.println("AT");
        delay(100);
        if (SerialAT.available()) {
            Serial.println("OK");
            while (SerialAT.available()) SerialAT.read(); // Clear buffer
            break;
        }
        retry++;
        delay(500);
    }
    if (retry >= 5) {
        Serial.println("FAILED - Modem not responding");
    }

    // Get modem info
    Serial.print("Modem info: ");
    modem.sendAT("+SIMCOMATI");
    delay(500);
    while (SerialAT.available()) {
        Serial.write(SerialAT.read());
    }
    Serial.println();

    // Check and unlock SIM
    Serial.print("Checking SIM card... ");
    delay(1000);
    int simStatus = modem.getSimStatus();

    if (simStatus == 0) {
        Serial.println("ERROR - No SIM detected!");
        Serial.println("Please insert SIM card and restart.");
    } else if (simStatus == 2) {
        Serial.println("Locked");
        Serial.print("Unlocking with PIN... ");
        if (modem.simUnlock(simPIN)) {
            Serial.println("OK");
        } else {
            Serial.println("FAILED - Check PIN code");
        }
    } else if (simStatus == 3) {
        Serial.println("ERROR - PUK required!");
    } else if (simStatus == 1) {
        Serial.println("OK (Ready)");
    }

    // Get SIM card ID
    Serial.print("SIM CCID: ");
    modem.sendAT("+CCID");
    delay(300);
    while (SerialAT.available()) {
        Serial.write(SerialAT.read());
    }
    Serial.println();

    // Check signal quality
    Serial.print("Signal quality: ");
    int16_t sq = modem.getSignalQuality();
    Serial.print(sq);
    if (sq > 0) {
        Serial.println(" (Good)");
    } else {
        Serial.println(" (No signal - check antenna)");
    }

    Serial.println("==============================================");
    Serial.println();
}

void loop()
{
    // Read GPS data and update display once per second
    while (SerialGPS.available()) {
        int c = SerialGPS.read();
        // Uncomment to debug GPS NMEA messages:
        // Serial.write(c);
        gps.encode(c);
    }

    // Display GPS info once per second
    if (millis() - lastGpsDisplay >= 5000) {
        if (gps.location.isUpdated() || gps.satellites.isUpdated()) {
            displayInfo();
            lastGpsDisplay = millis();
        }
    }

    // Check if GPS is detected
    if (millis() > 30000 && gps.charsProcessed() < 10) {
        Serial.println(F("No GPS detected: check wiring and antenna connection."));
        delay(1000);
    }

    // Button handler with debouncing (check every 50ms)
    if (millis() - lastButtonCheck >= 50) {
        bool currentButtonState = digitalRead(BOARD_BOOT_PIN);

        // Detect button press (HIGH to LOW transition)
        if (lastButtonState == HIGH && currentButtonState == LOW) {
            buttonPressed = true;
        }

        // Detect button release (LOW to HIGH transition)
        if (lastButtonState == LOW && currentButtonState == HIGH && buttonPressed) {
            buttonPressed = false;
            testInternetConnectivity();
        }

        lastButtonState = currentButtonState;
        lastButtonCheck = millis();
    }

    // Pass through modem AT commands with line buffering
    static String commandBuffer = "";

    // Modem to USB (byte-by-byte is fine for receiving)
    if (SerialAT.available()) {
        Serial.write(SerialAT.read());
    }

    // USB to Modem (buffer until newline to send complete AT commands)
    if (Serial.available()) {
        char c = Serial.read();

        // Check for newline characters
        if (c == '\n' || c == '\r') {
            // Send the buffered command (without the newline) followed by \r\n
            if (commandBuffer.length() > 0) {
                SerialAT.print(commandBuffer);
                SerialAT.print("\r\n");  // AT commands need CR+LF
                commandBuffer = "";  // Clear buffer for next command
            }
        } else {
            // Accumulate non-newline characters
            commandBuffer += c;
        }
    }

    delay(1);
}
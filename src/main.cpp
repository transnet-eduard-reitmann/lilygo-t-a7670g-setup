/**
 * @file      main.cpp
 * @author    LilyGO T-A7670G R2 GPS Setup
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2025-09-30
 * @note      This sketch is for T-A7670G R2 with L76K GPS Module (Model Q425)
 *            A7670G uses external GPS, not built-in GPS functionality
 */

#include <Arduino.h>
#include <TinyGPS++.h>

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

TinyGPSPlus gps;

void displayInfo()
{
    Serial.print(F("Location: "));
    if (gps.location.isValid()) {
        Serial.print(gps.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(gps.location.lng(), 6);
    } else {
        Serial.print(F("INVALID"));
    }

    Serial.print(F("  Date/Time: "));
    if (gps.date.isValid()) {
        Serial.print(gps.date.month());
        Serial.print(F("/"));
        Serial.print(gps.date.day());
        Serial.print(F("/"));
        Serial.print(gps.date.year());
    } else {
        Serial.print(F("INVALID"));
    }

    Serial.print(F(" "));
    if (gps.time.isValid()) {
        if (gps.time.hour() < 10) Serial.print(F("0"));
        Serial.print(gps.time.hour());
        Serial.print(F(":"));
        if (gps.time.minute() < 10) Serial.print(F("0"));
        Serial.print(gps.time.minute());
        Serial.print(F(":"));
        if (gps.time.second() < 10) Serial.print(F("0"));
        Serial.print(gps.time.second());
        Serial.print(F("."));
        if (gps.time.centisecond() < 10) Serial.print(F("0"));
        Serial.print(gps.time.centisecond());
    } else {
        Serial.print(F("INVALID"));
    }

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
    Serial.println("Waiting for GPS signal...");
    Serial.println("(This may take several minutes outdoors)");
    Serial.println();
}

void loop()
{
    // Read GPS data
    while (SerialGPS.available()) {
        int c = SerialGPS.read();
        // Uncomment to debug GPS NMEA messages:
        // Serial.write(c);
        if (gps.encode(c)) {
            displayInfo();
        }
    }

    // Check if GPS is detected
    if (millis() > 30000 && gps.charsProcessed() < 10) {
        Serial.println(F("No GPS detected: check wiring and antenna connection."));
        delay(1000);
    }

    // Pass through modem AT commands
    if (SerialAT.available()) {
        Serial.write(SerialAT.read());
    }
    if (Serial.available()) {
        SerialAT.write(Serial.read());
    }

    delay(1);
}
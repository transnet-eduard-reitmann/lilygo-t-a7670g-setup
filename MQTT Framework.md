# Self-Hosted MQTT Broker Setup (Windows Server, DuckDNS + Authentication)

This guide outlines the high-level steps to set up an MQTT broker (Eclipse Mosquitto) on a Windows Server, accessible over the internet with authentication and a dynamic DNS hostname (DuckDNS).

---

## 1. Prerequisites

- A physical Windows Server machine (company environment).
- Administrator rights on the server.
- Access to the company router/firewall (for port forwarding).
- A SIM-enabled IoT device (e.g., LilyGO T-A7670G) that will connect over cellular.

---

## 2. Applications Needed

1. **Mosquitto (MQTT Broker)**  
   - Official Eclipse Mosquitto Windows build.  
   - Handles all MQTT messaging.

2. **DuckDNS Client**  
   - Keeps your dynamic public IP synced with a hostname (e.g., `mycompany.duckdns.org`).  
   - Runs as a scheduled task or background service.

3. **Python 3 + Paho-MQTT (optional)**  
   - For testing subscriptions and processing GPS data.  
   - Not strictly required for broker operation, but useful for applications.

4. **Firewall/Router Access**  
   - To open port **1883** (MQTT) or **8883** (MQTT over TLS).  
   - Forward external traffic to your Windows Server.

---

## 3. Setup Framework

### Step 1: Install Mosquitto
- Download the Windows installer from the Eclipse Mosquitto site.
- Install as a **Windows Service** (so it runs automatically).
- Confirm broker runs locally on port 1883.

### Step 2: Configure Mosquitto
- Edit `mosquitto.conf`:
  - Enable `listener 1883`.
  - Add `allow_anonymous false`.
  - Configure password file (see below).
- Restart service after changes.

### Step 3: Add Authentication
- Use `mosquitto_passwd` to create a password file (`passwd`).
- Point to it in `mosquitto.conf`: password_file C:/mosquitto/config/passwd
- Restart broker.

### Step 4: Set Up DuckDNS
- Create a free account at [DuckDNS](https://www.duckdns.org/).
- Create a subdomain (e.g., `mycompany.duckdns.org`).
- Install/update DuckDNS client on Windows Server (PowerShell script or scheduled task).
- Verify the domain resolves to your server’s public IP.

### Step 5: Configure Router/Firewall
- Forward external TCP port `1883` → internal Windows Server IP.
- If using TLS, forward port `8883` instead.
- Ensure Windows Firewall allows inbound connections on the chosen port.

### Step 6: Test Connection
- From an external device (or mobile network):
- Publish a test message:
  ```
  mosquitto_pub -h mycompany.duckdns.org -t "test" -m "hello" -u USER -P PASS
  ```
- Subscribe and verify receipt:
  ```
  mosquitto_sub -h mycompany.duckdns.org -t "test" -u USER -P PASS
  ```

### Step 7: Run Python Application (Optional)
- Install Python 3.
- Install Paho-MQTT:

pip install paho-mqtt

- Create a subscriber script to handle GPS data from devices.

---

## 4. Next Steps (Optional Enhancements)
- Enable **TLS/SSL** for secure connections (port 8883).
- Set up **persistent storage** for MQTT messages.
- Integrate with a database (Postgres, SQLite) for GPS data.
- Build a dashboard (Flask + Leaflet.js or Grafana).

---

## 5. Maintenance
- Keep Mosquitto updated.
- Rotate passwords regularly.
- Monitor DuckDNS updates and ensure IP sync is reliable.
- Monitor logs (`mosquitto.log`) for failed connection attempts.

---

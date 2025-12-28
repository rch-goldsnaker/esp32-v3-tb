# ESP32 OTA ThingsBoard Example

This repository contains implementation examples of **Over-The-Air (OTA)** updates using **ESP32** and **ThingsBoard**. 

## 📋 Description

This project demonstrates how to implement remote firmware updates (OTA) through ThingsBoard.

### 💡 Example Use Case: Temperature and Humidity Monitoring

This project includes a practical example of version management where:

- **Version 1.0.0**: Configured to send **temperature** telemetry data to ThingsBoard
- **Version 1.0.1**: Updated version configured to send **humidity** telemetry data instead

This demonstrates how OTA updates can be used to change device functionality remotely without physical access, perfect for:
- Switching between different sensor readings
- Updating device behavior based on deployment needs
- Testing different telemetry configurations in production environments

## 🔧 Required Hardware

- **Heltec WiFi LoRa 32 V3** (or compatible ESP32 with OLED display)
- WiFi connection
- ThingsBoard Cloud account


## 📚 Libraries Used

- `ThingsBoard` (0.14.0) - ThingsBoard client for ESP32
- `ArduinoMqttClient` - MQTT client
- `ArduinoHttpClient` - HTTP client
- `PubSubClient` - Additional MQTT client


## 🚀 Installation and Configuration

### 1. Prerequisites

- [PlatformIO](https://platformio.org/) installed in VS Code
- [ThingsBoard Cloud](https://thingsboard.cloud/) account

### 2. Configure Credentials

**⚠️ IMPORTANT**: Before compiling the project, you must create the credentials file.

1. Navigate to the `include/` folder
2. Copy the file `credentials.template.h` and rename it to `credentials.h`
3. Edit `credentials.h` with your actual data:

```cpp
// ===========================================
// WiFi Configuration
// ===========================================
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// ===========================================
// ThingsBoard Device Token
// ===========================================
#define TB_TOKEN "YOUR_THINGSBOARD_TOKEN"

// ===========================================
// ThingsBoard Server Configuration
// ===========================================
#define TB_SERVER "YOUR_THINGSBOARD_SERVER_IP"
```

### 3. Configure ThingsBoard

1. Create an account on [ThingsBoard Cloud](https://thingsboard.cloud/)
2. Create a new device in your dashboard
3. Copy the **Device Token** and use it in `credentials.h`

### 4. Select Version

Copy the content from the desired version's `main.cpp` file to `src/main.cpp`:

- For version 1.0.0 (Temperature): `Version CORE_ESP32/v1.0.0/main.cpp` → `src/main.cpp`
- For version 1.0.1 (Humidity): `Version CORE_ESP32/v1.0.1/main.cpp` → `src/main.cpp`


## 📊 Available Versions

#### Version 1.0.0 - Temperature Sensor
- **Telemetry**: Sends temperature data (°C)
- **Purpose**: Monitor ambient temperature
- **Data Sent**:
  ```cpp
  temperature: 25.4°C  // Simulated or from sensor
  ```
- **Use Case**: Temperature monitoring in server rooms, greenhouses, or climate control systems

#### Version 1.0.1 - Humidity Sensor
- **Telemetry**: Sends humidity data (%)
- **Purpose**: Monitor relative humidity
- **Data Sent**:
  ```cpp
  humidity: 65.2%  // Simulated or from sensor
  ```
- **Use Case**: Humidity monitoring in storage facilities, museums, or agricultural environments

This example demonstrates how a single device can be repurposed through OTA updates:
- Deploy v1.0.0 to monitor temperature
- Update to v1.0.1 via ThingsBoard dashboard to switch to humidity monitoring
- No physical access required - update happens remotely
- Perfect for testing different sensor configurations or changing requirements

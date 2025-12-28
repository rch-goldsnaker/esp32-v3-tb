// ESP32 Libraries
#include <WiFi.h>
#include <Arduino_MQTT_Client.h>
#include <OTA_Firmware_Update.h>
#include <ThingsBoard.h>
#include <Espressif_Updater.h>

// Configuration files
#include "credentials.h"

// Firmware title and version used to compare with remote version, to check if an update is needed.
// Title needs to be the same and version needs to be different --> downgrading is possible
constexpr char CURRENT_FIRMWARE_TITLE[] = "CORE_ESP32";
constexpr char CURRENT_FIRMWARE_VERSION[] = "1.0.0";

// Maximum amount of retries we attempt to download each firmware chunck over MQTT
constexpr uint8_t FIRMWARE_FAILURE_RETRIES = 12U;

// Size of each firmware chunck downloaded over MQTT,
// increased packet size, might increase download speed
constexpr uint16_t FIRMWARE_PACKET_SIZE = 16384U;

constexpr char wifiSSID[] = WIFI_SSID;
constexpr char wifiPassword[] = WIFI_PASSWORD;
constexpr char token[] = TB_TOKEN;

// Thingsboard server configuration
constexpr char THINGSBOARD_SERVER[] = TB_SERVER;
constexpr uint16_t THINGSBOARD_PORT = 1883U;

// Maximum size packets will ever be sent or received by the underlying MQTT client,
// if the size is to small messages might not be sent or received messages will be discarded
constexpr uint16_t MAX_MESSAGE_SEND_SIZE = 512U;
constexpr uint16_t MAX_MESSAGE_RECEIVE_SIZE = 512U;

// Baud rate for the debugging serial connection
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;

// Telemetry sending interval (milliseconds)
constexpr uint32_t telemetrySendInterval = 5000U;
uint32_t previousDataSend = 0;

// Initialize ESP32 WiFi client
WiFiClient espClient;
// Initialize MQTT client and ThingsBoard
Arduino_MQTT_Client mqttClient(espClient);
OTA_Firmware_Update<> ota;
const std::array<IAPI_Implementation*, 1U> apis = { &ota };
ThingsBoard tb(mqttClient, MAX_MESSAGE_RECEIVE_SIZE, MAX_MESSAGE_SEND_SIZE, apis);

// Initialize ESP32 Updater for OTA
Espressif_Updater<> updater;

// Statuses for updating
bool currentFWSent = false;
bool updateRequestSent = false;


// Initializes WiFi connection
void InitWiFi() {
  Serial.println("Connecting to AP ...");
  WiFi.begin(wifiSSID, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

// Reconnects WiFi if connection is lost
bool reconnect() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  InitWiFi();
  return true;
}

// Called when OTA update starts
void update_starting_callback() {
  Serial.println("[OTA] Update starting - New firmware version detected!");
}

// Called when OTA update finishes
void finished_callback(const bool & success) {
  if (success) {
    Serial.println("[OTA] Update completed successfully! Rebooting...");
    esp_restart();
    return;
  }
  Serial.println("[OTA] Firmware download failed");
}

// Shows OTA update progress
void progress_callback(const size_t & current, const size_t & total) {
  Serial.printf("Progress %.2f%%\n", static_cast<float>(current * 100U) / total);
}

void setup() {
  // Initialize serial connection
  Serial.begin(SERIAL_DEBUG_BAUD);
  delay(1000);
  
  // Show firmware information
  Serial.println("========================================");
  Serial.printf("Firmware: %s\n", CURRENT_FIRMWARE_TITLE);
  Serial.printf("Version: %s\n", CURRENT_FIRMWARE_VERSION);
  Serial.println("========================================");
  
  InitWiFi();
}

void loop() {
  delay(1000);

  if (!reconnect()) {
    return;
  }

  if (!tb.connected()) {
    Serial.printf("Connecting to: (%s) with token (%s)\n", THINGSBOARD_SERVER, token);
    if (!tb.connect(THINGSBOARD_SERVER, token, THINGSBOARD_PORT)) {
      Serial.println("Failed to connect");
      return;
    }
  }

  if (!currentFWSent) {
    currentFWSent = ota.Firmware_Send_Info(CURRENT_FIRMWARE_TITLE, CURRENT_FIRMWARE_VERSION);
    if (currentFWSent) {
      Serial.printf("[OTA] Current firmware info sent: %s v%s\n", CURRENT_FIRMWARE_TITLE, CURRENT_FIRMWARE_VERSION);
    }
  }

  // Check for updates
  if (!updateRequestSent) {
    Serial.println("[OTA] Checking for firmware updates...");
    const OTA_Update_Callback callback(CURRENT_FIRMWARE_TITLE, CURRENT_FIRMWARE_VERSION, &updater, &finished_callback, &progress_callback, &update_starting_callback, FIRMWARE_FAILURE_RETRIES, FIRMWARE_PACKET_SIZE);
    updateRequestSent = ota.Start_Firmware_Update(callback);
    
    if (updateRequestSent) {
      Serial.println("[OTA] Update request initiated successfully");
    } else {
      Serial.println("[OTA] No update available or same version detected");
    }
  }

  // Sending telemetry every telemetrySendInterval time
  if (millis() - previousDataSend > telemetrySendInterval) {
    previousDataSend = millis();
    tb.sendTelemetryData("temperature", random(10, 20));
    tb.sendAttributeData("rssi", WiFi.RSSI());
    tb.sendAttributeData("channel", WiFi.channel());
    tb.sendAttributeData("bssid", WiFi.BSSIDstr().c_str());
    tb.sendAttributeData("localIp", WiFi.localIP().toString().c_str());
    tb.sendAttributeData("ssid", WiFi.SSID().c_str());
    Serial.println("[TELEMETRY] Data sent to ThingsBoard");
  }

  tb.loop();
}
#include "WiFiConfigManager.h"
#include <FreeRTOS.h>
#include <task.h>
#include <WiFi.h> // Include WiFi library for network operations
#include <LittleFS.h>
#include <ArduinoJson.h>

WiFiConfigManager::WiFiConfigManager(const String& configFilePath)
    : configFilePath(configFilePath) {}

WiFiConfigManager::~WiFiConfigManager() {
    stopSSIDScan();
}

void WiFiConfigManager::startSSIDScan(const std::function<void(const String&)>& onSSIDFound) {
    if (scanning) {
        return; // Scan already running
    }

    scanning = true;

    // Create a FreeRTOS task for scanning SSIDs
    xTaskCreate(
        scanTask,                // Task function
        "SSIDScanTask",          // Task name
        4096,                    // Stack size
        new std::pair<WiFiConfigManager*, std::function<void(const String&)>>(this, onSSIDFound), // Pass the instance and callback as a parameter
        1,                       // Task priority
        &scanTaskHandle          // Task handle
    );
}

void WiFiConfigManager::stopSSIDScan() {
    if (scanning && scanTaskHandle != nullptr) {
        scanning = false;
        vTaskDelete(scanTaskHandle); // Delete the FreeRTOS task
        scanTaskHandle = nullptr;
    }
}

void WiFiConfigManager::scanTask(void* parameter) {
    // Retrieve the callback function passed as a parameter
    auto param = static_cast<std::pair<WiFiConfigManager*, std::function<void(const String&)>>*>(parameter);
    WiFiConfigManager* instance = param->first;
    auto onSSIDFound = &param->second;

    // Perform the WiFi scan
    while (WiFi.scanNetworks() == -1) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for WiFi hardware to initialize
    }

    while (true) {
            if (!instance->scanning) {
                delete param; // Clean up the parameter
            if (!instance->scanning) {
                delete onSSIDFound; // Clean up the callback
                vTaskDelete(nullptr); // End the task
            }

            // Call the callback with the SSID
            (*onSSIDFound)(WiFi.SSID(i));
        }

        vTaskDelay(pdMS_TO_TICKS(5000)); // Wait before rescanning
    }
}

void WiFiConfigManager::saveNetworkProperties(const NetworkProperties& properties) {
    File file = LittleFS.open(configFilePath, "w");
    if (!file) {
        return; // Failed to open file
    }

    // Allocate a JSON document
    StaticJsonDocument<512> doc; // Adjust size as needed

    // Populate the JSON document
    doc["ssid"] = properties.ssid;
    doc["password"] = properties.password;
    doc["ip"] = properties.ip;
    doc["mask"] = properties.mask;
    doc["router"] = properties.router;
    doc["dns"] = properties.dns;
    doc["dhcp"] = properties.dhcp;

    // Serialize the JSON document to the file
    if (serializeJson(doc, file) == 0) {
        // Handle serialization error (optional)
    }

    file.close();
}

WiFiConfigManager::NetworkProperties WiFiConfigManager::loadNetworkProperties() {
    NetworkProperties properties;

    File file = LittleFS.open(configFilePath, "r");
    if (!file) {
        return properties; // Failed to open file
    }

    // Allocate a JSON document
    StaticJsonDocument<512> doc; // Adjust size as needed

    // Parse the JSON file
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        // Return default properties if parsing fails
        return properties;
    }

    // Extract values from the JSON document
    properties.ssid = doc["ssid"] | ""; // Default to empty string if not found
    properties.password = doc["password"] | "";
    properties.ip = doc["ip"] | "";
    properties.mask = doc["mask"] | "";
    properties.router = doc["router"] | "";
    properties.dns = doc["dns"] | "";
    properties.dhcp = doc["dhcp"] | true; // Default to true if not found

    return properties;
}

void WiFiConfigManager::startNetwork() {
    NetworkProperties properties = loadNetworkProperties();
    if (properties.dhcp) {
        WiFi.begin(properties.ssid.c_str(), properties.password.c_str());
    } else {
        WiFi.config(
            IPAddress().fromString(properties.ip),
            IPAddress().fromString(properties.router),
            IPAddress().fromString(properties.mask),
            IPAddress().fromString(properties.dns)
        );
        WiFi.begin(properties.ssid.c_str(), properties.password.c_str());
    }
}

void WiFiConfigManager::stopNetwork() {
    WiFi.disconnect();
}

bool WiFiConfigManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

WiFiConfigManager::ConnectionInfo WiFiConfigManager::getConnectionInfo() {
    ConnectionInfo info;
    
    // Only fill in data if we're connected
    if (isConnected()) {
        info.ip = WiFi.localIP().toString();
        info.subnet = WiFi.subnetMask().toString();
        info.gateway = WiFi.gatewayIP().toString();
        info.dns = WiFi.dnsIP().toString();
        info.ssid = WiFi.SSID();
        info.rssi = WiFi.RSSI();
        info.macAddress = WiFi.macAddress();
    } else {
        // Fill with empty values or "Not Connected"
        info.ip = "Not Connected";
        info.subnet = "Not Connected";
        info.gateway = "Not Connected";
        info.dns = "Not Connected";
        info.ssid = "Not Connected";
        info.rssi = 0;
        info.macAddress = WiFi.macAddress(); // MAC is available even when disconnected
    }
    
    return info;
}


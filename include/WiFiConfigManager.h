#pragma once

#include <Arduino.h> // For Arduino's String class
#include <functional>
#include <FreeRTOS.h> // Include FreeRTOS header for TaskHandle_t
#include <task.h>     // Include FreeRTOS task header

class WiFiConfigManager {
public:
    // Get the singleton instance
    static WiFiConfigManager& getInstance(const String& configFilePath = "") {
        static WiFiConfigManager instance(configFilePath);
        return instance;
    }

    // Delete copy/move constructors and assignment operators
    WiFiConfigManager(const WiFiConfigManager&) = delete;
    WiFiConfigManager& operator=(const WiFiConfigManager&) = delete;
    WiFiConfigManager(WiFiConfigManager&&) = delete;
    WiFiConfigManager& operator=(WiFiConfigManager&&) = delete;

    // Network properties structure
    struct NetworkProperties {
        String ssid;
        String password;
        String ip;      // Empty string indicates no IP address
        String mask;    // Empty string indicates no subnet mask
        String router;  // Empty string indicates no router address
        String dns;     // Empty string indicates no DNS address
        bool dhcp = true; // Default to DHCP
    };

    // Connection information structure
    struct ConnectionInfo {
        String ip;
        String subnet;
        String gateway;
        String dns;
        String ssid;
        int32_t rssi;
        String macAddress;
    };

    ~WiFiConfigManager();

    // Start scanning for SSIDs
    void startSSIDScan(const std::function<void(const String&)>& onSSIDFound);

    // Stop scanning for SSIDs
    void stopSSIDScan();

    // Save network properties to a file
    void saveNetworkProperties(const NetworkProperties& properties);

    // Load network properties from a file
    NetworkProperties loadNetworkProperties();

    // Start the network connection
    void startNetwork();

    // Stop the network connection
    void stopNetwork();

    // Check if the device is connected to a network
    bool isConnected();

    // Get connection information
    ConnectionInfo getConnectionInfo();

private:
    // Private constructor for singleton pattern
    WiFiConfigManager(const String& configFilePath);
    
    String configFilePath;
    bool scanning = false;
    TaskHandle_t scanTaskHandle = nullptr; // Handle for the FreeRTOS task

    // FreeRTOS task for scanning SSIDs
    static void scanTask(void* parameter);
};
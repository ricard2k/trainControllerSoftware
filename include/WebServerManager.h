#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "WiFiConfigManager.h"
#include "LocoCommandManager.h"

class WebServerManager {
public:
    WebServerManager(WiFiConfigManager* wifiManager, LocoCommandManager* locoManager);
    ~WebServerManager();

    // Initialize and start the web server
    void begin();
    
    // Stop the web server
    void stop();
    
    // Check if the server is running
    bool isRunning() const;

private:
    AsyncWebServer server;
    WiFiConfigManager* wifiManager;
    LocoCommandManager* locoManager;
    bool running = false;
    
    // Setup routes for the web server
    void setupRoutes();
    
    // API endpoints
    void handleRoot(AsyncWebServerRequest *request);
    void handleGetWiFiStatus(AsyncWebServerRequest *request);
    
    // Helper methods
    void sendJsonResponse(AsyncWebServerRequest *request, JsonDocument& doc);
    void sendErrorResponse(AsyncWebServerRequest *request, int code, const String& message);
};
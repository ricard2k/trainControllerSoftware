#include "WebServerManager.h"
#include <LittleFS.h>

WebServerManager::WebServerManager(WiFiConfigManager* wifiManager, LocoCommandManager* locoManager)
    : server(443), wifiManager(wifiManager), locoManager(locoManager) {
}

WebServerManager::~WebServerManager() {
    stop();
}

void WebServerManager::begin() {
    if (running || !wifiManager->isConnected()) {
        return;
    }

    // Setup all API routes
    setupRoutes();

    // Start server with SSL
    server.begin();
    running = true;
}

void WebServerManager::stop() {
    if (running) {
        server.end();
        running = false;
    }
}

bool WebServerManager::isRunning() const {
    return running;
}

void WebServerManager::setupRoutes() {
    // Serve static files from LittleFS (for web interface)
    server.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");

    // API routes
    server.on("/api/wifi/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
        handleGetWiFiStatus(request);
    });
    
    // Not found handler
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found");
    });
}

void WebServerManager::handleGetWiFiStatus(AsyncWebServerRequest *request) {
    WiFiConfigManager::ConnectionInfo info = wifiManager->getConnectionInfo();
    
    JsonDocument doc;
    doc["connected"] = wifiManager->isConnected();
    doc["ssid"] = info.ssid;
    doc["ip"] = info.ip;
    doc["subnet"] = info.subnet;
    doc["gateway"] = info.gateway;
    doc["rssi"] = info.rssi;
    doc["mac"] = info.macAddress;
    
    sendJsonResponse(request, doc);
}


void WebServerManager::sendJsonResponse(AsyncWebServerRequest *request, JsonDocument& doc) {
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
}

void WebServerManager::sendErrorResponse(AsyncWebServerRequest *request, int code, const String& message) {
    JsonDocument doc;
    doc["success"] = false;
    doc["error"] = message;
    
    String response;
    serializeJson(doc, response);
    request->send(code, "application/json", response);
}

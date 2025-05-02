#include "WebServerManager.h"
#include "cert.h" // Include SSL certificate
#include "key.h"  // Include private key

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
    server.beginSecure(cert_pem, cert_pem_len, key_pem, key_pem_len);
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

    server.on("/api/train/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
        handleGetTrainStatus(request);
    });
    
    server.on("/api/train/speed", HTTP_POST, [this](AsyncWebServerRequest *request) {
        handleSetSpeed(request);
    });
    
    server.on("/api/train/brake", HTTP_POST, [this](AsyncWebServerRequest *request) {
        handleSetBrake(request);
    });
    
    server.on("/api/train/lights", HTTP_POST, [this](AsyncWebServerRequest *request) {
        handleSetLights(request);
    });
    
    server.on("/api/train/controls", HTTP_POST, [this](AsyncWebServerRequest *request) {
        handleSetControls(request);
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

void WebServerManager::handleGetTrainStatus(AsyncWebServerRequest *request) {
    JsonDocument doc;
    
    // We'll use std::optional's has_value() to check if values have been set
    doc["speed"] = locoManager->lastSpeed.has_value() ? locoManager->lastSpeed.value() : 0;
    doc["brake"] = locoManager->lastBrake.has_value() ? locoManager->lastBrake.value() : 0;
    
    if (locoManager->lastFrontLights.has_value()) {
        doc["frontLights"] = getLightStatusString(locoManager->lastFrontLights.value());
    } else {
        doc["frontLights"] = "OFF";
    }
    
    if (locoManager->lastBackLights.has_value()) {
        doc["backLights"] = getLightStatusString(locoManager->lastBackLights.value());
    } else {
        doc["backLights"] = "OFF";
    }
    
    doc["bell"] = locoManager->lastBell.has_value() ? locoManager->lastBell.value() : false;
    doc["horn"] = locoManager->lastHorn.has_value() ? locoManager->lastHorn.value() : false;
    
    sendJsonResponse(request, doc);
}

void WebServerManager::handleSetSpeed(AsyncWebServerRequest *request) {
    if (request->hasParam("value", true)) {
        String speedStr = request->getParam("value", true)->value();
        int speed = speedStr.toInt();
        
        // Validate speed range
        if (speed >= 0 && speed <= 100) {
            locoManager->setSpeed(speed);
            
            JsonDocument doc;
            doc["success"] = true;
            doc["speed"] = speed;
            sendJsonResponse(request, doc);
        } else {
            sendErrorResponse(request, 400, "Speed must be between 0 and 100");
        }
    } else {
        sendErrorResponse(request, 400, "Missing 'value' parameter");
    }
}

void WebServerManager::handleSetBrake(AsyncWebServerRequest *request) {
    if (request->hasParam("value", true)) {
        String brakeStr = request->getParam("value", true)->value();
        int brake = brakeStr.toInt();
        
        // Validate brake range
        if (brake >= 0 && brake <= 100) {
            locoManager->setBrake(brake);
            
            JsonDocument doc;
            doc["success"] = true;
            doc["brake"] = brake;
            sendJsonResponse(request, doc);
        } else {
            sendErrorResponse(request, 400, "Brake must be between 0 and 100");
        }
    } else {
        sendErrorResponse(request, 400, "Missing 'value' parameter");
    }
}

void WebServerManager::handleSetLights(AsyncWebServerRequest *request) {
    if (!request->hasParam("position", true) || !request->hasParam("status", true)) {
        sendErrorResponse(request, 400, "Missing 'position' or 'status' parameter");
        return;
    }

    String position = request->getParam("position", true)->value();
    String status = request->getParam("status", true)->value();
    
    LocoCommandManager::LightStatus lightStatus = parseLightStatus(status);
    
    if (position.equalsIgnoreCase("front")) {
        locoManager->setFrontLights(lightStatus);
    } else if (position.equalsIgnoreCase("back")) {
        locoManager->setBackLights(lightStatus);
    } else {
        sendErrorResponse(request, 400, "Position must be 'front' or 'back'");
        return;
    }
    
    JsonDocument doc;
    doc["success"] = true;
    doc["position"] = position;
    doc["status"] = status;
    sendJsonResponse(request, doc);
}

void WebServerManager::handleSetControls(AsyncWebServerRequest *request) {
    if (!request->hasParam("control", true) || !request->hasParam("active", true)) {
        sendErrorResponse(request, 400, "Missing 'control' or 'active' parameter");
        return;
    }

    String control = request->getParam("control", true)->value();
    String activeStr = request->getParam("active", true)->value();
    bool active = (activeStr == "1" || activeStr.equalsIgnoreCase("true"));
    
    if (control.equalsIgnoreCase("bell")) {
        locoManager->setBell(active);
    } else if (control.equalsIgnoreCase("horn")) {
        locoManager->setHorn(active);
    } else {
        sendErrorResponse(request, 400, "Control must be 'bell' or 'horn'");
        return;
    }
    
    JsonDocument doc;
    doc["success"] = true;
    doc["control"] = control;
    doc["active"] = active;
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

String WebServerManager::getLightStatusString(LocoCommandManager::LightStatus status) {
    switch (status) {
        case LocoCommandManager::LightStatus::OFF:
            return "OFF";
        case LocoCommandManager::LightStatus::DIMM:
            return "DIMM";
        case LocoCommandManager::LightStatus::BRIGHT:
            return "BRIGHT";
        case LocoCommandManager::LightStatus::DITCHES:
            return "DITCHES";
        default:
            return "UNKNOWN";
    }
}

LocoCommandManager::LightStatus WebServerManager::parseLightStatus(const String& statusStr) {
    if (statusStr.equalsIgnoreCase("OFF")) {
        return LocoCommandManager::LightStatus::OFF;
    } else if (statusStr.equalsIgnoreCase("DIMM")) {
        return LocoCommandManager::LightStatus::DIMM;
    } else if (statusStr.equalsIgnoreCase("BRIGHT")) {
        return LocoCommandManager::LightStatus::BRIGHT;
    } else if (statusStr.equalsIgnoreCase("DITCHES")) {
        return LocoCommandManager::LightStatus::DITCHES;
    } else {
        return LocoCommandManager::LightStatus::OFF; // Default
    }
}
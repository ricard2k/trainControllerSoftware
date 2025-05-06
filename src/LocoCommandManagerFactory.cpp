#include "LocoCommandManagerFactory.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>

// Constructor now takes the file path as parameter
LocoCommandManagerFactory::LocoCommandManagerFactory(const char* filePath)
    : configFilePath(filePath), connectionUrl(""), currentManagerType(ManagerType::DccEx), isInitialized(false) {
    loadConfiguration();
}

bool LocoCommandManagerFactory::loadConfiguration() {
    if (!LittleFS.begin(true)) {
        return createDefaultConfigFile(); // If mounting fails, create a default config
    }

    if (!LittleFS.exists(configFilePath)) {
        return createDefaultConfigFile(); // If file doesn't exist, create it
    }

    File configFile = LittleFS.open(configFilePath, "r");
    if (!configFile) {
        return createDefaultConfigFile(); // If file can't be opened, create it
    }

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, configFile);
    configFile.close();

    if (error) {
        return createDefaultConfigFile(); // If parsing fails, recreate the file
    }

    const char* managerType = doc["managerType"];
    if (managerType && strcmp(managerType, "JMRI") == 0) {
        currentManagerType = ManagerType::JMRI;
    } else {
        currentManagerType = ManagerType::DccEx;
    }
    
    // Load connection URL if available
    const char* url = doc["connectionUrl"];
    if (url) {
        connectionUrl = String(url);
    }

    return true;
}

// New method to create default configuration file
bool LocoCommandManagerFactory::createDefaultConfigFile() {
    // Ensure file system is mounted
    if (!LittleFS.begin(true)) {
        return false;
    }
    
    // Set default manager type
    currentManagerType = ManagerType::DccEx;
    connectionUrl = ""; // Default empty URL
    
    return saveConfiguration();
}

bool LocoCommandManagerFactory::saveConfiguration() {
    // Ensure file system is mounted
    if (!LittleFS.begin(true)) {
        return false;
    }
    
    // Create configuration file
    File configFile = LittleFS.open(configFilePath, "w");
    if (!configFile) {
        return false;
    }
    
    StaticJsonDocument<256> doc;
    doc["managerType"] = (currentManagerType == ManagerType::JMRI) ? "JMRI" : "DccEx";
    doc["connectionUrl"] = connectionUrl;
    
    // Write JSON to file
    if (serializeJson(doc, configFile) == 0) {
        configFile.close();
        return false;
    }
    
    configFile.close();
    // Reset the initialized flag to recreate the manager with new settings
    isInitialized = false;
    return true;
}

bool LocoCommandManagerFactory::setManagerType(ManagerType type) {
    currentManagerType = type;
    return saveConfiguration();
}

bool LocoCommandManagerFactory::setConnectionUrl(const String& url) {
    connectionUrl = url;
    return saveConfiguration();
}

LocoCommandManager* LocoCommandManagerFactory::getLocoCommandManager() {
    if (!isInitialized) {
        // Create the appropriate manager based on configuration
        if (currentManagerType == ManagerType::JMRI) {
            commandManager = std::make_unique<JMRICommandManager>();
            if (!connectionUrl.isEmpty()) {
                commandManager->connect(connectionUrl);
            }
        } else {
            commandManager = std::make_unique<DccExCommandManager>();
            if (!connectionUrl.isEmpty()) {
                commandManager->connect(connectionUrl);
            }
        }
        isInitialized = true;
    }
    
    return commandManager.get();
}
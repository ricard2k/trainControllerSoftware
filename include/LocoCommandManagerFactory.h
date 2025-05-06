#pragma once

#include "LocoCommandManager.h"
#include "DccExCommandManager.h"
#include "JMRICommandManager.h"
#include <ArduinoJson.h>
#include <memory>

class LocoCommandManagerFactory {
public:
    // Delete copy/move constructors and assignment operators
    LocoCommandManagerFactory(const LocoCommandManagerFactory&) = delete;
    LocoCommandManagerFactory& operator=(const LocoCommandManagerFactory&) = delete;
    LocoCommandManagerFactory(LocoCommandManagerFactory&&) = delete;
    LocoCommandManagerFactory& operator=(LocoCommandManagerFactory&&) = delete;

    // Get the singleton instance with configurable file path
    static LocoCommandManagerFactory& getInstance(const char* configFilePath = "/locoCommandManager.json") {
        static LocoCommandManagerFactory instance(configFilePath);
        return instance;
    }

    // Get the appropriate LocoCommandManager implementation
    LocoCommandManager* getLocoCommandManager();
    
    // Enum for manager types
    enum class ManagerType {
        DccEx,
        JMRI
    };
    
    // Get current manager type
    ManagerType getManagerType() const {
        return currentManagerType;
    }
    
    // Get connection URL
    String getConnectionUrl() const {
        return connectionUrl;
    }
    
    // Set manager type and save configuration
    bool setManagerType(ManagerType type);
    
    // Set connection URL and save configuration
    bool setConnectionUrl(const String& url);
    
    // Save current configuration to file
    bool saveConfiguration();

private:
    // Private constructor for singleton pattern that accepts a file path
    LocoCommandManagerFactory(const char* filePath);
    
    // Load configuration from JSON file
    bool loadConfiguration();
    
    // Create default configuration file if it doesn't exist
    bool createDefaultConfigFile();
    
    // The current command manager instance
    std::unique_ptr<LocoCommandManager> commandManager;
    
    // Configuration file path
    const char* configFilePath;
    
    // Connection URL for the command manager
    String connectionUrl;
    
    // Type of command manager to use (DccEx or JMRI)
    ManagerType currentManagerType;
    bool isInitialized;
};
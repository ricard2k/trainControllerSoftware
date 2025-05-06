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
    
    // Type of command manager to use (DccEx or JMRI)
    enum class ManagerType {
        DccEx,
        JMRI
    };
    
    ManagerType currentManagerType;
    bool isInitialized;
};
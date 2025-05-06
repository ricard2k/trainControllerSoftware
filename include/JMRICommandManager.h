#pragma once

#include "LocoCommandManager.h"
#include <Arduino.h> // For Arduino's String class

class JMRICommandManager : public LocoCommandManager {
public:
    // Get the singleton instance
    static JMRICommandManager& getInstance() {
        static JMRICommandManager instance;
        return instance;
    }
    
    void initialize() override;
    void shutdown() override;
    void sendCommand(const String& command) override;

protected:
    void sendSpeedCommand(int speed) override;
    void sendBrakeCommand(int brake) override;
    void sendFrontLightsCommand(LightStatus status) override;
    void sendBackLightsCommand(LightStatus status) override;
    void sendBellCommand(bool active) override;
    void sendHornCommand(bool active) override;

private:
    // Private constructor for singleton pattern
    JMRICommandManager() {}
    
    String lightStatusToString(LightStatus status);
};
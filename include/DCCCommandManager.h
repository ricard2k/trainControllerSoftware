#pragma once

#include "LocoCommandManager.h"
#include <Arduino.h> // For Arduino's String class

class DCCCommandManager : public LocoCommandManager {
public:
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
    String lightStatusToString(LightStatus status);
};
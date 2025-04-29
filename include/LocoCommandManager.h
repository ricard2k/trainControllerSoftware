#pragma once

#include <optional>
#include <Arduino.h> // For Arduino's String class

class LocoCommandManager {
public:
    // Enum for light statuses
    enum class LightStatus {
        OFF,
        DIMM,
        BRIGHT,
        DITCHES
    };

    virtual ~LocoCommandManager() {}

    // Initialize the system
    virtual void initialize() = 0;

    // Shutdown the system
    virtual void shutdown() = 0;

    // Send a generic command
    virtual void sendCommand(const String& command) = 0;

    // Set speed value (0-100%)
    void setSpeed(int speed);

    // Set brake value (0-100%)
    void setBrake(int brake);

    // Set front lights status
    void setFrontLights(LightStatus status);

    // Set back lights status
    void setBackLights(LightStatus status);

    // Activate or deactivate the bell
    void setBell(bool active);

    // Activate or deactivate the horn
    void setHorn(bool active);

protected:
    // State variables to track the last sent values
    std::optional<int> lastSpeed;
    std::optional<int> lastBrake;
    std::optional<LightStatus> lastFrontLights;
    std::optional<LightStatus> lastBackLights;
    std::optional<bool> lastBell;
    std::optional<bool> lastHorn;

    // Pure virtual methods for derived classes to implement specific commands
    virtual void sendSpeedCommand(int speed) = 0;
    virtual void sendBrakeCommand(int brake) = 0;
    virtual void sendFrontLightsCommand(LightStatus status) = 0;
    virtual void sendBackLightsCommand(LightStatus status) = 0;
    virtual void sendBellCommand(bool active) = 0;
    virtual void sendHornCommand(bool active) = 0;
};

#endif // LOCO_COMMAND_MANAGER_H
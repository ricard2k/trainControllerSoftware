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

    // Delete copy/move constructors and assignment operators to prevent duplication
    LocoCommandManager(const LocoCommandManager&) = delete;
    LocoCommandManager& operator=(const LocoCommandManager&) = delete;
    LocoCommandManager(LocoCommandManager&&) = delete;
    LocoCommandManager& operator=(LocoCommandManager&&) = delete;

    virtual ~LocoCommandManager() {}

    // Connect to the system with the specified connection URL
    virtual void connect(const String& connectionUrl) = 0;

    // Disconnect from the system
    virtual void disconnect() = 0;

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
    // Protected constructor for singleton pattern
    LocoCommandManager() {}

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
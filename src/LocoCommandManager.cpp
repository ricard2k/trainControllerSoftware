#include "LocoCommandManager.h"

void LocoCommandManager::setSpeed(int speed) {
    if (!lastSpeed.has_value() || lastSpeed.value() != speed) {
        lastSpeed = speed;
        sendSpeedCommand(speed);
    }
}

void LocoCommandManager::setBrake(int brake) {
    if (!lastBrake.has_value() || lastBrake.value() != brake) {
        lastBrake = brake;
        sendBrakeCommand(brake);
    }
}

void LocoCommandManager::setFrontLights(LightStatus status) {
    if (!lastFrontLights.has_value() || lastFrontLights.value() != status) {
        lastFrontLights = status;
        sendFrontLightsCommand(status);
    }
}

void LocoCommandManager::setBackLights(LightStatus status) {
    if (!lastBackLights.has_value() || lastBackLights.value() != status) {
        lastBackLights = status;
        sendBackLightsCommand(status);
    }
}

void LocoCommandManager::setBell(bool active) {
    if (!lastBell.has_value() || lastBell.value() != active) {
        lastBell = active;
        sendBellCommand(active);
    }
}

void LocoCommandManager::setHorn(bool active) {
    if (!lastHorn.has_value() || lastHorn.value() != active) {
        lastHorn = active;
        sendHornCommand(active);
    }
}